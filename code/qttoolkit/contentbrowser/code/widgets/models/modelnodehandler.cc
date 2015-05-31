//------------------------------------------------------------------------------
//  modelnodeitemhandler.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include <QFileDialog>
#include <QLineEdit>
#include <QToolButton>
#include <QGroupBox>
#include <QBitmap>
#include <QColorDialog>
#include "modelnodehandler.h"
#include "modelnodeframe.h"
#include "materials/materialdatabase.h"
#include "io/ioserver.h"
#include "math/float4.h"
#include "io/uri.h"
#include "resources/managedtexture.h"
#include "ui_materialinfowidget.h"
#include "contentbrowserapp.h"
#include "graphics/modelentity.h"
#include "renderutil/nodelookuputil.h"
#include "resources/resourcemanager.h"


using namespace Materials;
using namespace ToolkitUtil;
using namespace Util;
using namespace ContentBrowser;
using namespace IO;
using namespace Math;
using namespace Graphics;

namespace Widgets
{
__ImplementClass(Widgets::ModelNodeHandler, 'MNIH', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ModelNodeHandler::ModelNodeHandler() :
	mainLayout(0),
	itemHandler(0),
	actionUpdateMode(true)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ModelNodeHandler::~ModelNodeHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeHandler::Setup( const Util::String& material, const Util::String& resource )
{
	n_assert(resource.IsValid());

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
	this->variableVectorMap.clear();
    this->lowerLimitFloatMap.clear();
    this->upperLimitFloatMap.clear();
    this->lowerLimitIntMap.clear();
    this->upperLimitIntMap.clear();

	// copy resource
	this->material = material;
	this->resource =  resource;	

	// get layout
	QVBoxLayout* mainLayout = static_cast<QVBoxLayout*>(this->ui->variableFrame->layout());

	// setup UI
	this->MakeMaterialUI(mainLayout, this->ui->nodeName, this->ui->materialBox, this->ui->materialHelp, material);
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::Discard()
{
	IndexT i;
	for (i = 0; i < this->textureResources.Size(); i++)
	{
		Resources::ResourceManager::Instance()->DiscardManagedResource(this->textureResources.ValueAtIndex(i).upcast<Resources::ManagedResource>());
	}
	this->textureResources.Clear();
	this->UnsetState();
}

//------------------------------------------------------------------------------
/**
	Reset entire GUI and run setup again
*/
void
ModelNodeHandler::HardRefresh(const Util::String& material, const Util::String& resource)
{
	// get layout
	QLayout* layout = this->mainLayout;

	// disconnect callback from material box to avoid multiple connections if we connect again
	disconnect(this->materialBox, SIGNAL(activated(const QString&)), this, SLOT(MaterialSelected(const QString&)));
	disconnect(this->materialHelp, SIGNAL(clicked()), this, SLOT(MaterialInfo()));

	// clear layout
	this->ClearFrame(layout);

	// setup again
	this->Setup(material, resource);
}

//------------------------------------------------------------------------------
/**
	This function walks through all textures and variables, and updates the variables.
	So this function will not remake the UI, and will assume the UI is already set up, as well as the state node and material variables.
*/
void
ModelNodeHandler::SoftRefresh(const Util::String& material, const Util::String& resource)
{
	// get material
	Ptr<Material> mat = MaterialDatabase::Instance()->GetMaterial(material);

	// get attribute
	Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();

	// now get state for node
	const State& state = attributes->GetState(this->nodePath);

	// get textures from material
	Array<Material::MaterialParameter> textures = this->GetTextures(mat);

	IndexT i;
	for (i = 0; i < textures.Size(); i++)
	{
		// copy parameter
		Material::MaterialParameter param = textures[i];

		QLineEdit* texEdit = this->textureTextMap.key(i);
		QPushButton* texButton = this->textureImgMap.key(i);

		// get default value
		QString textureObject = this->defaultTextureMap[i];

		// get texture
		Texture tex;
		tex.textureName = param.name;

		IndexT texIndex = state.textures.FindIndex(tex);
		if (texIndex != InvalidIndex)
		{
			param.defaultVal = state.textures[texIndex].textureResource;
		}
		else
		{
			param.defaultVal = textureObject.toUtf8().constData();
		}

		// update texture slot
		String res = param.defaultVal.GetString();
		this->SetupTextureSlotHelper(texEdit, texButton, res, textureObject.toUtf8().constData());

		// create texture
		Ptr<Resources::ManagedTexture> varTex = Resources::ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Texture::RTTI, res).downcast<Resources::ManagedTexture>();
		this->textureResources.EraseAtIndex(i);
		this->textureResources.Add(i, varTex);
		if (this->textureVariables[i].isvalid()) this->textureVariables[i]->SetTexture(varTex->GetTexture());
	}

	// get variables from material
	Array<Material::MaterialParameter> variables = this->GetVariables(mat);

	for (i = 0; i < variables.Size(); i++)
	{
		// copy parameter
		Material::MaterialParameter param = variables[i];

		// get texture
		Variable variable;
		variable.variableName = param.name;

		Variant min = param.min;
		Variant max = param.max;

		IndexT varIndex = state.variables.FindIndex(variable);
		if (varIndex != InvalidIndex)
		{
			// reset default value if types differ
			if (state.variables[varIndex].variableValue.GetType() == param.defaultVal.GetType())
			{
				param.defaultVal = state.variables[varIndex].variableValue;
				min = state.variables[varIndex].limits.Key();
				max = state.variables[varIndex].limits.Value();
			}
		}

		// get texture info
		String name = param.name;
		Variant var = param.defaultVal;

		// set variable
		if (this->scalarVariables[i].isvalid()) this->scalarVariables[i]->SetValue(var);

		if (var.GetType() == Variant::Float)
		{
			// get float
			float val = var.GetFloat();

			// get min-max
			float minVal = param.min.GetFloat();
			float maxVal = param.max.GetFloat();

			QDoubleSpinBox* floatBox = this->variableFloatValueMap.key(i);
			QDoubleSpinBox* floatMinBox = this->lowerLimitFloatMap.key(i);
			QDoubleSpinBox* floatMaxBox = this->upperLimitFloatMap.key(i);
			QSlider* floatSlider = this->variableSliderMap.key(i);

			// update values
			floatBox->blockSignals(true);
			floatMinBox->blockSignals(true);
			floatMaxBox->blockSignals(true);
			floatSlider->blockSignals(true);

			floatBox->setValue(val);
			floatMinBox->setValue(minVal);
			floatMaxBox->setValue(maxVal);
			floatSlider->setValue(val * 100);
			floatSlider->setRange(minVal * 100, maxVal * 100);

			floatBox->blockSignals(false);
			floatMinBox->blockSignals(false);
			floatMaxBox->blockSignals(false);
			floatSlider->blockSignals(false);
		}
		else if (var.GetType() == Variant::Int)
		{
			// get float
			int val = var.GetInt();

			// get min-max
			int minVal = param.min.GetInt();
			int maxVal = param.max.GetInt();

			QSpinBox* intBox = this->variableIntValueMap.key(i);
			QSpinBox* intMinBox = this->lowerLimitIntMap.key(i);
			QSpinBox* intMaxBox = this->upperLimitIntMap.key(i);
			QSlider* intSlider = this->variableSliderMap.key(i);

			intBox->blockSignals(true);
			intMinBox->blockSignals(true);
			intMaxBox->blockSignals(true);
			intSlider->blockSignals(true);

			intBox->setValue(val);
			intMinBox->setValue(minVal);
			intMaxBox->setValue(maxVal);
			intSlider->setValue(val);
			intSlider->setRange(minVal, maxVal);

			intBox->blockSignals(false);
			intMinBox->blockSignals(false);
			intMaxBox->blockSignals(false);
			intSlider->blockSignals(false);
		}
		else if (var.GetType() == Variant::Bool)
		{
			// get float
			bool val = var.GetBool();

			QCheckBox* boolBox = this->variableBoolMap.key(i);
			boolBox->blockSignals(true);
			boolBox->setChecked(val);
			boolBox->blockSignals(false);
		}
		else if (var.GetType() == Variant::Float2)
		{
			// get float
			float2 val = var.GetFloat2();

			// get min-max
			float2 minVal = param.min.GetFloat2();
			float2 maxVal = param.max.GetFloat2();

			QDoubleSpinBox* xBox = this->variableVectorMap[i][0];
			QDoubleSpinBox* yBox = this->variableVectorMap[i][1];

			xBox->blockSignals(true);
			yBox->blockSignals(true);

			xBox->setRange(minVal.x(), maxVal.x());
			xBox->setValue(val.x());
			yBox->setRange(minVal.y(), maxVal.y());
			yBox->setValue(val.y());

			xBox->blockSignals(false);
			yBox->blockSignals(false);
		}
		else if (var.GetType() == Variant::Float4)
		{
			// get float
			float4 val = var.GetFloat4();

			// get min-max
			float4 minVal = param.min.GetFloat4();
			float4 maxVal = param.max.GetFloat4();

			QDoubleSpinBox* xBox = this->variableVectorMap[i][0];
			QDoubleSpinBox* yBox = this->variableVectorMap[i][1];
			QDoubleSpinBox* zBox = this->variableVectorMap[i][2];
			QDoubleSpinBox* wBox = this->variableVectorMap[i][3];

			if (param.editType == Material::MaterialParameter::EditColor)
			{
				QPushButton* button = this->variableVectorColorEditMap.key(i);
				val = float4::clamp(val, float4(0), float4(1));
				button->setPalette(QPalette(QColor(val.x() * 255, val.y() * 255, val.z() * 255)));
			}

			xBox->blockSignals(true);
			yBox->blockSignals(true);
			zBox->blockSignals(true);
			wBox->blockSignals(true);

			xBox->setRange(minVal.x(), maxVal.x());
			xBox->setValue(val.x());
			yBox->setRange(minVal.y(), maxVal.y());
			yBox->setValue(val.y());
			zBox->setRange(minVal.z(), maxVal.z());
			zBox->setValue(val.z());
			wBox->setRange(minVal.w(), maxVal.w());
			wBox->setValue(val.w());

			xBox->blockSignals(false);
			yBox->blockSignals(false);
			zBox->blockSignals(false);
			wBox->blockSignals(false);
		}
	}

}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::UnsetState()
{
	IndexT i;
	for (i = 0; i < this->textureVariables.Size(); i++)
	{
		if (this->textureVariables[i].isvalid())
		{
			if (this->textureVariables[i]->IsValid())
			{
				this->stateNode->DiscardMaterialVariableInstance(this->textureVariables[i]);
			}
		}
	}
	this->textureVariables.Clear();
	for (i = 0; i < this->scalarVariables.Size(); i++)
	{
		if (this->scalarVariables[i].isvalid())
		{
			if (this->scalarVariables[i]->IsValid())
			{
				this->stateNode->DiscardMaterialVariableInstance(this->scalarVariables[i]);
			}
		}
	}
	this->scalarVariables.Clear();
	this->stateNode = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeHandler::ClearFrame( QLayout* layout )
{
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
Util::Array<Material::MaterialParameter> 
ModelNodeHandler::GetTextures( const Ptr<Material>& mat )
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
		if (defaultVar.GetType() == Variant::Object && show)
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
Util::Array<Material::MaterialParameter> 
ModelNodeHandler::GetVariables( const Ptr<Material>& mat )
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
ModelNodeHandler::Browse()
{
	// get sender
	QObject* sender = this->sender();

	// must be a button
	QPushButton* button = static_cast<QPushButton*>(sender);

	// get line edit
	QLineEdit* text = this->textureTextMap.key(this->textureImgMap[button]);
	QLabel* name = this->textureLabelMap.key(this->textureImgMap[button]);

	// create string for textures
	IO::URI tex("tex:");
	
	// pick a texture
	int res = ResourceBrowser::TextureBrowser::Instance()->Execute("Assign to: " + name->text());
	if (res == QDialog::Accepted)
	{
		// convert to nebula string
		String texture = ResourceBrowser::TextureBrowser::Instance()->GetSelectedTexture().toUtf8().constData();

		// get category
		String category = texture.ExtractLastDirName();

		// get actual file
		String texFile = texture.ExtractFileName();

		// format text
		String value;
		value.Format("tex:%s/%s", category.AsCharPtr(), texFile.AsCharPtr());

		// set text of item
		text->setText(value.AsCharPtr());

		// invoke texture change function
		this->TextureChanged(this->textureImgMap[button]);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::ChangeColor()
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

		/*
		// update variable
		if (this->scalarVariables[i].isvalid()) this->scalarVariables[i]->SetFloat4(color);

		// save change
		this->itemHandler->OnModelModified();
		*/
		this->Float4VariableChanged(i);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeHandler::MaterialSelected( const QString& material )
{
	// get attributes
	Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();

	// get state and set material
	State state = attributes->GetState(this->nodePath);
	state.material = material.toUtf8().constData();

	// set state
	attributes->SetState(this->nodePath, state);

	// apply modifications
	this->itemHandler->OnModelModified(true);
	this->itemHandler->HardRefresh();
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeHandler::MaterialInfo()
{
	// get attributes
	Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();

	// get material
	Ptr<Material> mat = MaterialDatabase::Instance()->GetMaterial(attributes->GetState(this->nodePath).material);

	// create ui
	Ui::MaterialInfoWidget ui;
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
ModelNodeHandler::TextureChanged(uint i)
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

	// get attributes
	Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();

    // setup texture
    this->SetupTextureSlotHelper(item, img, valueText, this->defaultTextureMap[i].toUtf8().constData());

	// remove eventual file extension
	valueText.StripFileExtension();

	// create new texture
	Texture tex;
	tex.textureName = idText;
	tex.textureResource = valueText;

	// allocate texture
	Ptr<Resources::ManagedTexture> textureObject = Resources::ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Texture::RTTI, valueText + NEBULA3_TEXTURE_EXTENSION).downcast<Resources::ManagedTexture>();
	if (this->textureResources.Contains(i))
	{
		Resources::ResourceManager::Instance()->DiscardManagedResource(this->textureResources[i].upcast<Resources::ManagedResource>());
		this->textureResources.EraseAtIndex(i);
	}
	this->textureResources.Add(i, textureObject);
	if (this->textureVariables[i].isvalid()) this->textureVariables[i]->SetTexture(textureObject->GetTexture());

	// get state
	State state = attributes->GetState(this->nodePath);

	// find the index of the state
	IndexT texIndex = state.textures.FindIndex(tex);

	if (texIndex == InvalidIndex)
	{
		if (valueText.IsValid())
		{
			state.textures.Append(tex);
		}
	}
	else if (valueText != defaultTextureMap[i].toUtf8().constData())
	{
		state.textures[texIndex] = tex;
	}
	else
	{
		state.textures.EraseIndex(texIndex);
	}

	// set state
	attributes->SetState(this->nodePath, state);

	// apply modifications
	this->itemHandler->OnModelModified();
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeHandler::TextureTextChanged()
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
ModelNodeHandler::VariableIntSliderChanged()
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
ModelNodeHandler::VariableIntSliderDone()
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
ModelNodeHandler::VariableFloatSliderChanged()
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
ModelNodeHandler::VariableFloatSliderDone()
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
ModelNodeHandler::VariableIntFieldChanged()
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
	
	// update UI immedately
	QApplication::processEvents();

	// update int
	this->IntVariableChanged(index);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeHandler::VariableFloatFieldChanged()
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
ModelNodeHandler::VariableFloat2FieldChanged()
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
ModelNodeHandler::VariableFloat4FieldChanged()
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
ModelNodeHandler::VariableCheckBoxChanged()
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
ModelNodeHandler::VariableFloatLimitsChanged()
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
ModelNodeHandler::VariableIntLimitsChanged()
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
ModelNodeHandler::FloatVariableChanged( uint i )
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

	// get attributes
	Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();
	
	// get state
	State state = attributes->GetState(this->nodePath);

    // create variable
    Variable var;
    var.variableName = idText;
	IndexT index = state.variables.FindIndex(var);

	if (box->value() != this->defaultValueMap[i].GetFloat())
	{
		if (index == InvalidIndex)
		{            
            // create limits
            Util::KeyValuePair<Util::Variant, Util::Variant> limits(Variant((float)lower->value()), Variant((float)upper->value()));
            var.limits = limits;
            var.variableValue = (scalar)box->value();
			state.variables.Append(var);
		}
		else
		{
            Util::KeyValuePair<Util::Variant, Util::Variant> limits(Variant((float)lower->value()), Variant((float)upper->value()));
			state.variables[index].variableValue = (scalar)box->value();
            state.variables[index].limits = limits;
		}
	}
	else
	{
		if (index != InvalidIndex)
		{
			state.variables.EraseIndex(index);
		}
	}

	// set attribute state
	attributes->SetState(this->nodePath, state);

	// apply modifications
	this->itemHandler->OnModelModified();
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeHandler::Float2VariableChanged( uint i )
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

	// get attributes
	Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();

	// get state
	State state = attributes->GetState(this->nodePath);

    // create variable
    Variable var;
    var.variableName = idText;
	IndexT index = state.variables.FindIndex(var);

	if (floatVec != this->defaultValueMap[i].GetFloat2())
	{
		if (index == InvalidIndex)
		{
            var.variableValue = floatVec;
			state.variables.Append(var);
		}
		else
		{
			state.variables[index].variableValue = floatVec;
		}
	}
	else
	{
		if (index != InvalidIndex)
		{
			state.variables.EraseIndex(index);
		}
	}

	// set attribute state
	attributes->SetState(this->nodePath, state);

	// apply modifications
	this->itemHandler->OnModelModified();
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeHandler::Float4VariableChanged( uint i )
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

	// get attributes
	Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();

	// get state
	State state = attributes->GetState(this->nodePath);

    // create variable
    Variable var;
    var.variableName = idText;
	IndexT index = state.variables.FindIndex(var);

	if (floatVec != this->defaultValueMap[i].GetFloat4())
	{
		if (index == InvalidIndex)
		{
            var.variableValue = floatVec;
			state.variables.Append(var);
		}
		else
		{
			state.variables[index].variableValue = floatVec;
		}
	}
	else
	{
		if (index != InvalidIndex)
		{
			state.variables.EraseIndex(index);
		}
	}

	// set attribute state
	attributes->SetState(this->nodePath, state);

	// apply modifications
	this->itemHandler->OnModelModified();
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeHandler::BoolVariableChanged( uint i )
{
	// get spin box
	QCheckBox* box = this->variableBoolMap.key(i);

	// set value
	if (this->scalarVariables[i].isvalid()) this->scalarVariables[i]->SetValue(box->isChecked());

	// also get label
	QLabel* label = this->variableLabelMap.key(i);

	// format label as text
	String idText = label->text().toUtf8().constData();

	// get attributes
	Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();

	// get state
	State state = attributes->GetState(this->nodePath);

    // create variable
    Variable var;
    var.variableName = idText;
	IndexT index = state.variables.FindIndex(var);

	if (box->isChecked() != this->defaultValueMap[i].GetBool())
	{
		if (index == InvalidIndex)
		{
            var.variableValue = box->isChecked();
			state.variables.Append(var);
		}
		else
		{
			state.variables[index].variableValue = box->isChecked();
		}
	}
	else
	{
		if (index != InvalidIndex)
		{
			state.variables.EraseIndex(index);
		}
	}

	// set attribute state
	attributes->SetState(this->nodePath, state);

	// apply modifications
	this->itemHandler->OnModelModified();
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeHandler::IntVariableChanged( uint i )
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

	// get attributes
	Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();

	// get state
	State state = attributes->GetState(this->nodePath);

    // create variable
    Variable var;
    var.variableName = idText;
	IndexT index = state.variables.FindIndex(var);

	if (box->value() != this->defaultValueMap[i].GetInt())
	{
		if (index == InvalidIndex)
		{
            Util::KeyValuePair<Util::Variant, Util::Variant> limits(Variant(lower->value()), Variant(upper->value()));
            var.limits = limits;
            var.variableValue = box->value();
			state.variables.Append(var);
		}
		else
		{
            Util::KeyValuePair<Util::Variant, Util::Variant> limits(Variant(lower->value()), Variant(upper->value()));
			state.variables[index].variableValue = box->value();
            state.variables[index].limits = limits;
		}
	}
	else
	{
		if (index != InvalidIndex)
		{
			state.variables.EraseIndex(index);
		}
	}

	// set attribute state
	attributes->SetState(this->nodePath, state);

	// apply modifications
	this->itemHandler->OnModelModified();
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeHandler::FloatLimitChanged(uint i)
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
    valueSlider->setMaximum(upper->value()*100);
    valueSlider->setMinimum(lower->value()*100);
    valueSlider->blockSignals(false);

    // manually call variable change
    this->FloatVariableChanged(i);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeHandler::IntLimitChanged(uint i)
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
ModelNodeHandler::SetupTextureSlotHelper( QLineEdit* textureField, QPushButton* textureButton, Util::String& resource, const Util::String& defaultResource )
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
        if (IoServer::Instance()->FileExists(resource))
        {
            QPalette pal;
            textureField->setPalette(pal);

            if (resource.IsValid())
            {
                URI texFile = resource;
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

			URI texFile = defaultResource;
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
		URI texFile = resource;
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
ModelNodeHandler::MakeMaterialUI(QVBoxLayout* mainLayout, QLabel* nodeName, QComboBox* materialBox, QPushButton* materialHelp, const Util::String& material)
{
	// we need to do this, because we might use different layouts
	this->mainLayout = mainLayout;
	this->materialBox = materialBox;
	this->materialHelp = materialHelp;

	// set title of node
	nodeName->setText(this->nodePath.AsCharPtr());

	// clear material list
	materialBox->clear();

	// connect changes to material box to be passed onto this
	connect(materialBox, SIGNAL(activated(const QString&)), this, SLOT(MaterialSelected(const QString&)));
	connect(materialHelp, SIGNAL(clicked()), this, SLOT(MaterialInfo()));

	// format combo box
	if (MaterialDatabase::Instance()->HasMaterialsByType(this->nodeType))
	{
		const Array<Ptr<Materials::Material> >& materials = MaterialDatabase::Instance()->GetMaterialsByType(this->nodeType);
		IndexT i;
		for (i = 0; i < materials.Size(); i++)
		{
			materialBox->addItem(materials[i]->GetName().AsString().AsCharPtr());
		}
	}

	// now find our material and set index
	int index = materialBox->findText(material.AsCharPtr());
	materialBox->setCurrentIndex(index);

	// get state node
	Ptr<PreviewState> previewState = ContentBrowserApp::Instance()->GetPreviewState();
	const Ptr<ModelEntity>& model = previewState->GetModel();

	Ptr<FetchSkinList> fetchSkinsMessage = FetchSkinList::Create();
	__Send(model, fetchSkinsMessage);
	Array<StringAtom> skins = fetchSkinsMessage->GetSkins();
	IndexT i;
	for (i = 0; i < skins.Size(); i++)
	{
		Ptr<Graphics::ShowSkin> showSkin = Graphics::ShowSkin::Create();
		showSkin->SetSkin(skins[i]);
		__Send(model, showSkin);
	}

	this->stateNode = RenderUtil::NodeLookupUtil::LookupStateNodeInstance(model, this->nodePath);

	// get material
	Ptr<Material> mat = MaterialDatabase::Instance()->GetMaterial(material);

	// get attribute
	Ptr<ModelAttributes> attributes = this->itemHandler->GetAttributes();

	// now get state for node
	const State& state = attributes->GetState(this->nodePath);

	// add textures
	Array<Material::MaterialParameter> textures = this->GetTextures(mat);
	for (i = 0; i < textures.Size(); i++)
	{
		// create new horizontal layout 
		QHBoxLayout* varLayout = new QHBoxLayout;

		// copy parameter
		Material::MaterialParameter param = textures[i];

		// create texture variables
		Ptr<MaterialVariableInstance> varInst;
		if (this->stateNode->HasMaterialVariable(param.name)) varInst = this->stateNode->CreateMaterialVariableInstance(param.name);
		this->textureVariables.Add(i, varInst);

		// get texture
		Ptr<Resources::ManagedTexture> textureObject = (Resources::ManagedTexture*)param.defaultVal.GetObject();
		this->defaultTextureMap[i] = textureObject->GetTexture()->GetResourceId().AsString().AsCharPtr();

		// get texture
		Texture tex;
		tex.textureName = param.name;

		IndexT texIndex = state.textures.FindIndex(tex);
		if (texIndex != InvalidIndex)
		{
			param.defaultVal = state.textures[texIndex].textureResource;
		}
		else
		{
			param.defaultVal = textureObject->GetTexture()->GetResourceId().AsString();
		}

		// get texture info
		String name = param.name;
		String res = param.defaultVal.GetString();
		res.ChangeFileExtension("dds");

		Ptr<Resources::ManagedTexture> varTex = Resources::ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Texture::RTTI, res).downcast<Resources::ManagedTexture>();
		this->textureResources.Add(i, varTex);
		if (varInst.isvalid()) varInst->SetTexture(varTex->GetTexture());		

		// create items
		QLabel* texName = new QLabel(name.AsCharPtr());
		QFont font = texName->font();
		font.setBold(true);
		texName->setFont(font);
		QPushButton* texImg = new QPushButton();
		QLineEdit* texRes = new QLineEdit();
		this->SetupTextureSlotHelper(texRes, texImg, res, textureObject->GetTexture()->GetResourceId().AsString());

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
		mainLayout->addLayout(varLayout);
	}

	// add variables
	Array<Material::MaterialParameter> vars = this->GetVariables(mat);
	for (i = 0; i < vars.Size(); i++)
	{
		// get parameter
		Material::MaterialParameter param = vars[i];
		this->defaultValueMap[i] = param.defaultVal;

		// get texture
		Variable variable;
		variable.variableName = param.name;

		Variant min = param.min;
		Variant max = param.max;

		IndexT varIndex = state.variables.FindIndex(variable);
		if (varIndex != InvalidIndex)
		{
			// reset default value if types differ
			if (state.variables[varIndex].variableValue.GetType() != param.defaultVal.GetType())
			{
				state.variables[varIndex].variableValue = param.defaultVal;
				state.variables[varIndex].limits = Util::KeyValuePair<Util::Variant, Util::Variant>(param.min, param.max);
			}
			else
			{
				param.defaultVal = state.variables[varIndex].variableValue;
				min = state.variables[varIndex].limits.Key();
				max = state.variables[varIndex].limits.Value();
			}

			// set limits (may set them to the exact same as before)
			state.variables[varIndex].limits = Util::KeyValuePair<Util::Variant, Util::Variant>(min, max);
		}

		// get texture info
		String name = param.name;
		Variant var = param.defaultVal;

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
		Ptr<MaterialVariableInstance> varInst;
		if (this->stateNode->HasMaterialVariable(param.name)) varInst = this->stateNode->CreateMaterialVariableInstance(param.name);
		if (varInst.isvalid()) varInst->SetValue(var);

		// create material instance
		this->scalarVariables.Add(i, varInst);

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
			mainLayout->addWidget(group);

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
			mainLayout->addWidget(group);
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
			mainLayout->addWidget(group);
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
			mainLayout->addWidget(group);
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
			mainLayout->addWidget(group);
		}
	}

	// add spacer
	mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

} // namespace Widgets