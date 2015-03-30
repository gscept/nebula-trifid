#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::ModelNodeItemHandler
    
    Implements an item handler for ModelNode group boxes
    
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include <QObject>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include "ui_modelnodeinfowidget.h"
#include "modelhandler.h"
#include "materials/material.h"
#include "models/nodes/statenodeinstance.h"
#include "resources/managedtexture.h"

namespace Widgets
{
class ModelNodeHandler : 
	public QObject,
	public Core::RefCounted
{
	Q_OBJECT
	__DeclareClass(ModelNodeHandler);
public:
	/// constructor
	ModelNodeHandler();
	/// destructor
	virtual ~ModelNodeHandler();

	/// sets the ui
	void SetUI(Ui::ModelNodeInfoWidget* ui);
	/// gets the ui
	Ui::ModelNodeInfoWidget* GetUI() const;

	/// set pointer to original item handler
	void SetModelHandler(const Ptr<ModelHandler>& itemHandler);
	/// get pointer to original item handler
	const Ptr<ModelHandler>& GetModelHandler() const;

	/// sets the node name of the handler
	void SetName(const Util::String& name);
	/// gets the node name of the handler
	const Util::String& GetName() const;
	/// sets the node path of the handler
	void SetPath(const Util::String& path);
	/// gets the node path of the handker
	const Util::String& GetPath() const;
	/// set node type
	void SetType(const Util::String& type);
	/// get node
	const Util::String& GetType() const;

	/// constructs the internal structure for the model node item
	virtual void Setup(const Util::String& material, const Util::String& resource);
	/// unset the state (deallocates variable instances and state node)
	virtual void UnsetState();
	/// refreshes the UI
	virtual void HardRefresh(const Util::String& material, const Util::String& resource);
	/// soft refresh, model parameters are changed
	virtual void SoftRefresh(const Util::String& material, const Util::String& resource);

	/// discards a model node handler
	void Discard();

	/// called whenever a texture has been changed
	void TextureChanged(uint i);
	/// called whenever a float parameter has been changed
	void FloatVariableChanged(uint i);
	/// called whenever a float2 parameter has been changed
	void Float2VariableChanged(uint i);
	/// called whenever a float4 parameter has been changed
	void Float4VariableChanged(uint i);
	/// called whenever a bool parameter has been changed
	void BoolVariableChanged(uint i);
	/// called whenever an int variable has been changed
	void IntVariableChanged(uint i);

    /// called whenever a float field limit has changed
    void FloatLimitChanged(uint i);
    /// called whenever an int field limit has changed
    void IntLimitChanged(uint i);

protected:
	/// sets up texture selection button and line edit based on resource
	void SetupTextureSlotHelper(QLineEdit* textureField, QPushButton* textureButton, Util::String& resource, const Util::String& defaultResource);

private slots:
	/// called whenever a material is selected
	void MaterialSelected(const QString& material);
	/// called whenever the material info button is clicked
	void MaterialInfo();

	/// called whenever a line edit is finished
	void TextureTextChanged();
	/// called whenever an int slider has changed its value
	void VariableIntSliderChanged();
	/// called whenever an int slider has stopped
	void VariableIntSliderDone();
	/// called whenever a float slider has changed its value
	void VariableFloatSliderChanged();
	/// called whenever a float slider has stopped
	void VariableFloatSliderDone();
	/// called whenever an int field has changed its value
	void VariableIntFieldChanged();
	/// called whenever a float field has changed its value
	void VariableFloatFieldChanged();
	/// called whenever a float2 field has changed its value
	void VariableFloat2FieldChanged();
	/// called whenever a float4 field has changed its value
	void VariableFloat4FieldChanged();	
	/// called whenever a bool check box has changed its value
	void VariableCheckBoxChanged();

    /// called whenever a float limit box changed its value
    void VariableFloatLimitsChanged();
    /// called whenever an int limit box changed its value
    void VariableIntLimitsChanged();

	/// called whenever the browse button is pressed
	void Browse();	
	/// called whenever a color picker is clicked
	void ChangeColor();

protected:

	/// setup material variables and textures
	void MakeMaterialUI(QVBoxLayout* mainLayout, QLabel* nodeName, QComboBox* materialBox, QPushButton* materialHelp, const Util::String& material);

	/// get material variables which are textures
	Util::Array<Materials::Material::MaterialParameter> GetTextures(const Ptr<Materials::Material>& mat);
	/// get material variables which aren't textures
	Util::Array<Materials::Material::MaterialParameter> GetVariables(const Ptr<Materials::Material>& mat);

	/// clears layout recursively
	void ClearFrame(QLayout* layout);

	bool actionUpdateMode;
	Util::String nodeName;
	Util::String nodePath;
	Util::String nodeType;
	Util::String resource;
	Util::String material;

	QLayout* mainLayout;
	QComboBox* materialBox;
	QPushButton* materialHelp;

	// default values
	QMap<uint, Util::Variant> defaultValueMap;
	QMap<uint, QString> defaultTextureMap;

	// texture
    QMap<QLineEdit*, uint> textureTextMap;
    QMap<QPushButton*, uint> textureImgMap;
    QMap<QLabel*, uint> textureLabelMap;

	// label of variable (generic)
    QMap<QLabel*, uint> variableLabelMap;

	// integer
	QMap<QSpinBox*, uint> variableIntValueMap;
	QMap<QSpinBox*, uint> lowerLimitIntMap;
    QMap<QSpinBox*, uint> upperLimitIntMap;

	// float
	QMap<QDoubleSpinBox*, uint> variableFloatValueMap;
	QMap<QDoubleSpinBox*, uint> lowerLimitFloatMap;
    QMap<QDoubleSpinBox*, uint> upperLimitFloatMap;

	// slider for float or integer
	QMap<QSlider*, uint> variableSliderMap;	

	// float2, float4
	QMap<QDoubleSpinBox*, uint> variableVectorFieldMap;
	QMap<uint, QList<QDoubleSpinBox*> > variableVectorMap;
	QMap<QPushButton*, uint> variableVectorColorEditMap;

	// bool
	QMap<QCheckBox*, uint> variableBoolMap;

	Ui::ModelNodeInfoWidget* ui;
	Ptr<ModelHandler> itemHandler;
	Ptr<Models::StateNodeInstance> stateNode;
	Util::Dictionary<IndexT, Ptr<Resources::ManagedTexture>> textureResources;
	Util::Dictionary<IndexT, Ptr<Materials::MaterialVariableInstance>> textureVariables;
	Util::Dictionary<IndexT, Ptr<Materials::MaterialVariableInstance>> scalarVariables;
	ToolkitUtil::State state;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
ModelNodeHandler::SetUI( Ui::ModelNodeInfoWidget* ui )
{
	n_assert(ui);
	this->ui = ui;
}

//------------------------------------------------------------------------------
/**
*/
inline Ui::ModelNodeInfoWidget* 
ModelNodeHandler::GetUI() const
{
	return this->ui;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ModelNodeHandler::SetName( const Util::String& name )
{
	n_assert(name.IsValid());
	this->nodeName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
ModelNodeHandler::GetName() const
{
	return this->nodeName;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ModelNodeHandler::SetPath( const Util::String& path )
{
	n_assert(path.IsValid());
	this->nodePath = path;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
ModelNodeHandler::GetPath() const
{
	return this->nodePath;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ModelNodeHandler::SetType( const Util::String& type )
{
	this->nodeType = type;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
ModelNodeHandler::GetType() const
{
	return this->nodeType;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ModelNodeHandler::SetModelHandler( const Ptr<ModelHandler>& itemHandler )
{
	n_assert(itemHandler.isvalid());
	this->itemHandler = itemHandler;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ModelHandler>& 
ModelNodeHandler::GetModelHandler() const
{
	return this->itemHandler;
}

} // namespace Widgets
//------------------------------------------------------------------------------