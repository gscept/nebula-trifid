#pragma once
//------------------------------------------------------------------------------
/**
    @class QtPostEffectAddon::PostEffectController
    
    Widget which handles communication from UI to graphics interface.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QDialog>
#include "posteffect/posteffectentity.h"
#include "environmentprobewindow.h"

namespace Ui
{
	class PostEffectWindow;
}
namespace QtPostEffectAddon
{
class PostEffectController : public QDialog
{
	Q_OBJECT
public:
	/// constructor
	PostEffectController();
	/// destructor
	virtual ~PostEffectController();

	/// set post effect entity, will also trigger loading of presets
	void SetPostEffectEntity(const Ptr<PostEffect::PostEffectEntity>& entity);
	/// get post effect entity
	const Ptr<PostEffect::PostEffectEntity>& GetPostEffectEntity() const;	
	/// select a specific post effect prefix
	void ActivatePrefix(const Util::String & name);
	/// load presets from xml tables (not database)
	void LoadPresets();
	/// get selected preset
	Util::String GetPreset();

protected:
	/// qts close handler
	virtual void closeEvent(QCloseEvent * event);
private slots:
	/// called whenever the saturation has changed
	void OnSaturationChanged();
	/// called whenever the balance has changed
	void OnSelectBalance();
	/// called whenever the luminance has changed
	void OnLuminanceChanged();
	/// called whenever the dof has changed
	void OnDofChanged();
	/// called whenever the hdr has changed
	void OnHDRChanged();
	/// called whenever the fog has changed
	void OnFogChanged();
	/// called whenever the sky has changed
	void OnSkyChanged();
	/// called whenever the blend time has changed
	void OnBlendChanged();
	/// called whenever the light ambient has changed
	void OnSelectAmbient();
	/// called whenever the light diffuse has changed
	void OnSelectDiffuse();
	/// called whenever the light back color has changed
	void OnSelectBack();
	/// called whenever the back light color factor has changed
	void OnBacklightFactorChanged();
	/// called whenever the light intensity has changed
	void OnLightIntensityChanged();
	/// called whenever the shadow parameters has changed
	void OnShadowChanged();
    /// called whenever the AO has changed
    void OnAOChanged();
	/// called when reset to default is pressed
	void OnReset();

	/// browses texture for sky
	void OnSkyTextureBrowse();
	/// selects hdr color
	void OnSelectHDRColor();
    ///
    void OnBrowseLightProbe();
    ///
    void OnProbeAccepted();
    ///
    void OnProbeRejected();


	/// save preset button is pressed
	void OnSave();
	/// save preset as button is pressed
	bool OnSaveAs();
	/// delete preset file and setting
	void OnDelete();

	/// preset is selected from combobox
	void OnPresetChanged(const QString& name);

	/// parameters modified
	bool IsModified();
	/// set modified
	void SetModified();


private:	
	/// sets up UI from post effect entity
	void SetupUiFromEntity();
	/// reset modified
	void ResetModified();

	Math::float4 lightColor;
	Math::float4 lightAmbientColor;
	Math::float4 lightOppositeColor;
	float lightIntensity;
	Ui::PostEffectWindow* ui;
	Ptr<PostEffect::PostEffectEntity> postEffectEntity;    
    Lighting::EnvironmentProbeWindow probeWindow;
	bool modified;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
PostEffectController::SetPostEffectEntity( const Ptr<PostEffect::PostEffectEntity>& entity )
{
	n_assert(entity.isvalid());
	this->postEffectEntity = entity;
	this->SetupUiFromEntity();	
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<PostEffect::PostEffectEntity>& 
PostEffectController::GetPostEffectEntity() const
{
	return this->postEffectEntity;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
PostEffectController::IsModified()
{
	return this->modified;	
}
} // namespace QtPostEffectAddon
//------------------------------------------------------------------------------