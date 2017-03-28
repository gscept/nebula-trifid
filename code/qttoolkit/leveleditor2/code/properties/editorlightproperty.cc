//------------------------------------------------------------------------------
//  leveleditor2/editorlightproperty.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/editorlightproperty.h"
#include "leveleditor2/leveleditor2protocol.h"
#include "coregraphics/shaperenderer.h"
#include "debugrender/debugshaperenderer.h"
#include "basegamefeature/basegameprotocol.h"
#include "lighting/lighttype.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "posteffect/posteffectmanager.h"
#include "math/polar.h"
#include "graphicsfeatureunit.h"
#include "editorproperty.h"
#include "coregraphics/mesh.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "leveleditor2app.h"


using namespace LevelEditor2;
using namespace Lighting;

namespace LevelEditor2
{

__ImplementClass(LevelEditor2::EditorLightProperty, 'LEPR', GraphicsFeature::LightProperty);

using namespace Game;
using namespace Messaging;
using namespace Graphics;
using namespace Math;
using namespace BaseGameFeature;
using namespace GraphicsFeature;
using namespace Resources;
using namespace CoreGraphics;


//------------------------------------------------------------------------------
/**
*/
EditorLightProperty::EditorLightProperty() :
    lightMesh(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
EditorLightProperty::SetupCallbacks()
{
	this->entity->RegisterPropertyCallback(this, Render);
}

//------------------------------------------------------------------------------
/**
*/
void
EditorLightProperty::SetupAcceptedMessages()
{	
	this->RegisterMessage(SetAttribute::Id);	
	LightProperty::SetupAcceptedMessages();	
}

//------------------------------------------------------------------------------
/**
*/
void
EditorLightProperty::OnActivate()
{
    // create and setup graphics light entity
    int lightType = this->entity->GetInt(Attr::LightType);    
#if (1)//__NEBULA2_EXPORT__
    // map lightcode from n2 to n3
    // nebula2:          in nebula3:
    // Point = 0,        Global = 0,
    // Directional,      Spot,
    // Spot,             Point,

    LightType::Code LightCodesSubstitute[LightType::NumLightTypes]; 
    LightCodesSubstitute[0] = LightType::Global;
    LightCodesSubstitute[1] = LightType::Spot;	
    LightCodesSubstitute[2] = LightType::Point;
    lightType = LightCodesSubstitute[lightType];
#endif

    this->billboard = BillboardEntity::Create();
    this->billboard->SetViewAligned(true);
    this->billboard->SetPickingId(this->GetEntity()->GetUniqueId());

    switch (lightType)
    {
    case LightType::Global:
        this->billboard->SetTexture(Util::String("tex:system/globallight") + NEBULA3_TEXTURE_EXTENSION);
        this->lightMesh = ResourceManager::Instance()->CreateManagedResource(Mesh::RTTI, "msh:system/globallight.nvx2").downcast<ManagedMesh>();
        break;
    case LightType::Spot:
        this->billboard->SetTexture(Util::String("tex:system/light") + NEBULA3_TEXTURE_EXTENSION);
        this->lightMesh = ResourceManager::Instance()->CreateManagedResource(Mesh::RTTI, "msh:system/spotlight.nvx2").downcast<ManagedMesh>();
        break;
    case LightType::Point:
        this->billboard->SetTexture(Util::String("tex:system/light") + NEBULA3_TEXTURE_EXTENSION);
        this->lightMesh = ResourceManager::Instance()->CreateManagedResource(Mesh::RTTI, "msh:system/pointlight.nvx2").downcast<ManagedMesh>();
        break;
    }

    // register with graphics server
    Graphics::Stage* stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();

    // attach billboard and set color
    stage->AttachEntity(this->billboard.upcast<Graphics::GraphicsEntity>());
    this->billboard->SetColor(this->entity->GetFloat4(Attr::LightColor));

    switch (lightType)
    {
    case LightType::Global:
        {
            Property::OnActivate();
            matrix44 trans = PostEffect::PostEffectManager::Instance()->GetDefaultEntity()->Params().light->GetLightTransform();
            this->lightEntity = PostEffect::PostEffectServer::Instance()->GetGlobalLightEntity();
            this->lightEntity->SetTransform(trans);
            this->lightFlickerUtil.SetLightEntity(this->lightEntity);
			this->lightFlickerUtil.SetEnabled(false);
            this->GetEntity()->SetMatrix44(Attr::Transform, trans);
            this->billboard->SetTransform(matrix44::translation(trans.get_position()));
            break;
        }        
    case LightType::Spot:
    case LightType::Point:
        {
            // if we have a spot or point light, run the base class which sets everything up
            LightProperty::OnActivate();
            break;
        }        
    }
}

//------------------------------------------------------------------------------
/**
*/
void
EditorLightProperty::OnDeactivate()
{
    // cleanup billboard
    Graphics::Stage* stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
    stage->RemoveEntity(this->billboard.upcast<Graphics::GraphicsEntity>());
    this->billboard = 0;

    int lightType = this->entity->GetInt(Attr::LightType);  
    switch (lightType)
    {
    case LightType::Global:
        Property::OnDeactivate();
        break;
    case LightType::Spot:
    case LightType::Point:
        LightProperty::OnDeactivate();       
        break;
    }

    this->lightEntity = 0;	
}

//------------------------------------------------------------------------------
/**
*/
void
EditorLightProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	n_assert(msg);
	
	if (msg->CheckId(SetAttribute::Id))
	{
		Ptr<BaseGameFeature::SetAttribute> gmsg = msg.cast<BaseGameFeature::SetAttribute>();		
		this->entity->SetAttr(gmsg->GetAttr());
		Ptr<BaseGameFeature::AttributesUpdated> upd = BaseGameFeature::AttributesUpdated::Create();
		this->entity->SendSync(upd.cast<Messaging::Message>());

        this->billboard->SetColor(this->entity->GetAttr(Attr::LightColor).GetFloat4());
	}
	else if (msg->CheckId(BaseGameFeature::UpdateTransform::Id))
	{		
		if(this->lightEntity->IsInstanceOf(GlobalLightEntity::RTTI))
		{
			Math::matrix44 transform = (msg.cast<BaseGameFeature::UpdateTransform>())->GetMatrix();
			this->billboard->SetTransform(matrix44::translation(transform.get_position()));
            this->lightEntity->SetTransform(transform);
			this->GetEntity()->SetMatrix44(Attr::Transform, transform);
			PostEffect::PostEffectManager::Instance()->GetDefaultEntity()->Params().light->SetLightTransform(transform);
			PostEffect::PostEffectManager::Instance()->GetDefaultEntity()->SetDirty(true);
		}
		else
		{
			LightProperty::HandleMessage(msg);
		}
	}
	else
	{
		LightProperty::HandleMessage(msg);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
EditorLightProperty::OnRender()
{
	LightProperty::OnRender();

	// a bit wonky, get the shadow transform because it will be a normalized version of the light transform (so its valid for rendering a shape)
	Math::matrix44 m = this->entity->GetMatrix44(Attr::Transform);
	int lightType = this->entity->GetInt(Attr::LightType);
	
	// ugly, we need a proxy for rendering from this thread
	if (this->GetEntity()->GetBool(Attr::IsSelected) && this->lightMesh.isvalid())
	{
		// draw mesh as a debug shape
		Debug::DebugShapeRenderer::Instance()->DrawMesh(m, 
				this->lightMesh->GetMesh(),
				0,
				LevelEditor2App::Instance()->GetWindow()->GetSelectionColour(), 
				CoreGraphics::RenderShape::CheckDepth);

		// hide billboard while rendering mesh for global lights
		if (lightType == LightType::Global) this->billboard->SetVisible(false);
	}
	else
	{
		// if we can hide them, we got to show them too!
		if (lightType == LightType::Global) this->billboard->SetVisible(true);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
EditorLightProperty::UpdateLightTransform(const Math::matrix44& transform)
{
	LightProperty::UpdateLightTransform(transform);
	matrix44 billboardTransform = matrix44::translation(transform.get_position());
	this->billboard->SetTransform(billboardTransform);
}

}