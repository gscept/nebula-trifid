//------------------------------------------------------------------------------
//  posteffect/posteffectregistry.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "posteffect/posteffectregistry.h"
#include "db/reader.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "db/dbserver.h"
#include "basegamefeature/basegameattr/basegameattributes.h"

namespace PostEffect
{
using namespace Util;

__ImplementClass(PostEffect::PostEffectRegistry, 'PERG', Core::RefCounted);
__ImplementInterfaceSingleton(PostEffect::PostEffectRegistry);

//------------------------------------------------------------------------------
/**
*/
PostEffectRegistry::PostEffectRegistry()
{   
    __ConstructInterfaceSingleton;
}   

//------------------------------------------------------------------------------
/**
*/
PostEffectRegistry::~PostEffectRegistry()
{
    __DestructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectRegistry::OnActivate()
{
	Util::String tableName = "_PostEffect_Presets";
	// open database
	const Ptr<Db::Database> & db = Db::DbServer::Instance()->GetStaticDatabase();
	if (!db->HasTable(tableName))
	{
		n_warning("PostEffectRegistry::OnActivate: failed to load table _PostEffect_Presets in static database");
		return;
	}
	Ptr<Db::Reader> reader = Db::Reader::Create();
	reader->SetDatabase(db);
	reader->SetTableName(tableName);
	bool success = reader->Open();

	int numRows = reader->GetNumRows();
	
	int index;
	for (index = 0; index < numRows; index++)
	{
		reader->SetToRow(index);

		PostEffect::PostEffectEntity::ParamSet parms;
		parms.Init();

		parms.light->SetLightTransform(reader->GetMatrix44(Attr::GlobalLightTransform));
		parms.light->SetLightColor(reader->GetFloat4(Attr::GlobalLightDiffuse));
		parms.light->SetLightOppositeColor(reader->GetFloat4(Attr::GlobalLightOpposite));
		parms.light->SetLightAmbientColor(reader->GetFloat4(Attr::GlobalLightAmbient));
		parms.light->SetLightCastShadows(reader->GetBool(Attr::GlobalLightCastShadows));
		parms.light->SetLightShadowIntensity(reader->GetFloat(Attr::GlobalLightShadowIntensity));
		parms.light->SetLightIntensity(reader->GetFloat(Attr::GlobalLightIntensity));
		parms.light->SetBackLightFactor(reader->GetFloat(Attr::GlobalLightBacklightFactor));
		parms.light->SetLightShadowBias(reader->GetFloat(Attr::GlobalLightShadowBias));
		
		parms.color->SetColorSaturation(reader->GetFloat(Attr::Saturation));
		parms.color->SetColorBalance(reader->GetFloat4(Attr::Balance));
		parms.color->SetColorMaxLuminance(reader->GetFloat(Attr::MaxLuminance));
		
		parms.hdr->SetHdrBloomIntensity(reader->GetFloat(Attr::BloomScale));
		parms.hdr->SetHdrBloomColor(reader->GetFloat4(Attr::BloomColor));
		parms.hdr->SetHdrBloomThreshold(reader->GetFloat(Attr::BloomThreshold));

		parms.fog->SetFogNearDistance(reader->GetFloat(Attr::FogNearDist));
		parms.fog->SetFogFarDistance(reader->GetFloat(Attr::FogFarDist));
		parms.fog->SetFogHeight(reader->GetFloat(Attr::FogHeight));
		parms.fog->SetFogColorAndIntensity(reader->GetFloat4(Attr::FogColor));
		
		parms.dof->SetFocusDistance(reader->GetFloat(Attr::FocusDistance));
		parms.dof->SetFocusLength(reader->GetFloat(Attr::FocusLength));
		parms.dof->SetFilterSize(reader->GetFloat(Attr::FocusRadius));

		parms.sky->SetSkyContrast(reader->GetFloat(Attr::SkyContrast));
		parms.sky->SetSkyBrightness(reader->GetFloat(Attr::SkyBrightness));
		parms.sky->SetSkyTexturePath(reader->GetString(Attr::SkyTexture));
        parms.sky->SetSkyRotationFactor(reader->GetFloat(Attr::SkyRotationFactor));
        if (reader->HasAttr(Attr::ProbeIrradianceMap))
        {
            parms.sky->SetIrradianceTexturePath(reader->GetString(Attr::ProbeIrradianceMap));
        }
        else
        {
            parms.sky->SetIrradianceTexturePath("tex:system/sky_irr");
        }
        if (reader->HasAttr(Attr::ProbeReflectionMap))
        {
            parms.sky->SetReflectanceTexturePath(reader->GetString(Attr::ProbeReflectionMap));
        }
        else
        {
            parms.sky->SetReflectanceTexturePath("tex:system/sky_refl");
        }
				
		parms.ao->SetStrength(reader->GetFloat(Attr::AOStrength));
		parms.ao->SetRadius(reader->GetFloat(Attr::AORadius));
		parms.ao->SetPower(reader->GetFloat(Attr::AOPower));

		parms.common->SetBlendTime(reader->GetFloat(Attr::PEBlendTime));
		Util::String name = reader->GetString(Attr::Id);
		parms.common->SetName(name);

		this->paramRegistry.Add(name, parms);
	}

}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectRegistry::OnDeactivate()
{
	Array<KeyValuePair<String, PostEffect::PostEffectEntity::ParamSet >> parms = this->paramRegistry.Content();
	for (int i = 0; i < parms.Size(); i++)
	{
		parms[i].Value().Discard();
	}
	this->paramRegistry.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectRegistry::ApplySettings(const Util::String & preset, const Ptr<PostEffect::PostEffectEntity> & entity)
{
	n_assert2(this->HasPreset(preset), "Unknown preset");
	entity->Params() = this->paramRegistry[preset];
	entity->SetDirty(true);
}



//------------------------------------------------------------------------------
/**
*/
void
PostEffectRegistry::SetPreset(const Util::String & name, const PostEffectEntity::ParamSet & params)
{
	if (this->HasPreset(name))
	{
		this->paramRegistry[name] = params;
	}
	else
	{
		PostEffectEntity::ParamSet newParms;		
		newParms = params;
		this->paramRegistry.Add(name, newParms);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectRegistry::Clear()
{
	this->paramRegistry.Clear();
}
} // namespace PostEffect
