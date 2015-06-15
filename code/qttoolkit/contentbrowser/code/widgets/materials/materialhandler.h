#pragma once
//------------------------------------------------------------------------------
/**
	@class Widgets::MaterialHandler
	
	Handles the UI which modifies a single surface material.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/basehandler.h"
#include "ui_materialinfowidget.h"
#include "materials/material.h"
#include "materials/managedsurfacematerial.h"
#include "resources/managedtexture.h"
#include "materials/surfaceconstant.h"
#include <QObject>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include "models/nodes/statenodeinstance.h"
#include "n3util/n3modeldata.h"
namespace Widgets
{
class MaterialHandler : public BaseHandler
{
	Q_OBJECT
	__DeclareClass(MaterialHandler);
public:
	/// constructor
	MaterialHandler();
	/// destructor
	virtual ~MaterialHandler();

    /// setup
    void Setup(const QString& resource);
    /// discard
    bool Discard();

    /// sets the ui on which this handler shall perform its actions
    void SetUI(Ui::MaterialInfoWidget* ui);
    /// gets pointer to ui
    Ui::MaterialInfoWidget* GetUI() const;

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

private slots:
    /// called whenever a material is selected
    void MaterialSelected(const QString& material);
    /// called whenever the material info button is clicked
    void MaterialInfo();

	/// called whenever the name gets changed
	void NameChanged();
	/// called whenever the new button gets pressed
	void NewSurface();
	/// called when the duplicate button gets pressed
	void Duplicate();

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
    /// sets up texture selection button and line edit based on resource
    void SetupTextureSlotHelper(QLineEdit* textureField, QPushButton* textureButton, Util::String& resource, const Util::String& defaultResource);
    /// setup material variables and textures
    void MakeMaterialUI(QVBoxLayout* mainLayout, QLineEdit* surfaceName, QComboBox* materialBox, QPushButton* materialHelp);

    /// get material variables which are textures
    Util::Array<Materials::Material::MaterialParameter> GetTextures(const Ptr<Materials::Material>& mat);
    /// get material variables which aren't textures
    Util::Array<Materials::Material::MaterialParameter> GetVariables(const Ptr<Materials::Material>& mat);

    /// clears layout recursively
    void ClearFrame(QLayout* layout);

private:

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
	Ptr<Materials::ManagedSurfaceMaterial> managedMaterial;
    Ptr<Materials::SurfaceMaterial> material;
    Util::Dictionary<IndexT, Ptr<Resources::ManagedTexture>> textureResources;
    Util::Dictionary<IndexT, Ptr<Materials::SurfaceConstant>> textureVariables;
    Util::Dictionary<IndexT, Ptr<Materials::SurfaceConstant>> scalarVariables;
    ToolkitUtil::State state;

    Ui::MaterialInfoWidget* ui;
};

//------------------------------------------------------------------------------
/**
*/
inline void
MaterialHandler::SetUI(Ui::MaterialInfoWidget* ui)
{
    this->ui = ui;
}

//------------------------------------------------------------------------------
/**
*/
inline Ui::MaterialInfoWidget*
MaterialHandler::GetUI() const
{
    return this->ui;
}

} // namespace Widgets