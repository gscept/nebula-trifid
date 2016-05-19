//------------------------------------------------------------------------------
//  posteffectcontrollerwindow.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include <QFileDialog>
#include <QColorDialog>
#include <QPixmap>
#include "posteffectcontroller.h"
#include "ui_posteffectsettingswindow.h"
#include "math/float4.h"
#include "io/ioserver.h"
#include "graphics/graphicsinterface.h"
#include "posteffect/posteffectprotocol.h"
#include "posteffect/posteffectmanager.h"
#include "posteffect/posteffectparser.h"
#include "posteffect/posteffectregistry.h"
#include <QMessageBox>
#include "environmentprobewindow.h"

using namespace IO;
using namespace Util;
using namespace PostEffect;
using namespace Math;
using namespace Graphics;
namespace QtPostEffectAddon
{

//------------------------------------------------------------------------------
/**
*/
PostEffectController::PostEffectController() :
	postEffectEntity(0),
	modified(false)
{
	// create ui
	this->ui = new Ui::PostEffectWindow;

	// setup ui
	this->ui->setupUi(this);
	this->setWindowFlags(Qt::Tool);

	// connect controls
	connect(this->ui->saturationSlider, SIGNAL(valueChanged(int)), this, SLOT(OnSaturationChanged()));
	connect(this->ui->saturationBox, SIGNAL(valueChanged(double)), this, SLOT(OnSaturationChanged()));
	connect(this->ui->colorBalance, SIGNAL(pressed()), this, SLOT(OnSelectBalance()));
	connect(this->ui->exposureSlider, SIGNAL(valueChanged(int)), this, SLOT(OnLuminanceChanged()));
	connect(this->ui->exposureBox, SIGNAL(valueChanged(double)), this, SLOT(OnLuminanceChanged()));
	connect(this->ui->dofFoc, SIGNAL(valueChanged(double)), this, SLOT(OnDofChanged()));
	connect(this->ui->dofLen, SIGNAL(valueChanged(double)), this, SLOT(OnDofChanged()));
	connect(this->ui->dofRad, SIGNAL(valueChanged(double)), this, SLOT(OnDofChanged()));
	connect(this->ui->hdrThreshold, SIGNAL(valueChanged(double)), this, SLOT(OnHDRChanged()));
	connect(this->ui->hdrBloomIntensity, SIGNAL(valueChanged(double)), this, SLOT(OnHDRChanged()));
	connect(this->ui->hdrColor, SIGNAL(pressed()), this, SLOT(OnSelectHDRColor()));
	connect(this->ui->fogRed, SIGNAL(valueChanged(double)), this, SLOT(OnFogChanged()));
	connect(this->ui->fogGreen, SIGNAL(valueChanged(double)), this, SLOT(OnFogChanged()));
	connect(this->ui->fogBlue, SIGNAL(valueChanged(double)), this, SLOT(OnFogChanged()));
	connect(this->ui->fogAlpha, SIGNAL(valueChanged(double)), this, SLOT(OnFogChanged()));
	connect(this->ui->fogMinDist, SIGNAL(valueChanged(double)), this, SLOT(OnFogChanged()));
	connect(this->ui->fogMaxDist, SIGNAL(valueChanged(double)), this, SLOT(OnFogChanged()));
	connect(this->ui->skyContrast, SIGNAL(valueChanged(double)), this, SLOT(OnSkyChanged()));
	connect(this->ui->skyBrightness, SIGNAL(valueChanged(double)), this, SLOT(OnSkyChanged()));
    connect(this->ui->skyRotation, SIGNAL(valueChanged(double)), this, SLOT(OnSkyChanged()));
	connect(this->ui->skyTexture, SIGNAL(editingFinished()), this, SLOT(OnSkyChanged()));
	connect(this->ui->browseSky, SIGNAL(pressed()), this, SLOT(OnSkyTextureBrowse()));
    connect(this->ui->lightProbeButton, SIGNAL(pressed()), this, SLOT(OnBrowseLightProbe()));
	connect(this->ui->blendSpeed, SIGNAL(valueChanged(double)), this, SLOT(OnBlendChanged()));
	connect(this->ui->lightAmbient, SIGNAL(pressed()), this, SLOT(OnSelectAmbient()));
	connect(this->ui->lightDiffuse, SIGNAL(pressed()), this, SLOT(OnSelectDiffuse()));
	connect(this->ui->lightBack, SIGNAL(pressed()), this, SLOT(OnSelectBack()));
	connect(this->ui->backLightFactor, SIGNAL(valueChanged(int)), this, SLOT(OnBacklightFactorChanged()));
	connect(this->ui->lightIntensity, SIGNAL(valueChanged(int)), this, SLOT(OnLightIntensityChanged()));
	connect(this->ui->lightShadowIntensity, SIGNAL(valueChanged(int)), this, SLOT(OnShadowChanged()));
	connect(this->ui->lightShadowBias, SIGNAL(valueChanged(int)), this, SLOT(OnShadowChanged()));
	connect(this->ui->lightCastShadows, SIGNAL(toggled(bool)), this, SLOT(OnShadowChanged()));
    connect(this->ui->aoStrength, SIGNAL(valueChanged(double)), this, SLOT(OnAOChanged()));
    connect(this->ui->aoAngleBias, SIGNAL(valueChanged(double)), this, SLOT(OnAOChanged()));
    connect(this->ui->aoRadius, SIGNAL(valueChanged(double)), this, SLOT(OnAOChanged()));
    connect(this->ui->aoPower, SIGNAL(valueChanged(double)), this, SLOT(OnAOChanged()));
	connect(this->ui->saveAsButton,SIGNAL(pressed()), this, SLOT(OnSaveAs()));
	connect(this->ui->saveButton, SIGNAL(pressed()), this, SLOT(OnSave()));
	connect(this->ui->resetButton, SIGNAL(pressed()), this, SLOT(OnReset()));
	connect(this->ui->deleteButton, SIGNAL(pressed()), this, SLOT(OnDelete()));
	connect(this->ui->presetCombo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(OnPresetChanged(const QString&)));	
    connect(&this->probeWindow, SIGNAL(accepted()), this, SLOT(OnProbeAccepted()));
    connect(&this->probeWindow, SIGNAL(rejected()), this, SLOT(OnProbeRejected()));
}

//------------------------------------------------------------------------------
/**
*/
PostEffectController::~PostEffectController()
{
	this->postEffectEntity = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnSaturationChanged()
{
	// start blocking signals
	this->ui->saturationSlider->blockSignals(true);
	this->ui->saturationBox->blockSignals(true);

	// get sender
	QObject* sender = this->sender();

	// if the sender is  the slider, set value of box, or vice versa...
	if (dynamic_cast<QSlider*>(sender))
	{
		this->ui->saturationBox->setValue(this->ui->saturationSlider->value() / 100.0f);
	}
	else
	{
		this->ui->saturationSlider->setValue(this->ui->saturationBox->value() * 100);
	}

	// re-engage signals
	this->ui->saturationSlider->blockSignals(false);
	this->ui->saturationBox->blockSignals(false);

	// only apply if post effect entity is valid
	if (this->postEffectEntity.isvalid())
	{
		// get saturation value
		double val = this->ui->saturationBox->value();

		// set params
		this->postEffectEntity->Params().color->SetColorSaturation(val);
		this->SetModified();		
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnSelectBalance()
{
	// get params
	Ptr<ColorParams> params = this->postEffectEntity->Params().color;

	// get color
	float4 color = params->GetColorBalance();

	// convert to qcolor
	QColor qcolor(color.x() * 255, color.y() * 255, color.z() * 255, color.w() * 255);

	// open color dialog
	QColorDialog dialog(qcolor);

	int val = dialog.exec();
	if (val == QDialog::Accepted)
	{
		QColor diaColor = dialog.currentColor();
		color.x() = diaColor.red() / 255.0f;
		color.y() = diaColor.green() / 255.0f;
		color.z() = diaColor.blue() / 255.0f;
		color.w() = diaColor.alpha() / 255.0f;

		this->ui->colorBalance->setPalette(QPalette(diaColor));

		this->postEffectEntity->Params().color->SetColorBalance(color);
		this->SetModified();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnLuminanceChanged()
{
	// start blocking signals
	this->ui->exposureSlider->blockSignals(true);
	this->ui->exposureBox->blockSignals(true);

	// get sender
	QObject* sender = this->sender();

	// if the sender is  the slider, set value of box, or vice versa...
	if (dynamic_cast<QSlider*>(sender))
	{
		this->ui->exposureBox->setValue(this->ui->exposureSlider->value() / 100.0f);
	}
	else
	{
		this->ui->exposureSlider->setValue(this->ui->exposureBox->value() * 100);
	}

	// re-engage signals
	this->ui->exposureSlider->blockSignals(false);
	this->ui->exposureBox->blockSignals(false);

	// only apply if post effect entity is valid
	if (this->postEffectEntity.isvalid())
	{
		// get saturation value
		double val = this->ui->exposureBox->value();

		// set params
		this->postEffectEntity->Params().color->SetColorMaxLuminance(val);
		this->SetModified();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnDofChanged()
{
	// only apply if post effect entity is valid
	if (this->postEffectEntity.isvalid())
	{
		// get saturation value
		double foc = this->ui->dofFoc->value();
		double len = this->ui->dofLen->value();
		double rad = this->ui->dofRad->value();

		// set params
		this->postEffectEntity->Params().dof->SetFilterSize(rad);
		this->postEffectEntity->Params().dof->SetFocusLength(len);
		this->postEffectEntity->Params().dof->SetFocusDistance(foc);
		this->SetModified();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnHDRChanged()
{
	// only apply if post effect entity is valid
	if (this->postEffectEntity.isvalid())
	{
		// get values
		double intensity = this->ui->hdrBloomIntensity->value();
		double threshold = this->ui->hdrThreshold->value();

		// set params
		this->postEffectEntity->Params().hdr->SetHdrBloomIntensity(intensity);
		this->postEffectEntity->Params().hdr->SetHdrBloomThreshold(threshold);
		this->SetModified();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnFogChanged()
{
	// only apply if post effect entity is valid
	if (this->postEffectEntity.isvalid())
	{
		// get saturation value
		double red = this->ui->fogRed->value();
		double green = this->ui->fogGreen->value();
		double blue = this->ui->fogBlue->value();
		double alpha = this->ui->fogAlpha->value();
		double fogMin = this->ui->fogMinDist->value();
		double fogMax = this->ui->fogMaxDist->value();

		// set params
		this->postEffectEntity->Params().fog->SetFogColorAndIntensity(float4(red, green, blue, alpha));
		this->postEffectEntity->Params().fog->SetFogFarDistance(fogMax);
		this->postEffectEntity->Params().fog->SetFogNearDistance(fogMin);
		this->SetModified();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnSkyChanged()
{

	// only apply if post effect entity is valid
	if (this->postEffectEntity.isvalid())
	{
		// get values
		double contrast = this->ui->skyContrast->value();
		double brightness = this->ui->skyBrightness->value();
        double rotation = this->ui->skyRotation->value();
		QString texture = this->ui->skyTexture->text();
		String tex = texture.toUtf8().constData();

		// check if texture exists
		if (!tex.IsValid() || !IoServer::Instance()->FileExists(tex + NEBULA3_TEXTURE_EXTENSION))
		{
			tex = String("tex:system/sky");
		}

		// sent to post effect server
		Ptr<PreloadTexture> msg = PreloadTexture::Create();
		msg->SetResource(tex);
		__StaticSend(GraphicsInterface, msg);		

		// set params
		this->postEffectEntity->Params().sky->SetSkyContrast(contrast);
		this->postEffectEntity->Params().sky->SetSkyBrightness(brightness);
		this->postEffectEntity->Params().sky->SetSkyTexturePath(tex);
        this->postEffectEntity->Params().sky->SetSkyRotationFactor(rotation);
		this->SetModified();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::SetupUiFromEntity()
{
	// only apply if post effect entity is valid
	if (this->postEffectEntity.isvalid())
	{
		Ptr<ColorParams> colorParams = this->postEffectEntity->Params().color;
		Ptr<FogParams> fogParams = this->postEffectEntity->Params().fog;
		Ptr<DepthOfFieldParams> dofParams = this->postEffectEntity->Params().dof;
		Ptr<SkyParams> skyParams = this->postEffectEntity->Params().sky;
		Ptr<HdrParams> hdrParams = this->postEffectEntity->Params().hdr;
		Ptr<LightParams> lightParams = this->postEffectEntity->Params().light;
        Ptr<AoParams> aoParams = this->postEffectEntity->Params().ao;

		// create color object to use for icons
		QColor qcolor;

		// set blend time
		this->ui->blendSpeed->setValue(this->postEffectEntity->Params().common->GetBlendTime());

		// get data from color
		float4 balance = colorParams->GetColorBalance();
		float maxLuminance = colorParams->GetColorMaxLuminance();
		float saturation = colorParams->GetColorSaturation();

		// block all signals
		this->blockSignals(true);

		qcolor = QColor(balance.x() * 255, balance.y() * 255, balance.z() * 255, balance.w() * 255);
		this->ui->colorBalance->setPalette(QPalette(qcolor));
		this->ui->saturationBox->setValue(saturation);
		this->ui->saturationSlider->setValue(saturation * 100);
		this->ui->exposureBox->setValue(maxLuminance);
		this->ui->exposureSlider->setValue(maxLuminance * 100);

		// get data from fog
		float4 fogColor = fogParams->GetFogColorAndIntensity();
		float fogNear = fogParams->GetFogNearDistance();
		float fogFar = fogParams->GetFogFarDistance();

		this->ui->fogRed->setValue(fogColor.x());
		this->ui->fogGreen->setValue(fogColor.y());
		this->ui->fogBlue->setValue(fogColor.z());
		this->ui->fogAlpha->setValue(fogColor.w());
		this->ui->fogMinDist->setValue(fogNear);
		this->ui->fogMaxDist->setValue(fogFar);

		// get data from dof
		float dist = dofParams->GetFocusDistance();
		float len = dofParams->GetFocusLength();
		float rad = dofParams->GetFilterSize();

		this->ui->dofLen->setValue(len);
		this->ui->dofRad->setValue(rad);
		this->ui->dofFoc->setValue(dist);

		// get data from hdr
		float hdrBloomIntensity = hdrParams->GetHdrBloomIntensity();
		float hdrThreshold = hdrParams->GetHdrBloomThreshold();
		float4 hdrColor = hdrParams->GetHdrBloomColor();

		this->ui->hdrBloomIntensity->setValue(hdrBloomIntensity);
		this->ui->hdrThreshold->setValue(hdrThreshold);

		qcolor = QColor(hdrColor.x() * 255, hdrColor.y() * 255, hdrColor.z() * 255, hdrColor.w() * 255);
		this->ui->hdrColor->setPalette(QPalette(qcolor));

		// setup light
		float lightIntensity = lightParams->GetLightIntensity();
        float4 lightAmbient = lightParams->GetLightAmbientColor();
        float4 lightDiffuse = lightParams->GetLightColor();
        float4 lightBack = lightParams->GetLightOppositeColor();
		
        this->lightIntensity = lightIntensity;
		this->lightColor = lightDiffuse;
		this->lightAmbientColor = lightAmbient;
		this->lightOppositeColor = lightBack;

		// set icon
		qcolor = QColor(lightAmbient.x() * 255, lightAmbient.y() * 255, lightAmbient.z() * 255, lightAmbient.w() * 255);
		this->ui->lightAmbient->setPalette(QPalette(qcolor));

		// set icon
		qcolor = QColor(lightDiffuse.x() * 255, lightDiffuse.y() * 255, lightDiffuse.z() * 255, lightDiffuse.w() * 255);
		this->ui->lightDiffuse->setPalette(QPalette(qcolor));

		// set icon
		qcolor = QColor(lightBack.x() * 255, lightBack.y() * 255, lightBack.z() * 255, lightBack.w() * 255);
		this->ui->lightBack->setPalette(QPalette(qcolor));

		// set light intensity
		this->ui->lightIntensity->setValue(lightIntensity * 500);

		float lightShadowIntensity = lightParams->GetLightShadowIntensity();
		float lightShadowBias = lightParams->GetLightShadowBias();
		bool lightCastsShadows = lightParams->GetLightCastsShadows();

		this->ui->lightShadowIntensity->setValue(lightShadowIntensity*100);
		this->ui->lightShadowBias->setValue(lightShadowBias * 10000);
		this->ui->lightCastShadows->setChecked(lightCastsShadows);

		// get sky stuff
		float contrast = skyParams->GetSkyContrast();
		float brightness = skyParams->GetSkyBrightness();
        float rotation = skyParams->GetSkyRotationFactor();
		String tex = skyParams->GetSkyTexturePath();

		this->ui->skyContrast->setValue(contrast);
		this->ui->skyBrightness->setValue(brightness);
        this->ui->skyRotation->setValue(rotation);
		this->ui->skyTexture->setText(tex.AsCharPtr());

        // get ao stuff
        float strength = aoParams->GetStrength();
        float anglebias = aoParams->GetAngleBias();
        float radius = aoParams->GetRadius();
        float power = aoParams->GetPower();

        this->ui->aoStrength->setValue(strength);
        this->ui->aoAngleBias->setValue(anglebias);
        this->ui->aoRadius->setValue(radius);
        this->ui->aoPower->setValue(power);

		// reenable signals
		this->blockSignals(false);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnSkyTextureBrowse()
{
	// create string for textures
	URI tex("tex:");

	// open file dialog
	QFileDialog dialog(QApplication::activeWindow(), tr("Pick texture"), tex.GetHostAndLocalPath().AsCharPtr(), tr("*.dds"));

	// exec dialog
	int result = dialog.exec();
	if (result == QDialog::Accepted)
	{
		// convert to nebula string
		String texture = dialog.selectedFiles()[0].toUtf8().constData();

		// get category
		String category = texture.ExtractLastDirName();

		// get actual file
		String texFile = texture.ExtractFileName();

		// format text
		String value;
		value.Format("tex:%s/%s", category.AsCharPtr(), texFile.AsCharPtr());

		// remove file extension
		value.StripFileExtension();

		// set text of item
		this->ui->skyTexture->setText(value.AsCharPtr());
		
		// call sky change
		this->OnSkyChanged();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnSelectHDRColor()
{
	// get params
	Ptr<HdrParams> params = this->postEffectEntity->Params().hdr;

	// get color
	float4 color = params->GetHdrBloomColor();

	// convert to qcolor
	QColor qcolor(color.x() * 255, color.y() * 255, color.z() * 255, color.w() * 255);

	// open color dialog
	QColorDialog dialog(qcolor);

	int val = dialog.exec();
	if (val == QDialog::Accepted)
	{
		QColor diaColor = dialog.currentColor();
		color.x() = diaColor.red() / 255.0f;
		color.y() = diaColor.green() / 255.0f;
		color.z() = diaColor.blue() / 255.0f;
		color.w() = diaColor.alpha() / 255.0f;

		this->ui->hdrColor->setPalette(QPalette(diaColor));

		params->SetHdrBloomColor(color);
		this->SetModified();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectController::OnBrowseLightProbe()
{
     
    probeWindow.SetIrradianceMap(this->postEffectEntity->Params().sky->GetIrradianceTexturePath());
    probeWindow.SetReflectionMap(this->postEffectEntity->Params().sky->GetReflectanceTexturePath());
    probeWindow.show();
    probeWindow.raise();
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectController::OnProbeAccepted()
{
    Util::String tex = Lighting::EnvironmentProbe::DefaultEnvironmentProbe->GetIrradianceMap()->GetTexture()->GetResourceId().AsString();
    tex.StripFileExtension();
    this->postEffectEntity->Params().sky->SetIrradianceTexturePath(tex);
    tex = Lighting::EnvironmentProbe::DefaultEnvironmentProbe->GetReflectionMap()->GetTexture()->GetResourceId().AsString();
    tex.StripFileExtension();
    this->postEffectEntity->Params().sky->SetReflectanceTexturePath(tex);
    this->SetModified();
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectController::OnProbeRejected()
{
    Lighting::EnvironmentProbe::DefaultEnvironmentProbe->AssignIrradianceMap(this->postEffectEntity->Params().sky->GetIrradianceTexturePath());
    Lighting::EnvironmentProbe::DefaultEnvironmentProbe->AssignReflectionMap(this->postEffectEntity->Params().sky->GetReflectanceTexturePath());
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnBlendChanged()
{
	// only apply blend if entity is valid
	if (this->postEffectEntity.isvalid())
	{
		// get blend
		float blend = this->ui->blendSpeed->value();

		// set blend
		this->postEffectEntity->Params().common->SetBlendTime(blend);
		this->SetModified();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnSelectAmbient()
{
	// get params
	Ptr<LightParams> params = this->postEffectEntity->Params().light;

	// get color
	float4 color = this->lightAmbientColor;

	// convert to qcolor
	QColor qcolor(color.x() * 255, color.y() * 255, color.z() * 255, color.w() * 255);

	// open color dialog
	QColorDialog dialog(qcolor);

	int val = dialog.exec();
	if (val == QDialog::Accepted)
	{
		QColor diaColor = dialog.currentColor();
		color.x() = diaColor.red() / 255.0f;
		color.y() = diaColor.green() / 255.0f;
		color.z() = diaColor.blue() / 255.0f;
		color.w() = diaColor.alpha() / 255.0f;

		this->ui->lightAmbient->setPalette(QPalette(diaColor));

		params->SetLightAmbientColor(color);
		this->lightAmbientColor = color;
		this->SetModified();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnSelectDiffuse()
{
	// get params
	Ptr<LightParams> params = this->postEffectEntity->Params().light;

	// get color
	float4 color = this->lightColor;

	// convert to qcolor
	QColor qcolor(color.x() * 255, color.y() * 255, color.z() * 255, color.w() * 255);

	// open color dialog
	QColorDialog dialog(qcolor);

	int val = dialog.exec();
	if (val == QDialog::Accepted)
	{
		QColor diaColor = dialog.currentColor();
		color.x() = diaColor.red() / 255.0f;
		color.y() = diaColor.green() / 255.0f;
		color.z() = diaColor.blue() / 255.0f;
		color.w() = diaColor.alpha() / 255.0f;

		this->ui->lightDiffuse->setPalette(QPalette(diaColor));

		params->SetLightColor(color);
		this->lightColor = color;
		this->SetModified();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnSelectBack()
{
	// get params
	Ptr<LightParams> params = this->postEffectEntity->Params().light;

	// get color
	float4 color = this->lightOppositeColor;

	// convert to qcolor
	QColor qcolor(color.x() * 255, color.y() * 255, color.z() * 255, color.w() * 255);

	// open color dialog
	QColorDialog dialog(qcolor);

	int val = dialog.exec();
	if (val == QDialog::Accepted)
	{
		QColor diaColor = dialog.currentColor();
		color.x() = diaColor.red() / 255.0f;
		color.y() = diaColor.green() / 255.0f;
		color.z() = diaColor.blue() / 255.0f;
		color.w() = diaColor.alpha() / 255.0f;

		this->ui->lightBack->setPalette(QPalette(diaColor));

		params->SetLightOppositeColor(color);
		this->lightOppositeColor = color;
		this->SetModified();
	}
}


//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnBacklightFactorChanged()
{
	// get params
	Ptr<LightParams> params = this->postEffectEntity->Params().light;

	// get value
	float backlightFactor = this->ui->backLightFactor->value() / 100.0f;

	params->SetBackLightFactor(backlightFactor);
	this->SetModified();
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnLightIntensityChanged()
{
	// get params
	Ptr<LightParams> params = this->postEffectEntity->Params().light;

	// get value
	this->lightIntensity = this->ui->lightIntensity->value() / 500.0f;

	params->SetLightIntensity(this->lightIntensity);
	this->SetModified();
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnShadowChanged()
{
	// get params
	Ptr<LightParams> params = this->postEffectEntity->Params().light;

	// get values
	float shadowIntensity = this->ui->lightShadowIntensity->value() / 100.0f;
	float shadowBias = this->ui->lightShadowBias->value() / 10000.0f;
	bool castShadows = this->ui->lightCastShadows->isChecked();

	// set values
	params->SetLightShadowIntensity(shadowIntensity);
	params->SetLightCastShadows(castShadows);
	params->SetLightShadowBias(shadowBias);
	this->SetModified();
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnAOChanged()
{
    // get params
    Ptr<AoParams> params = this->postEffectEntity->Params().ao;

    // get values
    float strength = this->ui->aoStrength->value();
    float anglebias = this->ui->aoAngleBias->value();
    float radius = this->ui->aoRadius->value();
    float power = this->ui->aoPower->value();

    // set values
    params->SetStrength(strength);
    params->SetAngleBias(anglebias);
    params->SetRadius(radius);
    params->SetPower(power);
	this->SetModified();
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectController::OnReset()
{
	this->OnPresetChanged(this->ui->presetCombo->currentText());
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectController::LoadPresets()
{
	this->ui->presetCombo->blockSignals(true);
	this->ui->presetCombo->clear();
	PostEffect::PostEffectRegistry::Instance()->Clear();
	Util::Array<Util::String> files = IO::IoServer::Instance()->ListFiles(IO::URI("root:data/tables/posteffect"), "*.xml",true);
	IndexT fileIndex;
	for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
	{
		PostEffect::PostEffectEntity::ParamSet parms;
		parms.Init();
		if (ToolkitUtil::PostEffectParser::Load(files[fileIndex], parms))
		{
			PostEffect::PostEffectRegistry::Instance()->SetPreset(parms.common->GetName(), parms);
			this->ui->presetCombo->addItem(parms.common->GetName().AsCharPtr());
		}
	}
	// check if default preset exists
	if (!PostEffect::PostEffectRegistry::Instance()->HasPreset("Default"))
	{
		PostEffect::PostEffectEntity::ParamSet parms;
		parms.Init();
		parms.common->SetName("Default");
		ToolkitUtil::PostEffectParser::Save("root:data/tables/posteffect/defaults.xml", parms);
		PostEffect::PostEffectRegistry::Instance()->SetPreset(parms.common->GetName(), parms);
		this->ui->presetCombo->addItem(parms.common->GetName().AsCharPtr());
	}
	this->ui->presetCombo->blockSignals(false);
	this->ActivatePrefix("Default");
	// forcibly trigger update of preset change, in case there is only one qt will not call the slots
	this->OnPresetChanged("Default");
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectController::OnPresetChanged(const QString& qname)
{
	Util::String name(qname.toLatin1().constData());
	PostEffect::PostEffectRegistry::Instance()->ApplySettings(name, this->postEffectEntity);
	this->SetupUiFromEntity();
	if (qname == "Default")
	{
		this->ui->saveButton->setEnabled(false);
		this->ui->deleteButton->setEnabled(false);
	}
	else
	{
		this->ui->saveButton->setEnabled(true);
		this->ui->deleteButton->setEnabled(true);
	}	
	this->ResetModified();
}


//------------------------------------------------------------------------------
/**
*/
bool
PostEffectController::OnSaveAs()
{
	QFileDialog fileDialog(this, "Save Preset as...", IO::URI("root:data/tables/posteffect").GetHostAndLocalPath().AsCharPtr(), "*.xml");
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);
	fileDialog.setOptions(QFileDialog::HideNameFilterDetails);
	fileDialog.setNameFilterDetailsVisible(false);
	fileDialog.setDefaultSuffix("xml");

	if (fileDialog.exec() == QDialog::Accepted)
	{
		QString fullPath = fileDialog.selectedFiles()[0];
		IO::URI path(fullPath.toUtf8().constData());
		Util::String full = path.GetTail().ExtractFileName();

		full.StripFileExtension();

		bool oldParm = PostEffect::PostEffectRegistry::Instance()->HasPreset(full);

		PostEffect::PostEffectEntity::ParamSet newParms;
		newParms = this->postEffectEntity->Params();
		newParms.common->SetName(full);
		PostEffect::PostEffectRegistry::Instance()->SetPreset(full, newParms);
		ToolkitUtil::PostEffectParser::Save(path.LocalPath(), newParms);
		if (!oldParm)
		{
			this->ui->presetCombo->addItem(full.AsCharPtr());
		}			
		this->ActivatePrefix(full);		
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectController::OnSave()
{
	Util::String current = this->ui->presetCombo->currentText().toLatin1().constData();
	PostEffect::PostEffectEntity::ParamSet newParms;
	newParms = this->postEffectEntity->Params();
	newParms.common->SetName(current);
	PostEffect::PostEffectRegistry::Instance()->SetPreset(current, newParms);
	ToolkitUtil::PostEffectParser::Save("root:data/tables/posteffect/" + current + ".xml", newParms);
	this->ResetModified();
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectController::ResetModified()
{
	this->modified = false;	
	this->ui->saveButton->setEnabled(false);
	this->ui->resetButton->setEnabled(false);
	this->setWindowTitle("PostEffect Preset - " + this->ui->presetCombo->currentText());
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectController::SetModified()
{
	if (!this->modified)
	{
		this->setWindowTitle("PostEffect Preset - " + this->ui->presetCombo->currentText() + " *");
		if (this->ui->presetCombo->currentText() != "Default")
		{
			this->ui->saveButton->setEnabled(true);
		}		
		this->ui->resetButton->setEnabled(true);
	}
	this->modified = true;
	this->postEffectEntity->SetDirty(true);
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectController::ActivatePrefix(const Util::String & name)
{
	this->ui->presetCombo->setCurrentIndex(this->ui->presetCombo->findText(name.AsCharPtr()));	
}

//------------------------------------------------------------------------------
/**
*/
Util::String
PostEffectController::GetPreset()
{
	return this->ui->presetCombo->currentText().toLatin1().constData();
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectController::OnDelete()
{
	Util::String preset = this->ui->presetCombo->currentText().toLatin1().constData();
	IO::IoServer::Instance()->DeleteFile("root:data/tables/posteffect/" + preset + ".xml");
	this->LoadPresets();
	this->ActivatePrefix("Default");
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectController::closeEvent(QCloseEvent * event)
{
	if (this->IsModified())
	{
		QMessageBox box;
		box.setText("The preset has been modified");
		box.setInformativeText("Do you want to save your changes");
		box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
		box.setDefaultButton(QMessageBox::Save);
		int ret = box.exec();
		switch (ret)
		{
		case QMessageBox::Save:
		{
			if (this->GetPreset() == "Default")
			{
				if (!this->OnSaveAs())
				{
					this->OnReset();
				}
			}
			else
			{
				this->OnSave();
			}			
		}			
			break;
		case QMessageBox::Discard:
			this->OnReset();
			break;		
		}
	}
	QDialog::closeEvent(event);
}
} // namespace QtPostEffectAddon