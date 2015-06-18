//------------------------------------------------------------------------------
//  materialhandler.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "materialhandler.h"
#include "resources/resourcemanager.h"
#include "ui_materialhelpwidget.h"
#include "code/assetbrowser.h"
#include "io/uri.h"
#include "io/ioserver.h"
#include "materials/materialdatabase.h"
#include "contentbrowserapp.h"
#include "previewer/previewstate.h"
#include "graphics/modelentity.h"
#include "renderutil/nodelookuputil.h"
#include "materials/streamsurfacematerialsaver.h"
#include "binaryxmlconverter.h"
#include "logger.h"
#include "mutablesurfacematerial.h"
#include "messaging/staticmessagehandler.h"

#include <QDialog>
#include <QColorDialog>
#include <QGroupBox>
#include <QBitmap>
#include <QMessageBox>
#include <QInputDialog>

using namespace ToolkitUtil;
using namespace Graphics;
using namespace ContentBrowser;
using namespace Math;
using namespace Materials;
using namespace Util;
namespace Widgets
{
__ImplementClass(Widgets::MaterialHandler, 'MAHA', BaseHandler);

//------------------------------------------------------------------------------
/**
*/
MaterialHandler::MaterialHandler() :
	hasChanges(false)
{
	this->saveDialogUi.setupUi(&this->saveDialog);

	// connect button slot
	connect(this->saveDialogUi.newCategory, SIGNAL(pressed()), this, SLOT(OnNewCategory()));
	this->SetupSaveDialog();
}

//------------------------------------------------------------------------------
/**
*/
MaterialHandler::~MaterialHandler()
{
	disconnect(this->ui->newButton, SIGNAL(clicked()));
	disconnect(this->ui->saveButton, SIGNAL(clicked()));
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::Setup(const QString& resource)
{
    BaseHandler::Setup();

    // clear reference maps
    this->textureImgMap.clear();
    this->textureTextMap.clear();
    this->textureLabelMap.clear();
    this->variableLabelMap.clear();
    this->variableSliderMap.clear();
    this->variableFloatValueMap.clear();
    this->variableIntValueMap.clear();
    this->variableBoolMap.clear();
    this->variableVectorFieldMap.clear();
	this->variableVectorColorEditMap.clear();
    this->variableVectorMap.clear();
    this->lowerLimitFloatMap.clear();
    this->upperLimitFloatMap.clear();
    this->lowerLimitIntMap.clear();
    this->upperLimitIntMap.clear();

    // enable the elements which are only viable if we are working on a surface
    this->ui->templateBox->setEnabled(true);
    this->ui->saveButton->setEnabled(true);
    this->ui->saveAsButton->setEnabled(true);

	// get components of resource
	String res = resource.toUtf8().constData();
	res.StripAssignPrefix();
	res.StripFileExtension();
	this->category = res.ExtractDirName();
	this->category.SubstituteString("/", "");
	this->file = res.ExtractFileName();
	this->ui->surfaceName->setText(String::Sprintf("%s/%s", this->category.AsCharPtr(), this->file.AsCharPtr()).AsCharPtr());

    // create resource
	this->managedMaterial = Resources::ResourceManager::Instance()->CreateManagedResource(SurfaceMaterial::RTTI, resource.toUtf8().constData(), NULL, true).downcast<Materials::ManagedSurfaceMaterial>();
	this->material = this->managedMaterial->GetMaterial();

    // get layout
	this->mainLayout = static_cast<QVBoxLayout*>(this->ui->variableFrame->layout());

    // setup UI
    this->MakeMaterialUI(this->ui->surfaceName, this->ui->templateBox, this->ui->materialHelp);
}

//------------------------------------------------------------------------------
/**
*/
bool
MaterialHandler::Discard()
{
	// avoid discarding changes if the user doesn't want to
	if (this->hasChanges)
	{
		QMessageBox::StandardButton button = QMessageBox::warning(NULL, "Pending changes", "Your material has unsaved changes, are you sure you want to close it?", QMessageBox::Ok | QMessageBox::No);
		if (button == QMessageBox::Cancel)
		{
			return false;
		}
	}

	// clear everything, including the UI
	IndexT i;
	for (i = 0; i < this->textureResources.Size(); i++)
	{
		Resources::ResourceManager::Instance()->DiscardManagedResource(this->textureResources.ValueAtIndex(i).upcast<Resources::ManagedResource>());
	}
	this->textureResources.Clear();
    this->textureVariables.Clear();
    this->scalarVariables.Clear();
	Resources::ResourceManager::Instance()->DiscardManagedResource(this->managedMaterial.upcast<Resources::ManagedResource>());
	this->managedMaterial = 0;
	this->material = 0;
	this->ClearFrame(this->mainLayout);

	return BaseHandler::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::MaterialSelected(const QString& material)
{
	// avoid discarding changes if the user doesn't want to
	if (this->hasChanges)
	{
		QMessageBox::StandardButton button = QMessageBox::warning(NULL, "Pending changes", "Switching material templates will effectively discard all changes, are you sure you want to do this?", QMessageBox::Ok | QMessageBox::No);
		if (button == QMessageBox::Cancel)
		{
			return;
		}
	}

	// discard textures managed by this handler
	IndexT i;
	for (i = 0; i < this->textureResources.Size(); i++)
	{
		Resources::ResourceManager::Instance()->DiscardManagedResource(this->textureResources.ValueAtIndex(i).upcast<Resources::ManagedResource>());
	}
	this->textureResources.Clear();
	this->textureVariables.Clear();
	this->scalarVariables.Clear();

	// update material
	Ptr<Material> mat = MaterialServer::Instance()->GetMaterialByName(material.toUtf8().constData());
	Ptr<MutableSurfaceMaterial> mutableMaterial = this->material.downcast<MutableSurfaceMaterial>();
	mutableMaterial->SetMaterialTemplate(mat);

	// rebuild the UI
	this->ResetUI();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::MaterialInfo()
{
	// get material
	Ptr<Material> mat = this->material->GetMaterialTemplate();

	// create ui
	Ui::MaterialHelpWidget ui;
	QDialog dialog;
	ui.setupUi(&dialog);

	// set info
	ui.materialName->setText(mat->GetName().AsString().AsCharPtr());
	Util::String desc = mat->GetDescription();
	ui.materialDesc->setText(desc.AsCharPtr());

	// show widget
	dialog.exec();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::TextureChanged(uint i)
{
    // cast as line edit
    QLineEdit* item = this->textureTextMap.key(i);
    item->setEnabled(true);

    // get label
    QLabel* label = this->textureLabelMap.key(i);

    // get image button
    QPushButton* img = this->textureImgMap.key(i);

    // convert to nebula texture
    String valueText = item->text().toUtf8().constData();
    String idText = label->text().toUtf8().constData();

    // setup texture
    this->SetupTextureSlotHelper(item, img, valueText, this->defaultTextureMap[i].toUtf8().constData());

    // allocate texture
    Ptr<Resources::ManagedTexture> textureObject = Resources::ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Texture::RTTI, valueText, NULL, true).downcast<Resources::ManagedTexture>();
    if (this->textureResources.Contains(i))
    {
        Resources::ResourceManager::Instance()->DiscardManagedResource(this->textureResources[i].upcast<Resources::ManagedResource>());
        this->textureResources.EraseAtIndex(i);
    }
    this->textureResources.Add(i, textureObject);
    if (this->textureVariables[i].isvalid()) this->textureVariables[i]->SetTexture(textureObject->GetTexture());
	this->hasChanges = true;
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::NewSurface()
{
	// avoid discarding changes if the user doesn't want to
	if (this->hasChanges)
	{
		QMessageBox::StandardButton button = QMessageBox::warning(NULL, "Pending changes", "Your material has unsaved changes, are you sure you want to close it?", QMessageBox::Ok | QMessageBox::Cancel);
		if (button == QMessageBox::Cancel)
		{
			return;
		}
	}
	
	// reset variables
	this->textureResources.Clear();
	this->textureVariables.Clear();
	this->scalarVariables.Clear();

	// enable UI controls
	this->ui->templateBox->setEnabled(true);
	this->ui->saveButton->setEnabled(true);
	this->ui->saveAsButton->setEnabled(true);

	// don't discard textures here, because we just need to reset this handler
	this->textureResources.Clear();

	// clear names
	this->category.Clear();
	this->file.Clear();

	// reset name of label
	this->ui->surfaceName->setText("<unnamed>");

	// basically load placeholder, duplicate it, then clear the UI
	if (this->managedMaterial.isvalid()) Resources::ResourceManager::Instance()->DiscardManagedResource(this->managedMaterial.upcast<Resources::ManagedResource>());
	this->managedMaterial = Resources::ResourceManager::Instance()->CreateManagedResource(SurfaceMaterial::RTTI, "intsur:system/placeholder.sur", NULL, true).downcast<Materials::ManagedSurfaceMaterial>();
	this->material = this->managedMaterial->GetMaterial();
	this->hasChanges = false;
	this->ResetUI();	
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::TextureTextChanged()
{
    // get sender
    QObject* sender = this->sender();

    // cast to line edit
    QLineEdit* lineEdit = static_cast<QLineEdit*>(sender);

    // skip the rest if nothing has happened
    if (!lineEdit->isModified()) return;

    // get image button
    QPushButton* img = this->textureImgMap.key(this->textureTextMap[lineEdit]);

    // get index and invoke actual function
    uint index = this->textureTextMap[lineEdit];

    // update texture
    this->TextureChanged(index);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::VariableIntSliderChanged()
{
    // get sender
    QObject* sender = this->sender();

    // cast to line edit
    QSlider* slider = static_cast<QSlider*>(sender);

    // get index
    uint index = this->variableSliderMap[slider];

    // set value
    if (this->scalarVariables[index].isvalid()) this->scalarVariables[index]->SetValue(slider->value());

    // get spin box
    QSpinBox* box = this->variableIntValueMap.key(index);

    // freeze signal from slider, set value, and unfreeze
    box->blockSignals(true);
    box->setValue(slider->value());
    box->blockSignals(false);

    // update UI immediately
    QApplication::processEvents();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::VariableIntSliderDone()
{
    // get sender
    QObject* sender = this->sender();

    // cast to line edit
    QSlider* slider = static_cast<QSlider*>(sender);

    // get index
    uint index = this->variableSliderMap[slider];

    // update int
    this->IntVariableChanged(index);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::VariableFloatSliderChanged()
{
    // get sender
    QObject* sender = this->sender();

    // cast to line edit
    QSlider* slider = static_cast<QSlider*>(sender);

    // get index
    uint index = this->variableSliderMap[slider];

    // set value
    if (this->scalarVariables[index].isvalid()) this->scalarVariables[index]->SetValue(slider->value() / 100.0f);

    // get spin box
    QDoubleSpinBox* box = this->variableFloatValueMap.key(index);

    // freeze signal from slider, set value, and unfreeze
    box->blockSignals(true);
    box->setValue((float)slider->value() / 100.0f);
    box->blockSignals(false);

    // update UI immediately
    QApplication::processEvents();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::VariableFloatSliderDone()
{
    // get sender
    QObject* sender = this->sender();

    // cast to line edit
    QSlider* slider = static_cast<QSlider*>(sender);

    // get index
    uint index = this->variableSliderMap[slider];

    // convert slider value to float and 
    this->FloatVariableChanged(index);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::VariableIntFieldChanged()
{
    // get sender
    QObject* sender = this->sender();

    // cast to box
    QSpinBox* box = static_cast<QSpinBox*>(sender);

    // get index
    uint index = this->variableIntValueMap[box];

    // get slider and set value
    QSlider* slider = this->variableSliderMap.key(index);
    if (slider)
    {
        slider->blockSignals(true);
        slider->setValue(box->value());
        slider->blockSignals(false);
    }

    // update UI immediately
    QApplication::processEvents();

    // update int
    this->IntVariableChanged(index);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::VariableFloatFieldChanged()
{
    // get sender
    QObject* sender = this->sender();

    // cast to line edit
    QDoubleSpinBox* box = static_cast<QDoubleSpinBox*>(sender);

    // get index
    uint index = this->variableFloatValueMap[box];

    // get slider and set value
    QSlider* slider = this->variableSliderMap.key(index);
    if (slider)
    {
        slider->blockSignals(true);
        slider->setValue(box->value() * 100);
        slider->blockSignals(false);
    }

    // update UI
    QApplication::processEvents();

    // update float
    this->FloatVariableChanged(index);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::VariableFloat2FieldChanged()
{
    // get sender
    QObject* sender = this->sender();

    // cast to line edit
    QDoubleSpinBox* box = static_cast<QDoubleSpinBox*>(sender);

    // get index
    uint index = this->variableVectorFieldMap[box];
    this->Float2VariableChanged(index);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::VariableFloat4FieldChanged()
{
    // get sender
    QObject* sender = this->sender();

    // cast to line edit
    QDoubleSpinBox* box = static_cast<QDoubleSpinBox*>(sender);

    // get index
    uint index = this->variableVectorFieldMap[box];
    this->Float4VariableChanged(index);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::VariableCheckBoxChanged()
{
    // get sender
    QObject* sender = this->sender();

    // cast to line edit
    QCheckBox* box = static_cast<QCheckBox*>(sender);

    // get index
    uint index = this->variableBoolMap[box];
    this->BoolVariableChanged(index);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::VariableFloatLimitsChanged()
{
    // get sender
    QObject* sender = this->sender();

    // cast to line edit
    QDoubleSpinBox* box = static_cast<QDoubleSpinBox*>(sender);

    // get index
    if (this->lowerLimitFloatMap.contains(box))
    {
        uint index = this->lowerLimitFloatMap[box];
        this->FloatLimitChanged(index);
    }
    else
    {
        uint index = this->upperLimitFloatMap[box];
        this->FloatLimitChanged(index);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::VariableIntLimitsChanged()
{
    // get sender
    QObject* sender = this->sender();

    // cast to line edit
    QSpinBox* box = static_cast<QSpinBox*>(sender);

    // get index
    if (this->lowerLimitIntMap.contains(box))
    {
        uint index = this->lowerLimitIntMap[box];
        this->IntLimitChanged(index);
    }
    else
    {
        uint index = this->upperLimitIntMap[box];
        this->IntLimitChanged(index);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::Browse()
{
    // get sender
    QObject* sender = this->sender();

    // must be a button
    QPushButton* button = static_cast<QPushButton*>(sender);

    // get line edit
    QLineEdit* text = this->textureTextMap.key(this->textureImgMap[button]);
    QLabel* name = this->textureLabelMap.key(this->textureImgMap[button]);

    // pick a texture
    int res = ResourceBrowser::AssetBrowser::Instance()->Execute("Assign to: " + name->text(), ResourceBrowser::AssetBrowser::Textures);
    if (res == QDialog::Accepted)
    {
        // convert to nebula string
        String texture = ResourceBrowser::AssetBrowser::Instance()->GetSelectedTexture().toUtf8().constData();

		// set text of item
        text->setText(texture.AsCharPtr());

        // invoke texture change function
        this->TextureChanged(this->textureImgMap[button]);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::ChangeColor()
{
    // get sender
    QObject* sender = this->sender();

    // must be a button
    QPushButton* button = static_cast<QPushButton*>(sender);

    // get index 
    int i = this->variableVectorColorEditMap[button];

    // get spin boxes for value
    QList<QDoubleSpinBox*> vector = this->variableVectorMap[i];

    // convert to qcolor
    float4 values(vector[0]->value(), vector[1]->value(), vector[2]->value(), vector[3]->value());
    values = float4::clamp(values, float4(0), float4(1));
    QColor qcolor(values.x() * 255, values.y() * 255, values.z() * 255, values.w() * 255);

    // open color dialog
    QColorDialog dialog(qcolor);

    int val = dialog.exec();
    if (val == QDialog::Accepted)
    {
        QColor diaColor = dialog.currentColor();
        float4 color;
        color.x() = diaColor.red() / 255.0f;
        color.y() = diaColor.green() / 255.0f;
        color.z() = diaColor.blue() / 255.0f;
        color.w() = diaColor.alpha() / 255.0f;

        vector[0]->blockSignals(true);
        vector[1]->blockSignals(true);
        vector[2]->blockSignals(true);
        vector[3]->blockSignals(true);

        vector[0]->setValue(color.x());
        vector[1]->setValue(color.y());
        vector[2]->setValue(color.z());
        vector[3]->setValue(color.w());

        vector[0]->blockSignals(false);
        vector[1]->blockSignals(false);
        vector[2]->blockSignals(false);
        vector[3]->blockSignals(false);

        // update button
        diaColor.setAlpha(255);
        button->setPalette(QPalette(diaColor));

        this->Float4VariableChanged(i);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::Save()
{
	if (this->category.IsEmpty() || this->file.IsEmpty())
	{
		if (this->saveDialog.exec() == QDialog::Accepted)
		{
			this->category = this->saveDialogUi.categoryBox->currentText().toUtf8().constData();
			this->file = this->saveDialogUi.nameEdit->text().toUtf8().constData();
		}		
		else
		{
			// just abort
			return;
		}
	}
		
	Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();
	String resName = String::Sprintf("src:assets/%s/%s.sur", this->category.AsCharPtr(), this->file.AsCharPtr());
	Ptr<IO::Stream> stream = ioServer->CreateStream(resName);

	// save material
	Ptr<StreamSurfaceMaterialSaver> saver = StreamSurfaceMaterialSaver::Create();
	saver->SetStream(stream);
	this->material->SetSaver(saver.upcast<Resources::ResourceSaver>());
	if (!this->material->Save())
	{
		QMessageBox::critical(NULL, "Could not save surface!", "Surface could not be saved");
	}
	this->material->SetSaver(0);
	QString label;
	this->ui->surfaceName->setText(label.sprintf("%s/%s", this->category.AsCharPtr(), this->file.AsCharPtr()));

	// format the target where the resource will be exported to
	String exportTarget = String::Sprintf("sur:%s/%s.sur", this->category.AsCharPtr(), this->file.AsCharPtr());

	// create directory
	ioServer->CreateDirectory(exportTarget.ExtractToLastSlash());

	// also convert it
	Logger logger;
	BinaryXmlConverter converter;
	converter.ConvertFile(resName, exportTarget, logger);
	this->hasChanges = false;

	// hmm, now our managed material here will need to be updated, since we made a new material
	Resources::ResourceManager::Instance()->DiscardManagedResource(this->managedMaterial.upcast<Resources::ManagedResource>());
	this->managedMaterial = Resources::ResourceManager::Instance()->CreateManagedResource(SurfaceMaterial::RTTI, exportTarget, NULL, true).downcast<Materials::ManagedSurfaceMaterial>();
	this->material = this->managedMaterial->GetMaterial();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::SaveAs()
{
	// update UI
	this->saveDialogUi.categoryBox->setCurrentIndex(this->saveDialogUi.categoryBox->findText(this->category.AsCharPtr()));
	this->saveDialogUi.nameEdit->setText(this->file.AsCharPtr());

	// open dialog
	if (this->saveDialog.exec() == QDialog::Accepted)
	{
		this->category = this->saveDialogUi.categoryBox->currentText().toUtf8().constData();
		this->file = this->saveDialogUi.nameEdit->text().toUtf8().constData();

		Ptr<IO::IoServer> ioServer = IO::IoServer::Instance();
		String resName = String::Sprintf("src:assets/%s/%s.sur", this->category.AsCharPtr(), this->file.AsCharPtr());
		Ptr<IO::Stream> stream = ioServer->CreateStream(resName);

		// create saver and save material
		Ptr<StreamSurfaceMaterialSaver> saver = StreamSurfaceMaterialSaver::Create();
		saver->SetStream(stream);
		this->material->SetSaver(saver.upcast<Resources::ResourceSaver>());
		if (!this->material->Save())
		{
			QMessageBox::critical(NULL, "Could not save surface!", "Surface could not be saved");
		}
		this->material->SetSaver(0);

		// reformat label
		QString label;
		this->ui->surfaceName->setText(label.sprintf("%s/%s", this->category.AsCharPtr(), this->file.AsCharPtr()));

		// format the target where the resource will be exported to
		String exportTarget = String::Sprintf("sur:%s/%s.sur", this->category.AsCharPtr(), this->file.AsCharPtr());

		// create directory
		ioServer->CreateDirectory(exportTarget.ExtractToLastSlash());

		// also convert it
		Logger logger;
		BinaryXmlConverter converter;
		converter.ConvertFile(resName, exportTarget, logger);
		this->hasChanges = false;

		// hmm, now our managed material here will need to be updated, since we made a new material
		Resources::ResourceManager::Instance()->DiscardManagedResource(this->managedMaterial.upcast<Resources::ManagedResource>());
		this->managedMaterial = Resources::ResourceManager::Instance()->CreateManagedResource(SurfaceMaterial::RTTI, exportTarget, NULL, true).downcast<Materials::ManagedSurfaceMaterial>();
		this->material = this->managedMaterial->GetMaterial();

		// make sure to reload the surface in case we have just overwritten it
		Ptr<ReloadResourceIfExists> msg = ReloadResourceIfExists::Create();
		msg->SetResourceName(exportTarget);
		__StaticSend(GraphicsInterface, msg);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::OnNewCategory()
{
	QString category = QInputDialog::getText(NULL, "Create new category", "Category", QLineEdit::Normal);

	// just quit if we don't have any text
	if (category.isEmpty())
	{
		return;
	}

	String dir;
	dir.Format("src:assets/%s", category.toUtf8().constData());
	if (!IO::IoServer::Instance()->DirectoryExists(dir))
	{
		IO::IoServer::Instance()->CreateDirectory(dir);
	}

	// reinitialize ui
	this->SetupSaveDialog();

	// set current category
	this->saveDialogUi.categoryBox->setCurrentIndex(this->saveDialogUi.categoryBox->findText(category));
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::FloatVariableChanged(uint i)
{
    // get spin box
    QDoubleSpinBox* box = this->variableFloatValueMap.key(i);
    QDoubleSpinBox* lower = this->lowerLimitFloatMap.key(i);
    QDoubleSpinBox* upper = this->upperLimitFloatMap.key(i);

    // also get label
    QLabel* label = this->variableLabelMap.key(i);

    // set value
    if (this->scalarVariables[i].isvalid()) this->scalarVariables[i]->SetValue((float)box->value());

    // format label as text
    String idText = label->text().toUtf8().constData();
	this->hasChanges = true;
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::Float2VariableChanged(uint i)
{
    // get spin box
    QList<QDoubleSpinBox*> vector = this->variableVectorMap[i];

    // create float from vector
    float2 floatVec(vector[0]->value(), vector[1]->value());

    // set value
    if (this->scalarVariables[i].isvalid()) this->scalarVariables[i]->SetValue(floatVec);

    // also get label
    QLabel* label = this->variableLabelMap.key(i);

    // format label as text
    String idText = label->text().toUtf8().constData();
	this->hasChanges = true;
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::Float4VariableChanged(uint i)
{
    // get spin box
    QList<QDoubleSpinBox*> vector = this->variableVectorMap[i];

    // create float from vector
    float4 floatVec(vector[0]->value(), vector[1]->value(), vector[2]->value(), vector[3]->value());

    // update color field if it exists
    QPushButton* key = this->variableVectorColorEditMap.key(i);
    if (0 != key)
    {
        float4 clamped = float4::clamp(floatVec, float4(0), float4(1));
        key->setPalette(QPalette(QColor(clamped.x() * 255, clamped.y() * 255, clamped.z() * 255)));
    }

    // set value
    if (this->scalarVariables[i].isvalid()) this->scalarVariables[i]->SetValue(floatVec);

    // also get label
    QLabel* label = this->variableLabelMap.key(i);

    // format label as text
    String idText = label->text().toUtf8().constData();
	this->hasChanges = true;
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::BoolVariableChanged(uint i)
{
    // get spin box
    QCheckBox* box = this->variableBoolMap.key(i);

    // set value
    if (this->scalarVariables[i].isvalid()) this->scalarVariables[i]->SetValue(box->isChecked());

    // also get label
    QLabel* label = this->variableLabelMap.key(i);

    // format label as text
    String idText = label->text().toUtf8().constData();
	this->hasChanges = true;
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::IntVariableChanged(uint i)
{
    // get spin box
    QSpinBox* box = this->variableIntValueMap.key(i);
    QSpinBox* lower = this->lowerLimitIntMap.key(i);
    QSpinBox* upper = this->upperLimitIntMap.key(i);

    // set value
    if (this->scalarVariables[i].isvalid()) this->scalarVariables[i]->SetValue(box->value());

    // also get label
    QLabel* label = this->variableLabelMap.key(i);

    // format label as text
    String idText = label->text().toUtf8().constData();
	this->hasChanges = true;
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::FloatLimitChanged(uint i)
{
    // get spin box
    QDoubleSpinBox* lower = this->lowerLimitFloatMap.key(i);
    QDoubleSpinBox* upper = this->upperLimitFloatMap.key(i);
    QDoubleSpinBox* value = this->variableFloatValueMap.key(i);
    QSlider* valueSlider = this->variableSliderMap.key(i);

    // set value limits, do not trigger any events
    value->blockSignals(true);
    value->setMaximum(upper->value());
    value->setMinimum(lower->value());
    value->blockSignals(false);

    // set slider limits, do not trigger any events
    valueSlider->blockSignals(true);
    valueSlider->setMaximum(upper->value() * 100);
    valueSlider->setMinimum(lower->value() * 100);
    valueSlider->blockSignals(false);

    // manually call variable change
    this->FloatVariableChanged(i);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::IntLimitChanged(uint i)
{
    // get spin box
    QSpinBox* lower = this->lowerLimitIntMap.key(i);
    QSpinBox* upper = this->upperLimitIntMap.key(i);
    QSpinBox* value = this->variableIntValueMap.key(i);
    QSlider* valueSlider = this->variableSliderMap.key(i);

    // set value limits, do not trigger any events
    value->blockSignals(true);
    value->setMaximum(upper->value());
    value->setMinimum(lower->value());
    value->blockSignals(false);

    // set slider limits, do not trigger any events
    valueSlider->blockSignals(true);
    valueSlider->setMaximum(upper->value());
    valueSlider->setMinimum(lower->value());
    valueSlider->blockSignals(false);

    // manually call variable change
    this->IntVariableChanged(i);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::SetupTextureSlotHelper(QLineEdit* textureField, QPushButton* textureButton, Util::String& resource, const Util::String& defaultResource)
{
    n_assert(0 != textureField);
    n_assert(0 != textureButton);

    // create pixmap which will be used to set the icon of the browsing button
    QPixmap pixmap;

    // if the resource name is not empty, load texture
    if (!resource.IsEmpty())
    {
        resource.ChangeAssignPrefix("tex");
        resource.ChangeFileExtension("dds");
        if (IO::IoServer::Instance()->FileExists(resource))
        {
            QPalette pal;
            textureField->setPalette(pal);

            if (resource.IsValid())
            {
                IO::URI texFile = resource;
                pixmap.load(texFile.LocalPath().AsCharPtr());
                int width = n_min(pixmap.width(), 512);
                int height = n_min(pixmap.height(), 512);
                pixmap = pixmap.scaled(QSize(24, 24), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                textureButton->setToolTip("<html><img height=" + QString::number(height) + " width=" + QString::number(width) + " src=\"" + QString(texFile.LocalPath().AsCharPtr()) + "\"/></html>");

                textureField->blockSignals(true);
                textureField->setText(resource.AsCharPtr());
                textureField->blockSignals(false);
            }
        }
        else
        {
            textureField->blockSignals(true);
            textureField->setText(resource.AsCharPtr());
            textureField->blockSignals(false);

            IO::URI texFile = defaultResource;
            pixmap.load(texFile.LocalPath().AsCharPtr());
            int width = n_min(pixmap.width(), 512);
            int height = n_min(pixmap.height(), 512);
            pixmap = pixmap.scaled(QSize(24, 24), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            textureButton->setToolTip("<html><img height=" + QString::number(height) + " width=" + QString::number(width) + " src=\"" + QString(texFile.LocalPath().AsCharPtr()) + "\"/></html>");

            QPalette pal;
            pal.setColor(QPalette::Text, Qt::red);
            textureField->setPalette(pal);
        }

    }
    else
    {
        // set resource to the default one
        resource = defaultResource;
        IO::URI texFile = resource;
        pixmap.load(texFile.LocalPath().AsCharPtr());
        int width = n_min(pixmap.width(), 512);
        int height = n_min(pixmap.height(), 512);
        pixmap = pixmap.scaled(QSize(24, 24), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        textureButton->setToolTip("<html><img height=" + QString::number(height) + " width=" + QString::number(width) + " src=\"" + QString(texFile.LocalPath().AsCharPtr()) + "\"/></html>");

        textureField->blockSignals(true);
        textureField->setText(resource.AsCharPtr());
        textureField->blockSignals(false);
    }

    QPalette palette;
    palette.setBrush(textureButton->backgroundRole(), QBrush(pixmap));
    textureButton->setPalette(palette);
    textureButton->setFixedSize(QSize(24, 24));
    textureButton->setMask(pixmap.mask());
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::MakeMaterialUI(QLabel* surfaceName, QComboBox* materialBox, QPushButton* materialHelp)
{
    // we need to do this, because we might use different layouts
    this->materialBox = materialBox;
    this->materialHelp = materialHelp;

    // clear material list
    materialBox->clear();

    // connect changes to material box to be passed onto this
    connect(materialBox, SIGNAL(activated(const QString&)), this, SLOT(MaterialSelected(const QString&)));
    connect(materialHelp, SIGNAL(clicked()), this, SLOT(MaterialInfo()));

    // format combo box
    const Array<String> materials = ContentBrowser::MaterialDatabase::Instance()->GetMaterialList();
    IndexT i;
    for (i = 0; i < materials.Size(); i++)
    {
        const Ptr<Materials::Material>& mat = ContentBrowser::MaterialDatabase::Instance()->GetMaterial(materials[i]);
        materialBox->addItem(mat->GetName().AsString().AsCharPtr());
    }

    // now find our material and set index
    int index = materialBox->findText(this->material->GetMaterialTemplate()->GetName().AsString().AsCharPtr());
    materialBox->setCurrentIndex(index);

    // get state node
    Ptr<PreviewState> previewState = ContentBrowserApp::Instance()->GetPreviewState();
    const Ptr<ModelEntity>& model = previewState->GetModel();

    Ptr<FetchSkinList> fetchSkinsMessage = FetchSkinList::Create();
    __Send(model, fetchSkinsMessage);
    Array<StringAtom> skins = fetchSkinsMessage->GetSkins();
    for (i = 0; i < skins.Size(); i++)
    {
        Ptr<Graphics::ShowSkin> showSkin = Graphics::ShowSkin::Create();
        showSkin->SetSkin(skins[i]);
        __Send(model, showSkin);
    }

    // get material
    Ptr<Material> mat = this->material->GetMaterialTemplate();

    // add textures
    Array<Material::MaterialParameter> textures = this->GetTextures(mat);
    for (i = 0; i < textures.Size(); i++)
    {
        // copy parameter
        Material::MaterialParameter param = textures[i];

        // create texture variables
        Ptr<SurfaceConstant> constant;
        if (this->material->HasConstant(param.name)) constant = this->material->GetConstant(param.name);
		else										 continue;
        this->textureVariables.Add(i, constant);

		// create new horizontal layout 
		QHBoxLayout* varLayout = new QHBoxLayout;

        // get texture
        Ptr<CoreGraphics::Texture> textureObject = (CoreGraphics::Texture*)constant->GetValue().GetObject();
        this->defaultTextureMap[i] = textureObject->GetResourceId().AsString().AsCharPtr();
        String res = textureObject->GetResourceId().AsString();

        // get texture info
        String name = param.name;

        // create items
        QLabel* texName = new QLabel(name.AsCharPtr());
        QFont font = texName->font();
        font.setBold(true);
        texName->setFont(font);
        QPushButton* texImg = new QPushButton();
        QLineEdit* texRes = new QLineEdit();
        this->SetupTextureSlotHelper(texRes, texImg, res, textureObject->GetResourceId().AsString());

        // add both elements to dictionaries
        this->textureTextMap[texRes] = i;
        this->textureImgMap[texImg] = i;
        this->textureLabelMap[texName] = i;

        // connect slots
        connect(texImg, SIGNAL(released()), this, SLOT(Browse()));
        connect(texRes, SIGNAL(editingFinished()), this, SLOT(TextureTextChanged()));

        // add stuff to layout
        varLayout->addWidget(texName);

        // space coming items
        varLayout->addStretch(100);
        texRes->setFixedWidth(150);

        varLayout->addWidget(texRes);
        varLayout->addWidget(texImg);

        // add layout to base layout
        this->mainLayout->addLayout(varLayout);
    }

    // add variables
    Array<Material::MaterialParameter> vars = this->GetVariables(mat);
    for (i = 0; i < vars.Size(); i++)
    {
        // get parameter
        Material::MaterialParameter param = vars[i];
        this->defaultValueMap[i] = param.defaultVal;

        Variant min = param.min;
        Variant max = param.max;

        // get texture info
        String name = param.name;
        Variant var = param.defaultVal;

		// create material instance
		Ptr<SurfaceConstant> constant;
		if (this->material->HasConstant(param.name)) constant = this->material->GetConstant(param.name);
		else										 continue;

        // setup label
        QLabel* varName = new QLabel(name.AsCharPtr());
        QFont font = varName->font();
        font.setBold(true);
        font.setPointSize(10);
        varName->setFont(font);

        // setup group
        QGroupBox* group = new QGroupBox;
        QVBoxLayout* groupLayout = new QVBoxLayout;
        groupLayout->setContentsMargins(QMargins(2, 5, 2, 5));
        group->setLayout(groupLayout);
        group->setFlat(true);

        // create material instance
        this->scalarVariables.Add(i, constant);

        if (var.GetType() == Variant::Float4)
        {
            // create two layouts, one for the variable and one for the label
            QHBoxLayout* labelLayout = new QHBoxLayout;
            labelLayout->setAlignment(Qt::AlignCenter);
            QHBoxLayout* varLayout = new QHBoxLayout;

            // add label to layout
            labelLayout->addWidget(varName);

            // set name in map
            this->variableLabelMap[varName] = i;

            // create ui
            QDoubleSpinBox* box1 = new QDoubleSpinBox;
            QDoubleSpinBox* box2 = new QDoubleSpinBox;
            QDoubleSpinBox* box3 = new QDoubleSpinBox;
            QDoubleSpinBox* box4 = new QDoubleSpinBox;

            // get float
            float4 val = var.GetFloat4();

            // get min-max
            float4 minVal = param.min.GetFloat4();
            float4 maxVal = param.max.GetFloat4();

            box1->setRange(minVal.x(), maxVal.x());
            box1->setValue(val.x());
            box1->setSingleStep(0.01f);
            box1->setDecimals(2);

            box2->setRange(minVal.y(), maxVal.y());
            box2->setValue(val.y());
            box2->setSingleStep(0.01f);
            box2->setDecimals(2);

            box3->setRange(minVal.z(), maxVal.z());
            box3->setValue(val.z());
            box3->setDecimals(2);
            box3->setSingleStep(0.01f);

            box4->setRange(minVal.w(), maxVal.w());
            box4->setValue(val.w());
            box4->setSingleStep(0.01f);
            box4->setDecimals(2);

            // add label
            this->variableLabelMap[varName] = i;

            // add boxes by index
            this->variableVectorFieldMap[box1] = i;
            this->variableVectorFieldMap[box2] = i;
            this->variableVectorFieldMap[box3] = i;
            this->variableVectorFieldMap[box4] = i;

            // add to registry of boxes
            this->variableVectorMap[i].append(box1);
            this->variableVectorMap[i].append(box2);
            this->variableVectorMap[i].append(box3);
            this->variableVectorMap[i].append(box4);

            // connect boxes to slot
            connect(box1, SIGNAL(valueChanged(double)), this, SLOT(VariableFloat4FieldChanged()));
            connect(box2, SIGNAL(valueChanged(double)), this, SLOT(VariableFloat4FieldChanged()));
            connect(box3, SIGNAL(valueChanged(double)), this, SLOT(VariableFloat4FieldChanged()));
            connect(box4, SIGNAL(valueChanged(double)), this, SLOT(VariableFloat4FieldChanged()));

            // add boxes to layout
            varLayout->addWidget(box1);
            varLayout->addWidget(box2);
            varLayout->addWidget(box3);
            varLayout->addWidget(box4);

            // add to layout
            if (!param.desc.IsEmpty()) group->setToolTip(param.desc.AsCharPtr());
            groupLayout->addLayout(labelLayout);
            groupLayout->addLayout(varLayout);
			this->mainLayout->addWidget(group);

            // handle if we have a color edit
            if (param.editType == Material::MaterialParameter::EditColor)
            {
                // create button with color
                QPushButton* colorChooserButton = new QPushButton;
                colorChooserButton->setText("Change...");
                val = float4::clamp(val, float4(0), float4(1));
                QPalette palette(QColor(val.x() * 255, val.y() * 255, val.z() * 255));
                colorChooserButton->setPalette(palette);

                groupLayout->addWidget(colorChooserButton);
                connect(colorChooserButton, SIGNAL(clicked()), this, SLOT(ChangeColor()));

                this->variableVectorColorEditMap[colorChooserButton] = i;
            }
        }
        else if (var.GetType() == Variant::Float2)
        {
            // create two layouts, one for the variable and one for the label
            QHBoxLayout* labelLayout = new QHBoxLayout;
            labelLayout->setAlignment(Qt::AlignCenter);
            QHBoxLayout* varLayout = new QHBoxLayout;

            // add label to layout
            labelLayout->addWidget(varName);

            // set name in map
            this->variableLabelMap[varName] = i;

            // create ui
            QDoubleSpinBox* box1 = new QDoubleSpinBox;
            QDoubleSpinBox* box2 = new QDoubleSpinBox;

            // get float
            float2 val = var.GetFloat2();

            // get min-max
            float2 minVal = param.min.GetFloat2();
            float2 maxVal = param.max.GetFloat2();

            box1->setRange(minVal.x(), maxVal.x());
            box1->setValue(val.x());
            box1->setSingleStep(0.01f);
            box1->setDecimals(2);

            box2->setRange(minVal.y(), maxVal.y());
            box2->setValue(val.y());
            box2->setSingleStep(0.01f);
            box2->setDecimals(2);

            // add label
            this->variableLabelMap[varName] = i;

            // add boxes by index
            this->variableVectorFieldMap[box1] = i;
            this->variableVectorFieldMap[box2] = i;

            // add to registry of boxes
            this->variableVectorMap[i].append(box1);
            this->variableVectorMap[i].append(box2);

            // connect boxes to slot
            connect(box1, SIGNAL(valueChanged(double)), this, SLOT(VariableFloat2FieldChanged()));
            connect(box2, SIGNAL(valueChanged(double)), this, SLOT(VariableFloat2FieldChanged()));

            // add boxes to layout
            varLayout->addWidget(box1);
            varLayout->addWidget(box2);

            // add to layout
            if (!param.desc.IsEmpty()) group->setToolTip(param.desc.AsCharPtr());
            groupLayout->addLayout(labelLayout);
            groupLayout->addLayout(varLayout);
			this->mainLayout->addWidget(group);
        }
        else if (var.GetType() == Variant::Float)
        {
            // create two layouts, one for the variable and one for the label
            QHBoxLayout* labelLayout = new QHBoxLayout;
            labelLayout->setAlignment(Qt::AlignCenter);
            QHBoxLayout* varLayout = new QHBoxLayout;

            // add label to layout
            labelLayout->addWidget(varName);

            // create limits
            QDoubleSpinBox* lowerLimit = new QDoubleSpinBox;
            lowerLimit->setRange(-10000, 10000);
            lowerLimit->setButtonSymbols(QAbstractSpinBox::NoButtons);
            lowerLimit->setValue(min.GetFloat());

            QDoubleSpinBox* upperLimit = new QDoubleSpinBox;
            upperLimit->setRange(-10000, 10000);
            upperLimit->setButtonSymbols(QAbstractSpinBox::NoButtons);
            upperLimit->setValue(max.GetFloat());

            varLayout->addWidget(lowerLimit);
            connect(lowerLimit, SIGNAL(valueChanged(double)), this, SLOT(VariableFloatLimitsChanged()));

            // depending on what type of resource we have, we need different handlers
            QSlider* slider = new QSlider(Qt::Horizontal);
            slider->setRange(min.GetFloat() * 100, max.GetFloat() * 100);
            slider->setValue(var.GetFloat() * 100);
            slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

            connect(slider, SIGNAL(valueChanged(int)), this, SLOT(VariableFloatSliderChanged()));
            connect(slider, SIGNAL(sliderReleased()), this, SLOT(VariableFloatSliderDone()));
            varLayout->addWidget(slider);

            // add to registry
            this->variableSliderMap[slider] = i;

            varLayout->addWidget(upperLimit);
            connect(upperLimit, SIGNAL(valueChanged(double)), this, SLOT(VariableFloatLimitsChanged()));

            // create value representation
            QDoubleSpinBox* box = new QDoubleSpinBox;
            box->setRange(min.GetFloat(), max.GetFloat());
            box->setValue(var.GetFloat());
            box->setSingleStep(0.01f);
            box->setFixedWidth(75);

            // connect box to actual value
            connect(box, SIGNAL(valueChanged(double)), this, SLOT(VariableFloatFieldChanged()));

            varLayout->addWidget(box);

            // add UI elements to lists
            this->variableLabelMap[varName] = i;
            this->variableFloatValueMap[box] = i;
            this->lowerLimitFloatMap[lowerLimit] = i;
            this->upperLimitFloatMap[upperLimit] = i;

            // add to layout
            if (!param.desc.IsEmpty()) group->setToolTip(param.desc.AsCharPtr());
            groupLayout->addLayout(labelLayout);
            groupLayout->addLayout(varLayout);
			this->mainLayout->addWidget(group);
        }
        else if (var.GetType() == Variant::Int)
        {
            // create new horizontal layout 
            QHBoxLayout* labelLayout = new QHBoxLayout;
            labelLayout->setAlignment(Qt::AlignCenter);
            QHBoxLayout* varLayout = new QHBoxLayout;

            // add label to layout
            labelLayout->addWidget(varName);

            // create limits
            QSpinBox* lowerLimit = new QSpinBox;
            lowerLimit->setRange(-10000, 10000);
            lowerLimit->setButtonSymbols(QAbstractSpinBox::NoButtons);
            lowerLimit->setValue(min.GetInt());

            QSpinBox* upperLimit = new QSpinBox;
            upperLimit->setRange(-10000, 10000);
            upperLimit->setButtonSymbols(QAbstractSpinBox::NoButtons);
            upperLimit->setValue(max.GetInt());

            varLayout->addWidget(lowerLimit);
            connect(lowerLimit, SIGNAL(valueChanged(int)), this, SLOT(VariableIntLimitsChanged()));

            // depending on what type of resource we have, we need different handlers
            QSlider* slider = new QSlider(Qt::Horizontal);
            slider->setRange(min.GetInt(), max.GetInt());
            slider->setValue(var.GetInt());
            slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

            connect(slider, SIGNAL(valueChanged(int)), this, SLOT(VariableIntSliderChanged()));
            connect(slider, SIGNAL(sliderReleased()), this, SLOT(VariableIntSliderDone()));
            varLayout->addWidget(slider);

            this->variableSliderMap[slider] = i;

            varLayout->addWidget(upperLimit);
            connect(upperLimit, SIGNAL(valueChanged(int)), this, SLOT(VariableIntLimitsChanged()));

            // create value representation
            QSpinBox* box = new QSpinBox;
            box->setRange(min.GetInt(), max.GetInt());
            box->setValue(var.GetInt());
            box->setFixedWidth(75);

            // connect box to actual value
            connect(box, SIGNAL(valueChanged(int)), this, SLOT(VariableIntFieldChanged()));

            varLayout->addWidget(box);

            // add UI elements to lists
            this->variableLabelMap[varName] = i;
            this->variableIntValueMap[box] = i;
            this->lowerLimitIntMap[lowerLimit] = i;
            this->upperLimitIntMap[upperLimit] = i;

            // add to group
            if (!param.desc.IsEmpty()) group->setToolTip(param.desc.AsCharPtr());
            groupLayout->addLayout(labelLayout);
            groupLayout->addLayout(varLayout);
			this->mainLayout->addWidget(group);
        }
        else if (var.GetType() == Variant::Bool)
        {
            // create new horizontal layout 
            QHBoxLayout* labelLayout = new QHBoxLayout;
            labelLayout->setAlignment(Qt::AlignCenter);
            QHBoxLayout* varLayout = new QHBoxLayout;
            varLayout->setAlignment(Qt::AlignRight);

            // create check box
            QCheckBox* box = new QCheckBox;
            box->setChecked(var.GetBool());

            // connect check box to change
            connect(box, SIGNAL(toggled(bool)), this, SLOT(VariableCheckBoxChanged()));

            // add label
            this->variableLabelMap[varName] = i;

            // add check box
            this->variableBoolMap[box] = i;

            // add label and box to layout
            labelLayout->addWidget(varName);
            varLayout->addWidget(box);

            // add to group
            if (!param.desc.IsEmpty()) group->setToolTip(param.desc.AsCharPtr());
            groupLayout->addLayout(labelLayout);
            groupLayout->addLayout(varLayout);
			this->mainLayout->addWidget(group);
        }
    }

    // add spacer
	this->mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
}


//------------------------------------------------------------------------------
/**
*/
Util::Array<Materials::Material::MaterialParameter>
MaterialHandler::GetTextures(const Ptr<Materials::Material>& mat)
{
    // create return value
    Util::Array<Material::MaterialParameter> retval;

    // get parameters
    const Util::Dictionary<StringAtom, Material::MaterialParameter>& params = mat->GetParameters();

    // add textures
    IndexT i;
    for (i = 0; i < params.Size(); i++)
    {
        const Variant& defaultVar = params.ValueAtIndex(i).defaultVal;
        bool show = !params.ValueAtIndex(i).system;
        if (defaultVar.GetType() == Variant::String && show)
        {
            retval.Append(params.ValueAtIndex(i));
        }
    }

    // return parameters which are textures
    return retval;
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Materials::Material::MaterialParameter>
MaterialHandler::GetVariables(const Ptr<Materials::Material>& mat)
{
    // get parameters
    const Util::Dictionary<StringAtom, Material::MaterialParameter>& params = mat->GetParameters();

    // create retval
    Util::Array<Material::MaterialParameter> retval;

    // add textures
    IndexT i;
    for (i = 0; i < params.Size(); i++)
    {
        const Variant& defaultVar = params.ValueAtIndex(i).defaultVal;
        bool show = !params.ValueAtIndex(i).system;
        if (defaultVar.GetType() != Variant::Object && show)
        {
            retval.Append(params.ValueAtIndex(i));
        }
    }

    // return parameter list
    return retval;
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::ClearFrame(QLayout* layout)
{
	disconnect(this->ui->templateBox, SIGNAL(activated(const QString&)), this, SLOT(MaterialSelected(const QString&)));
	disconnect(this->ui->materialHelp, SIGNAL(clicked()), this, SLOT(MaterialInfo()));

    if (layout)
    {
        QLayoutItem* item;
        QLayout* subLayout;
        QWidget* widget;
        while (item = layout->takeAt(0))
        {
            if (subLayout = item->layout()) this->ClearFrame(subLayout);
            else if (widget = item->widget()) { widget->hide(); delete widget; }
            else delete item;
        }
        if (layout != this->mainLayout->layout())
        {
            delete layout;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::SetupSaveDialog()
{
	// clear category box
	this->saveDialogUi.categoryBox->clear();

	// format string for resource folder
	String folder = "src:assets/";

	// find all categories
	Array<String> directories = IO::IoServer::Instance()->ListDirectories(folder, "*");

	// go through directories and add to category box
	IndexT i;
	for (i = 0; i < directories.Size(); i++)
	{
		this->saveDialogUi.categoryBox->addItem(directories[i].AsCharPtr());
	}

	// set active index
	this->saveDialogUi.categoryBox->setCurrentIndex(0);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialHandler::ResetUI()
{
	// clear reference maps
	this->textureImgMap.clear();
	this->textureTextMap.clear();
	this->textureLabelMap.clear();
	this->variableLabelMap.clear();
	this->variableSliderMap.clear();
	this->variableFloatValueMap.clear();
	this->variableIntValueMap.clear();
	this->variableBoolMap.clear();
	this->variableVectorFieldMap.clear();
	this->variableVectorColorEditMap.clear();
	this->variableVectorMap.clear();
	this->lowerLimitFloatMap.clear();
	this->upperLimitFloatMap.clear();
	this->lowerLimitIntMap.clear();
	this->upperLimitIntMap.clear();

	// get layout
	this->mainLayout = static_cast<QVBoxLayout*>(this->ui->variableFrame->layout());

	this->ClearFrame(this->mainLayout);
	this->MakeMaterialUI(this->ui->surfaceName, this->ui->templateBox, this->ui->materialHelp);
}

} // namespace Widgets