//------------------------------------------------------------------------------
// materialwindow.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "materialwindow.h"
#include "io/ioserver.h"
#include "io/xmlwriter.h"
#include "shadywindow.h"
#include "QFileDialog"
#include "QMessageBox"
#include "QInputDialog"
#include "materials/materialserver.h"
#include "variable/shadyvariable.h"
#include "coregraphics/shaderserver.h"

using namespace IO;
namespace Shady
{

__ImplementClass(Shady::MaterialWindow, 'MTWI', Core::RefCounted);
//------------------------------------------------------------------------------
/**
*/
MaterialWindow::MaterialWindow()
{
	this->ui.setupUi(this);
}

//------------------------------------------------------------------------------
/**
*/
MaterialWindow::~MaterialWindow()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::Setup()
{
	QObject::connect(this->ui.actionOpen_list, SIGNAL(triggered()), this, SLOT(OnOpenMaterialList()));
	QObject::connect(this->ui.actionNew_list, SIGNAL(triggered()), this, SLOT(OnNewMaterialList()));
	QObject::connect(this->ui.actionSave_list, SIGNAL(triggered()), this, SLOT(OnSaveMaterialList()));
	QObject::connect(this->ui.actionSave_list_as, SIGNAL(triggered()), this, SLOT(OnSaveMaterialListAs()));

	QObject::connect(this->ui.newMaterial, SIGNAL(pressed()), this, SLOT(OnNewMaterial()));
	QObject::connect(this->ui.deleteMaterial, SIGNAL(pressed()), this, SLOT(OnDeleteMaterial()));
	QObject::connect(this->ui.materialBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnMaterialSelected(int)));

	QObject::connect(this->ui.materialName, SIGNAL(editingFinished()), this, SLOT(OnMaterialNameChanged()));
	QObject::connect(this->ui.isVirtual, SIGNAL(stateChanged(int)), this, SLOT(OnMaterialVirtualSet()));
	QObject::connect(this->ui.descEditor, SIGNAL(pressed()), this, SLOT(OnMaterialEditDescription()));
	QObject::connect(this->ui.inherit, SIGNAL(editingFinished()), this, SLOT(OnMaterialInheritChanged()));

	QObject::connect(this->ui.addPass, SIGNAL(pressed()), this, SLOT(OnAddPass()));
	QObject::connect(this->ui.removePass, SIGNAL(pressed()), this, SLOT(OnRemovePass()));
	QObject::connect(this->ui.addVariable, SIGNAL(pressed()), this, SLOT(OnAddVariable()));
	QObject::connect(this->ui.removeVariable, SIGNAL(pressed()), this, SLOT(OnRemoveVariable()));

	QObject::connect(this->ui.variableWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnVariableRightClicked(const QPoint&)));
	this->ui.variableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	this->passDelegate.window = this;
	this->variableDelegate.window = this;
	this->ui.passWidget->setItemDelegate(&this->passDelegate);
	this->ui.variableWidget->setItemDelegate(&this->variableDelegate);

	// open with a fresh list
	this->OnNewMaterialList();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::SetupFromShader(const IO::URI& path)
{
	IoServer* ioServer = IoServer::Instance();
	if (ioServer->FileExists(path))
	{
		// material exists, just open it and modify
		this->Load(path);
	}
	else
	{
		// create material
	}
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::Load(const IO::URI& path)
{
	this->ui.materialBox->blockSignals(true);
	this->ui.variableWidget->clearContents();
	this->ui.passWidget->clearContents();
	this->ui.passWidget->setRowCount(0);
	this->ui.variableWidget->setRowCount(0);	
	this->ui.materialBox->clear();
	this->ui.inherit->clear();
	this->ui.materialBox->blockSignals(false);
	this->materials.Clear();

	Ptr<XmlReader> reader = XmlReader::Create();
	Ptr<Stream> stream = IoServer::Instance()->CreateStream(path);
	reader->SetStream(stream);
	if (reader->Open())
	{
		if (!reader->HasNode("/Nebula3/Materials"))
		{
			n_error("MaterialLoader: '%s' is not a valid material palette!", path.AsString().AsCharPtr());
		}

		reader->SetToNode("/Nebula3/Materials");
		if (reader->SetToFirstChild("Material")) do
		{
			ParseMaterial(reader);
		}
		while (reader->SetToNextChild("Material"));
	}
	else
	{
		SHADY_ERROR_FORMAT("Cannot open %s for writing", path.LocalPath().AsCharPtr());
	}
	this->currentListfilePath = path.LocalPath();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::ParseMaterial(const Ptr<XmlReader>& reader)
{
	MaterialState state;
	state.name = reader->GetString("name");
	state.desc = reader->GetOptString("desc", "");
	state.inherit = reader->GetOptString("inherits", "");
	state.isVirtual = reader->GetOptBool("virtual", false);
	if (!state.isVirtual) state.type = reader->GetString("type");
	if (reader->SetToFirstChild("Pass")) do
	{
		ParsePass(reader, state);
	}
	while (reader->SetToNextChild("Pass"));

	if (reader->SetToFirstChild("Param")) do
	{
		ParseParameter(reader, state);
	}
	while (reader->SetToNextChild("Param"));

	// add to UI
	this->materials.Append(state);
	this->ui.materialBox->addItem(state.name.AsString().AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::ParsePass(const Ptr<XmlReader>& reader, MaterialState& state)
{
	MaterialPassState pass;
	pass.batch = reader->GetString("batch");
	pass.shader = reader->GetString("shader");
	pass.variation = reader->GetString("variation");
	state.passes.Append(pass);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::ParseParameter(const Ptr<XmlReader>& reader, MaterialState& state)
{
	MaterialVariableState var;
	var.name = reader->GetString("name");
	var.desc = reader->GetOptString("desc", "");
	var.type = Util::Variant::StringToType(reader->GetString("type"));
	switch (var.type)
	{
	case Util::Variant::Float:
		var.defaultVal.SetFloat(reader->GetOptFloat("defaultValue", 0.0f));
		var.minVal.SetFloat(reader->GetOptFloat("min", 0.0f));
		var.maxVal.SetFloat(reader->GetOptFloat("max", 1.0f));
		break;
	case Util::Variant::Int:
		var.defaultVal.SetInt(reader->GetOptInt("defaultValue", 0));
		var.minVal.SetInt(reader->GetOptInt("min", 0));
		var.maxVal.SetInt(reader->GetOptInt("max", 1));
		break;
	case Util::Variant::Bool:
		var.defaultVal.SetBool(reader->GetOptBool("defaultValue", false));
		var.minVal.SetBool(false);
		var.maxVal.SetBool(true);
		break;
	case Util::Variant::Float4:
		var.defaultVal.SetFloat4(reader->GetOptFloat4("defaultValue", Math::float4(0, 0, 0, 0)));
		var.minVal.SetFloat4(reader->GetOptFloat4("min", Math::float4(0, 0, 0, 0)));
		var.maxVal.SetFloat4(reader->GetOptFloat4("max", Math::float4(1, 1, 1, 1)));
		break;
	case Util::Variant::Float2:
		var.defaultVal.SetFloat2(reader->GetOptFloat2("defaultValue", Math::float2(0, 0)));
		var.minVal.SetFloat2(reader->GetOptFloat2("min", Math::float2(0, 0)));
		var.maxVal.SetFloat2(reader->GetOptFloat2("max", Math::float2(1, 1)));
		break;
	case Util::Variant::Matrix44:
		var.defaultVal.SetMatrix44(reader->GetOptMatrix44("defaultValue", Math::matrix44::identity()));
		break;
	case Util::Variant::String:
		var.defaultVal.SetString(reader->GetOptString("defaultValue", "tex:system/placeholder.dds"));
		break;
	}
	var.system = reader->GetOptBool("system", false);
	var.editType = reader->GetOptString("editType", "raw");
	state.variables.Append(var);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::Save(const IO::URI& path)
{
	Ptr<XmlWriter> writer = XmlWriter::Create();
	Ptr<Stream> stream = IoServer::Instance()->CreateStream(path);
	writer->SetStream(stream);
	if (writer->Open())
	{
		// write header
		writer->BeginNode("Nebula3");

		// write dependency on base list
		writer->BeginNode("Dependency");
		writer->SetString("list", "base.xml");
		writer->EndNode();

		writer->BeginNode("Materials");

		IndexT i;
		for (i = 0; i < this->materials.Size(); i++)
		{
			const MaterialState& state = this->materials[i];

			// write material body
			writer->BeginNode("Material");
			writer->SetString("name", state.name.AsString());
			writer->SetString("desc", state.desc);
			writer->SetString("type", state.type);
			writer->SetString("inherits", state.inherit);
			writer->SetBool("virtual", state.isVirtual);

			// write passes
			IndexT j;
			for (j = 0; j < state.passes.Size(); j++)
			{
				const MaterialPassState& pass = state.passes[j];
				writer->BeginNode("Pass");
				writer->SetString("batch", pass.batch);
				writer->SetString("shader", pass.shader);
				writer->SetString("variation", pass.variation);
				writer->EndNode(); // end Pass node
			}

			// write variables
			for (j = 0; j < state.variables.Size(); j++)
			{
				const MaterialVariableState& var = state.variables[j];
				writer->BeginNode("Param");
				writer->SetString("name", var.name);
				writer->SetString("desc", var.desc);
				writer->SetString("type", Util::Variant::TypeToString(var.type));
				writer->SetString("defaultVal", var.defaultVal.ToString());
				writer->SetString("minVal", var.minVal.ToString());
				writer->SetString("maxVal", var.maxVal.ToString());
				writer->SetString("editType", var.editType);
				writer->SetBool("system", var.system);
				writer->EndNode(); // end Param node
			}

			writer->EndNode(); // end Material node
		}

		writer->EndNode();	// end Materials
		writer->EndNode();	// end Nebula3

		// close writer
		writer->Close();
	}
	else
	{
		SHADY_ERROR_FORMAT("Cannot open %s for writing", path.LocalPath().AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnSaveMaterialList()
{
	if (this->currentListfilePath.IsEmpty()) this->OnSaveMaterialListAs();
	else
	{
		this->Save(this->currentListfilePath);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnSaveMaterialListAs()
{
	IO::URI mats("proj:work/materials");
	IoServer::Instance()->CreateDirectory(mats);
	QString file = QFileDialog::getSaveFileName(NULL, "Save list file", mats.LocalPath().AsCharPtr(), "XML (*.xml)");
	if (!file.isEmpty())
	{
		this->currentListfilePath = file.toUtf8().constData();
		this->setWindowTitle(Util::String::Sprintf("Material editor - %s", this->currentListfilePath.ExtractFileName().AsCharPtr()).AsCharPtr());
		this->Save(this->currentListfilePath);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnOpenMaterialList()
{
	IO::URI mats("proj:work/materials");
	if (IoServer::Instance()->DirectoryExists(mats))
	{
		QString file = QFileDialog::getOpenFileName(NULL, "Open material list", mats.LocalPath().AsCharPtr(), "XML (*.xml)");
		if (!file.isEmpty())
		{
			this->Load(file.toUtf8().constData());
			this->setWindowTitle(Util::String::Sprintf("Material editor - %s", this->currentListfilePath.ExtractFileName().AsCharPtr()).AsCharPtr());
		}
	}
	else
	{
		int res = QMessageBox::question(NULL, "No material lists found", "No material lists available for this project, do you want to create one?", QMessageBox::Yes | QMessageBox::No);
		if (res == QMessageBox::Yes)
		{
			this->OnNewMaterialList();
		}
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnNewMaterialList()
{
	this->materials.Clear();
	this->currentListfilePath.Clear();
	this->ui.materialName->setText("");
	this->ui.passWidget->clearContents();
	this->ui.variableWidget->clearContents();
	this->ui.passWidget->setRowCount(0);
	this->ui.variableWidget->setRowCount(0);
	this->ui.materialBox->clear();
	this->ui.addPass->setEnabled(false);
	this->ui.removePass->setEnabled(false);
	this->ui.addVariable->setEnabled(false);
	this->ui.removeVariable->setEnabled(false);
	
	this->setWindowTitle("Material editor");
}

//------------------------------------------------------------------------------
/**
	This function handles selecting materials.
	Also, because it listens to the box index change signal, it will also handle the list becoming empty, and the reverse.
	
*/
void
MaterialWindow::OnMaterialSelected(int index)
{
	this->ui.passWidget->clearContents();
	this->ui.variableWidget->clearContents();
	this->ui.passWidget->setRowCount(0);
	this->ui.variableWidget->setRowCount(0);
	if (!this->materials.IsEmpty())
	{
		const MaterialState& state = this->materials[index];
		this->ui.materialName->setText(state.name.Value());
		this->ui.isVirtual->setChecked(state.isVirtual);
		this->ui.inherit->setText(state.inherit.AsCharPtr());

		IndexT i;
		for (i = 0; i < state.passes.Size(); i++)
		{
			this->SetupPass(&state.passes[i]);
		}
		for (i = 0; i < state.variables.Size(); i++)
		{
			this->SetupVariable(&state.variables[i]);
		}

		this->ui.addPass->setEnabled(true);
		this->ui.removePass->setEnabled(true);
		this->ui.addVariable->setEnabled(true);
		this->ui.removeVariable->setEnabled(true);
		this->ui.deleteMaterial->setEnabled(true);
	}
	else
	{
		this->ui.addPass->setEnabled(false);
		this->ui.removePass->setEnabled(false);
		this->ui.addVariable->setEnabled(false);
		this->ui.removeVariable->setEnabled(false);
		this->ui.deleteMaterial->setEnabled(false);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnNewMaterial()
{
	MaterialState newState;
	newState.name = "New material";
	newState.isVirtual = false;
	newState.type = "static";
	newState.inherit = "";
	this->materials.Append(newState);
	this->ui.materialBox->addItem(newState.name.Value());
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnDeleteMaterial()
{
	int selected = this->ui.materialBox->currentIndex();
	this->materials.EraseIndex(selected);
	this->ui.materialBox->removeItem(selected);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnMaterialNameChanged()
{
	int selected = this->ui.materialBox->currentIndex();
	this->ui.materialBox->setItemText(selected, this->ui.materialName->text());
	MaterialState& state = this->materials[selected];
	state.name = this->ui.materialName->text().toUtf8().constData();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnMaterialVirtualSet()
{
	int selected = this->ui.materialBox->currentIndex();
	MaterialState& state = this->materials[selected];
	state.isVirtual = this->ui.isVirtual->isChecked();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnMaterialEditDescription()
{
	int selected = this->ui.materialBox->currentIndex();
	MaterialState& state = this->materials[selected];
	QString desc = QInputDialog::getText(NULL, "Write description", "Description", QLineEdit::Normal, state.desc.AsCharPtr());
	if (!desc.isEmpty()) state.desc = desc.toUtf8().constData();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnMaterialInheritChanged()
{
	int selected = this->ui.materialBox->currentIndex();
	MaterialState& state = this->materials[selected];
	state.inherit = this->ui.inherit->text().toUtf8().constData();

	// lookup base material palette
	const Ptr<Materials::MaterialPalette>& palette = Materials::MaterialServer::Instance()->LookupMaterialPalette("base.xml");

	// check if inheritance is properly formatted
	Util::Array<Util::String> materialsToInherit = state.inherit.Tokenize("|");
	Util::String correctedInheritance;
	IndexT i;
	for (i = 0; i < materialsToInherit.Size(); i++)
	{
		Util::String mat = materialsToInherit[i];
		mat.Trim(" ");
		if (!palette->HasMaterial(mat))
		{
			SHADY_WARNING_FORMAT("Material %s doesn't exist in base.xml", mat.AsCharPtr());
			materialsToInherit.EraseIndex(i--);
			continue;
		}
		if (i > 0) correctedInheritance.Append("|");
		correctedInheritance.Append(mat);
		
	}

	this->ui.inherit->setText(correctedInheritance.AsCharPtr());
	state.inherit = correctedInheritance;	
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnAddPass()
{
	MaterialPassState pass;
	pass.batch = "FlatGeometryLit";
	pass.shader = ShadyProject::Instance()->GetPath().LocalPath().ExtractFileName();
	pass.shader.StripFileExtension();	// will contain .ndp
	pass.variation = "Generated";

	MaterialState& state = this->materials[this->ui.materialBox->currentIndex()];
	state.passes.Append(pass);

	// setup UI
	this->SetupPass(&state.passes.Back());
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnRemovePass()
{
	MaterialState& state = this->materials[this->ui.materialBox->currentIndex()];
	QList<QTableWidgetSelectionRange> ranges = this->ui.passWidget->selectedRanges();
	IndexT i;
	for (i = 0; i < ranges.length(); i++)
	{
		const QTableWidgetSelectionRange& range = ranges[i];
		int begin = range.topRow();
		IndexT j = begin;
		for (; j < begin + range.rowCount(); j++)
		{
			state.passes.EraseIndex(j);
			this->ui.passWidget->removeRow(j);
		}
	}
	
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnAddVariable()
{
	MaterialVariableState var;
	var.name = "NewVariable";
	var.type = Util::Variant::Float;
	var.minVal = 1.0f;
	var.maxVal = 1.0f;
	var.defaultVal = 1.0f;
	var.desc = "";
	var.system = false;
	MaterialState& state = this->materials[this->ui.materialBox->currentIndex()];
	state.variables.Append(var);
	
	// setup UI
	this->SetupVariable(&state.variables.Back());
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnRemoveVariable()
{
	MaterialState& state = this->materials[this->ui.materialBox->currentIndex()];
	QList<QTableWidgetSelectionRange> ranges = this->ui.variableWidget->selectedRanges();
	IndexT i;
	for (i = 0; i < ranges.length(); i++)
	{
		const QTableWidgetSelectionRange& range = ranges[i];
		int begin = range.topRow();
		IndexT j = begin;
		for (; j < begin + range.rowCount(); j++)
		{
			state.variables.EraseIndex(j);
			this->ui.variableWidget->removeRow(j);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::SetupVariable(const MaterialVariableState* state)
{
	// setup UI
	int newRow = this->ui.variableWidget->rowCount();
	this->ui.variableWidget->insertRow(newRow);
	this->ui.variableWidget->setItem(newRow, 0, new QTableWidgetItem(state->name.AsCharPtr()));
	this->ui.variableWidget->setItem(newRow, 1, new QTableWidgetItem(Util::Variant::TypeToString(state->type).AsCharPtr()));
	this->ui.variableWidget->setItem(newRow, 2, new QTableWidgetItem(state->defaultVal.ToString().AsCharPtr()));
	this->ui.variableWidget->setItem(newRow, 3, new QTableWidgetItem(state->minVal.ToString().AsCharPtr()));
	this->ui.variableWidget->setItem(newRow, 4, new QTableWidgetItem(state->maxVal.ToString().AsCharPtr()));
	this->ui.variableWidget->setItem(newRow, 5, new QTableWidgetItem(state->desc.AsCharPtr()));

	if (state->type == Util::Variant::Bool || state->type == Util::Variant::String)
	{
		this->ui.variableWidget->item(newRow, 3)->setFlags(0);
		this->ui.variableWidget->item(newRow, 4)->setFlags(0);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::SetupPass(const MaterialPassState* state)
{
	// setup UI
	int newRow = this->ui.passWidget->rowCount();
	this->ui.passWidget->insertRow(newRow);
	this->ui.passWidget->setItem(newRow, 0, new QTableWidgetItem(state->batch.AsCharPtr()));
	this->ui.passWidget->setItem(newRow, 1, new QTableWidgetItem(state->shader.AsCharPtr()));
	this->ui.passWidget->setItem(newRow, 2, new QTableWidgetItem(state->variation.AsCharPtr()));
}

//------------------------------------------------------------------------------
/**
*/
QDialog*
MaterialWindow::CreateItemFrame(QWidget* widget)
{
	QDialog* dialog = new QDialog;
	dialog->setWindowFlags(Qt::FramelessWindowHint);

	QVBoxLayout layout;
	dialog->setLayout(&layout);
	layout.setContentsMargins(QMargins(0, 0, 0, 0));
	layout.addWidget(widget);
	
	return dialog;
}

//------------------------------------------------------------------------------
/**
*/
MaterialWindow::MaterialState&
MaterialWindow::GetCurrentState() const
{
	return this->materials[this->ui.materialBox->currentIndex()];
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::SetVariableItemEnabled(const QModelIndex& index, bool enable)
{
	QTableWidgetItem* item = this->ui.variableWidget->item(index.row(), index.column());
	if (enable) item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
	else		item->setFlags(Qt::ItemIsSelectable);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::OnVariableRightClicked(const QPoint& point)
{
	QMenu menu;
	QAction* copy = menu.addAction("Set to parameter");
	QAction* selected = menu.exec(this->ui.variableWidget->mapToGlobal(point));
	if (selected == copy)
	{
		QMenu menu;
		Util::String path = Util::String::Sprintf("shd:%s", ShadyProject::Instance()->GetPath().LocalPath().ExtractFileName().AsCharPtr());
		const Ptr<CoreGraphics::Shader>& shader = CoreGraphics::ShaderServer::Instance()->GetShader(path);

		IndexT i;
		for (i = 0; i < shader->GetNumVariables(); i++)
		{
			const Ptr<CoreGraphics::ShaderVariable>& var = shader->GetVariableByIndex(i);
			menu.addAction(var->GetName().Value());
		}

		// execute the new menu
		QAction* selected = menu.exec(this->ui.variableWidget->mapToGlobal(point));
		if (selected != NULL)
		{
			QMenu* menu = selected->menu();
			Nody::Node* node = (Nody::Node*)menu->property("node").value<void*>();
			Nody::Variable* var = (Nody::Variable*)menu->property("variable").value<void*>();

			// handle hidden variable
			if (var->GetIOFlag() == ShadyVariable::Hidden)
			{

			}
		}
	}
}

} // namespace Shady