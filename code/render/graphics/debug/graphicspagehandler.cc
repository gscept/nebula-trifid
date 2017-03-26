//------------------------------------------------------------------------------
//  graphicspagehandler.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/debug/graphicspagehandler.h"
#include "graphics/graphicsserver.h"
#include "graphics/stage.h"
#include "graphics/view.h"
#include "characters/character.h"
#include "models/nodes/statenode.h"
#include "characters/charjointinfo.h"
#include "animation/playclipjob.h"
#include "debugrender/debuggraphicshandler.h"
#include "graphics/graphicsinterface.h"
#include "graphics/billboardentity.h"

namespace Debug
{
__ImplementClass(Debug::GraphicsPageHandler, 'GPHD', Http::HttpRequestHandler);

using namespace Util;
using namespace Http;
using namespace Math;
using namespace Resources;
using namespace Graphics;
using namespace Lighting;
using namespace Characters;
using namespace CoreAnimation;
using namespace Animation;
using namespace Models;
using namespace Billboards;

//------------------------------------------------------------------------------
/**
*/
GraphicsPageHandler::GraphicsPageHandler()
{
    this->SetName("Graphics");
    this->SetDesc("show graphics subsystem s");
    this->SetRootLocation("graphics");
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsPageHandler::HandleRequest(const Ptr<HttpRequest>& request)
{
    n_assert(HttpMethod::Get == request->GetMethod());
    GraphicsServer* server = GraphicsServer::Instance();
    
    // check if a command has been defined in the URI
    Dictionary<String,String> query = request->GetURI().ParseQuery();
    if (query.Contains("stage"))
    {
        request->SetStatus(this->WriteStageInfo(request, ResourceId(query["stage"])));
        return;
    }
    else if (query.Contains("view"))
    {
        request->SetStatus(this->WriteViewInfo(request, ResourceId(query["view"])));
        return;
    }
    else if (query.Contains("entity"))
    {
        request->SetStatus(this->WriteEntityInfo(request, query["entity"].AsInt()));
        return;
    }
    else if (query.Contains("playAnim"))
    {
        request->SetStatus(this->PlayAnim(request, query["playAnim"]));
        return;
    }
    else if (query.Contains("toggleSkin"))
    {
        request->SetStatus(this->ToggleSkin(request, query["toggleSkin"]));
        return;
    }
    else if (query.Contains("applySkinList"))
    {
        request->SetStatus(this->ApplySkinList(request, query["applySkinList"]));
        return;
    }
    else if (query.Contains("applyVariation"))
    {
        request->SetStatus(this->ApplyVariation(request, query["applyVariation"]));
        return;
    }
    else if (query.Contains("modelnodeinstance"))
    {
        request->SetStatus(this->WriteModelNodeInstanceInfo(request, query["modelnodeinstance"]));
        return;
    }
    else if (query.Contains("animSeqHud"))
    {
        request->SetStatus(this->ShowHideAnimSequencerHUD(request, query["animSeqHud"]));
        return;
    }
    else if (query.Contains("togglePerfHud"))
    {
        Ptr<Debug::ControlPerfHUD> msg = Debug::ControlPerfHUD::Create();
        msg->SetToggle(true);
		Graphics::GraphicsInterface::Instance()->Send(msg.upcast<Messaging::Message>());
    }

    // configure a HTML page writer
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(request->GetResponseContentStream());
    htmlWriter->SetTitle("NebulaT  Graphics Subsystem");
    if (htmlWriter->Open())
    {
        htmlWriter->Element(HtmlElement::Heading1, " Graphics Subsystem");
        htmlWriter->AddAttr("href", "/index.html");
        htmlWriter->Element(HtmlElement::Anchor, "Home");

        // link to enable/disable the perf-hud
        htmlWriter->LineBreak();
        htmlWriter->AddAttr("href", "/graphics?togglePerfHud=yes");
        htmlWriter->Element(HtmlElement::Anchor, "Toggle Performance HUD");

        // create a table of all stages
        htmlWriter->Element(HtmlElement::Heading3, "Stages");
        htmlWriter->AddAttr("border", "1");
        htmlWriter->AddAttr("rules", "cols");
        htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->AddAttr("bgcolor", "lightsteelblue");
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableHeader, "Name");
                htmlWriter->Element(HtmlElement::TableHeader, "Entities (All)");
                htmlWriter->Element(HtmlElement::TableHeader, "Models");
                htmlWriter->Element(HtmlElement::TableHeader, "Lights");
                htmlWriter->Element(HtmlElement::TableHeader, "Cameras");
            htmlWriter->End(HtmlElement::TableRow);

            const Array<Ptr<Stage> >& stages = server->GetStages();
            IndexT stageIndex;
            for (stageIndex = 0; stageIndex < stages.Size(); stageIndex++)
            {
                const Ptr<Stage>& stage = stages[stageIndex];
                htmlWriter->Begin(HtmlElement::TableRow);
                    htmlWriter->Begin(HtmlElement::TableData);
                        htmlWriter->AddAttr("href", "/graphics?stage=" + stage->GetName().AsString());
                        htmlWriter->Element(HtmlElement::Anchor, stage->GetName().Value());
                    htmlWriter->End(HtmlElement::TableData);
                    htmlWriter->Element(HtmlElement::TableData, String::FromInt(stage->GetEntities().Size()));
                    htmlWriter->Element(HtmlElement::TableData, String::FromInt(stage->GetEntitiesByType(GraphicsEntityType::Model).Size()));
                    htmlWriter->Element(HtmlElement::TableData, String::FromInt(stage->GetEntitiesByType(GraphicsEntityType::Light).Size()));
                    htmlWriter->Element(HtmlElement::TableData, String::FromInt(stage->GetEntitiesByType(GraphicsEntityType::Camera).Size()));
                htmlWriter->End(HtmlElement::TableRow);
            }
        htmlWriter->End(HtmlElement::Table);

        // create a table of all views
        htmlWriter->Element(HtmlElement::Heading3, "Views");
        htmlWriter->AddAttr("border", "1");
        htmlWriter->AddAttr("rules", "cols");
        htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->AddAttr("bgcolor", "lightsteelblue");
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableHeader, "Name");
                htmlWriter->Element(HtmlElement::TableHeader, "Stage");
                htmlWriter->Element(HtmlElement::TableHeader, "RenderTarget");
                htmlWriter->Element(HtmlElement::TableHeader, "Viewport");
                htmlWriter->Element(HtmlElement::TableHeader, "FrameShader");
            htmlWriter->End(HtmlElement::TableRow);

            const Array<Ptr<View> >& views = server->GetViews();
            IndexT viewIndex;
            for (viewIndex = 0; viewIndex < views.Size(); viewIndex++)
            {
                const Ptr<View>& view = views[viewIndex];
                htmlWriter->Begin(HtmlElement::TableRow);
                    htmlWriter->Begin(HtmlElement::TableData);
                        htmlWriter->AddAttr("href", "/graphics?view=" + view->GetName().AsString());
                        htmlWriter->Element(HtmlElement::Anchor, view->GetName().Value());
                    htmlWriter->End(HtmlElement::TableData);
                    htmlWriter->Begin(HtmlElement::TableData);
                        htmlWriter->AddAttr("href", "/graphics?stage=" + view->GetStage()->GetName().AsString());
                        htmlWriter->Element(HtmlElement::Anchor, view->GetStage()->GetName().Value());
                    htmlWriter->End(HtmlElement::TableData);
                    htmlWriter->Begin(HtmlElement::TableData);                        
					htmlWriter->AddAttr("href", "/display");
					htmlWriter->Element(HtmlElement::Anchor, "default");
                    htmlWriter->End(HtmlElement::TableData);
                    htmlWriter->Begin(HtmlElement::TableData);
                    if (view->UseResolveRect())
                    {
                        const Math::rectangle<int>& rect = view->GetResolveRect();
                        String rectString;
                        rectString.Format("origin: %d,%d; dimensions: %d,%d", rect.left, rect.top, rect.width(), rect.height());
                        htmlWriter->Element(HtmlElement::Anchor, rectString);
                    }
                    else
                    {
                        htmlWriter->Element(HtmlElement::Anchor, "screen");
                    }
                    htmlWriter->End(HtmlElement::TableData);
                    if (view->GetFrameScript().isvalid()) htmlWriter->Element(HtmlElement::TableData, view->GetFrameScript()->GetResourceId().Value());
                htmlWriter->End(HtmlElement::TableRow);
            }
        htmlWriter->End(HtmlElement::Table);

        // create table of all entities
        htmlWriter->Element(HtmlElement::Heading3, "Entities");
        const Array<Ptr<GraphicsEntity> > entities = server->GetEntities();
        this->WriteEntityTable(entities, htmlWriter);

        htmlWriter->Close();
        request->SetStatus(HttpStatus::OK);
    }
    else
    {
        request->SetStatus(HttpStatus::InternalServerError);
    }
}

//------------------------------------------------------------------------------
/**
    Write a table with GraphicsEntities.
*/
void
GraphicsPageHandler::WriteEntityTable(const Array<Ptr<GraphicsEntity> >& entities, const Ptr<HtmlPageWriter>& htmlWriter)
{
    htmlWriter->AddAttr("border", "1");
    htmlWriter->AddAttr("rules", "cols");
    htmlWriter->Begin(HtmlElement::Table);
        htmlWriter->AddAttr("bgcolor", "lightsteelblue");
        htmlWriter->Begin(HtmlElement::TableRow);                
            htmlWriter->Element(HtmlElement::TableHeader, "Id");
            htmlWriter->Element(HtmlElement::TableHeader, "Type");
            htmlWriter->Element(HtmlElement::TableHeader, "ResId");
			htmlWriter->Element(HtmlElement::TableHeader, "Hidden");
            htmlWriter->Element(HtmlElement::TableHeader, "Visible");
            htmlWriter->Element(HtmlElement::TableHeader, "Stage");
            htmlWriter->Element(HtmlElement::TableHeader, "Pos");
            htmlWriter->Element(HtmlElement::TableHeader, "Size");
			htmlWriter->Element(HtmlElement::TableHeader, "Instanced");
        htmlWriter->End(HtmlElement::TableRow);

        IndexT entityIndex;
        for (entityIndex = 0; entityIndex < entities.Size(); entityIndex++)
        {
            const Ptr<GraphicsEntity>& entity = entities[entityIndex];
            GraphicsEntityType::Code type = entity->GetType();     
            const point& pos = entity->GetTransform().get_position();
            const vector& size = entity->GetGlobalBoundingBox().size();

            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Begin(HtmlElement::TableData);
                    htmlWriter->AddAttr("href", "/graphics?entity=" + String::FromInt(entity->GetId()));
                    htmlWriter->Element(HtmlElement::Anchor, String::FromInt(entity->GetId()));
                htmlWriter->End(HtmlElement::TableData);                    
                if (GraphicsEntityType::Model == type)
                {
					if (entity->IsA(BillboardEntity::RTTI))
					{
						htmlWriter->Element(HtmlElement::TableData, "Model (Billboard)");
						htmlWriter->Element(HtmlElement::TableData, "-");
					}
					else
					{
						const Ptr<ModelEntity>& modelEntity = entity.downcast<ModelEntity>();
						if (modelEntity->HasCharacter())
						{
							htmlWriter->Element(HtmlElement::TableData, "Model (Char)");
						}
						else
						{
							htmlWriter->Element(HtmlElement::TableData, "Model");
						}
						htmlWriter->Element(HtmlElement::TableData, modelEntity->GetResourceId().Value());
					}

                }
                else if (GraphicsEntityType::Camera == type)
                {
                    htmlWriter->Element(HtmlElement::TableData, "Camera");
                    htmlWriter->Element(HtmlElement::TableData, "-");
                }
                else if (GraphicsEntityType::Light == type)
                {
                    htmlWriter->Element(HtmlElement::TableData, "Light");
                    htmlWriter->Element(HtmlElement::TableData, "-");
                }
                else
                {
                    htmlWriter->Element(HtmlElement::TableData, "UNKNOWN");
                    htmlWriter->Element(HtmlElement::TableData, "-");
                }
                htmlWriter->Element(HtmlElement::TableData, String::FromBool(!entity->IsVisible()));
				htmlWriter->Element(HtmlElement::TableData, String::FromBool(entity->GetClipStatus() != ClipStatus::Outside));
                if (entity->IsAttachedToStage())
                {
                    htmlWriter->Begin(HtmlElement::TableData);
                        htmlWriter->AddAttr("href", "/graphics?stage=" + entity->GetStage()->GetName().AsString());
                        htmlWriter->Element(HtmlElement::Anchor, entity->GetStage()->GetName().Value());
                    htmlWriter->End(HtmlElement::TableData);
                }
                else
                {
                    htmlWriter->Element(HtmlElement::TableData, "-");
                }
                String posString, sizeString;
                posString.Format("%.3f,%.3f,%.3f", pos.x(), pos.y(), pos.z());
                sizeString.Format("%.3f,%.3f,%.3f", size.x(), size.y(), size.z());
                htmlWriter->Element(HtmlElement::TableData, posString);
                htmlWriter->Element(HtmlElement::TableData, sizeString);
				if (entity->IsA(ModelEntity::RTTI))
				{
					if (entity.cast<ModelEntity>()->IsInstanced())
					{
						htmlWriter->Element(HtmlElement::TableData,"true");
					}
					else
					{
						htmlWriter->Element(HtmlElement::TableData,"false");
					}
				}
				else
				{	
					htmlWriter->Element(HtmlElement::TableData,"N/A");
				}
            htmlWriter->End(HtmlElement::TableRow);
        }
    htmlWriter->End(HtmlElement::Table);
}

//------------------------------------------------------------------------------
/**
*/
HttpStatus::Code
GraphicsPageHandler::WriteStageInfo(const Ptr<HttpRequest>& request, const ResourceId& stageName)
{
    GraphicsServer* server = GraphicsServer::Instance();
    if (server->HasStage(stageName))
    {
        Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
        htmlWriter->SetStream(request->GetResponseContentStream());
        htmlWriter->SetTitle("Graphics Stage Info");
        if (htmlWriter->Open())
        {
            htmlWriter->Element(HtmlElement::Heading1, "Graphics Stage: " + stageName.AsString());
            htmlWriter->AddAttr("href", "/index.html");
            htmlWriter->Element(HtmlElement::Anchor, "Home");
            htmlWriter->LineBreak();
            htmlWriter->AddAttr("href", "/graphics");
            htmlWriter->Element(HtmlElement::Anchor, "Graphics Home");
            htmlWriter->LineBreak();

            const Ptr<Stage>& stage = server->GetStageByName(stageName);
            const Array<Ptr<GraphicsEntity> >& entities = stage->GetEntities();
            this->WriteEntityTable(entities, htmlWriter);

            htmlWriter->Close();
            return HttpStatus::OK;            
        }
        else
        {
            return HttpStatus::InternalServerError;
        }
    }
    else
    {
        return HttpStatus::NotFound;
    }
}

//------------------------------------------------------------------------------
/**
*/
HttpStatus::Code
GraphicsPageHandler::WriteViewInfo(const Ptr<HttpRequest>& request, const ResourceId& viewName)
{
    // @todo: implement GraphicsPageHandler::WriteViewInfo
    return HttpStatus::NotFound;
}

//------------------------------------------------------------------------------
/**
*/
HttpStatus::Code
GraphicsPageHandler::WriteEntityInfo(const Ptr<HttpRequest>& request, GraphicsEntity::Id entityId)
{
    GraphicsServer* server = GraphicsServer::Instance();
    if (server->HasEntity(entityId))
    {
        const Ptr<GraphicsEntity>& entity = server->GetEntityById(entityId);
        Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
        htmlWriter->SetStream(request->GetResponseContentStream());
        htmlWriter->SetTitle("Graphics Entity Info");
        if (htmlWriter->Open())
        {
            htmlWriter->Element(HtmlElement::Heading1, " Graphics Entity Info");
            htmlWriter->AddAttr("href", "/index.html");
            htmlWriter->Element(HtmlElement::Anchor, "Home");
            htmlWriter->LineBreak();
            htmlWriter->AddAttr("href", "/graphics");
            htmlWriter->Element(HtmlElement::Anchor, "Graphics Home");
            htmlWriter->LineBreak();

            htmlWriter->Element(HtmlElement::Heading3, "Generic Info:");
            htmlWriter->Begin(HtmlElement::Table);
                htmlWriter->Begin(HtmlElement::TableRow);
                    htmlWriter->Element(HtmlElement::TableData, "Entity Id: ");
                    htmlWriter->Element(HtmlElement::TableData, String::FromInt(entityId));
                htmlWriter->End(HtmlElement::TableRow);
                htmlWriter->Begin(HtmlElement::TableRow);
                    htmlWriter->Element(HtmlElement::TableData, "Active: ");
                    htmlWriter->Element(HtmlElement::TableData, entity->IsActive() ? "yes" : "no");
                htmlWriter->End(HtmlElement::TableRow);
                htmlWriter->Begin(HtmlElement::TableRow);
                    htmlWriter->Element(HtmlElement::TableData, "Valid: ");
                    htmlWriter->Element(HtmlElement::TableData, entity->IsValid() ? "yes" : "no");
                htmlWriter->End(HtmlElement::TableRow);
                htmlWriter->Begin(HtmlElement::TableRow);
                    htmlWriter->Element(HtmlElement::TableData, "Visible: ");
                    htmlWriter->Element(HtmlElement::TableData, entity->IsVisible() ? "yes" : "no");
                htmlWriter->End(HtmlElement::TableRow);
                htmlWriter->Begin(HtmlElement::TableRow);
                    htmlWriter->Element(HtmlElement::TableData, "Position: ");
                    htmlWriter->Element(HtmlElement::TableData, String::FromFloat4(entity->GetTransform().get_position()));
                htmlWriter->End(HtmlElement::TableRow);
                htmlWriter->Begin(HtmlElement::TableRow);
                    htmlWriter->Element(HtmlElement::TableData, "Size: ");
                    htmlWriter->Element(HtmlElement::TableData, String::FromFloat4(entity->GetGlobalBoundingBox().size()));
                htmlWriter->End(HtmlElement::TableRow);
            htmlWriter->End(HtmlElement::Table);

            if (GraphicsEntityType::Model == entity->GetType())
            {
                htmlWriter->Element(HtmlElement::Heading3, "Model Entity Info:");
                this->WriteModelEntityInfo(entity.downcast<ModelEntity>(), htmlWriter);
            }
            else if (GraphicsEntityType::Light == entity->GetType())
            {
                htmlWriter->Element(HtmlElement::Heading3, "Light Entity Info:");
                this->WriteLightEntityInfo(entity.downcast<AbstractLightEntity>(), htmlWriter);
            }
            else if (GraphicsEntityType::Camera == entity->GetType())
            {
                htmlWriter->Element(HtmlElement::Heading3, "Camera Entity Info:");
                this->WriteCameraEntityInfo(entity.downcast<CameraEntity>(), htmlWriter);
            }                    
            htmlWriter->Close();
            return HttpStatus::OK;
        }
        else
        {
            return HttpStatus::InternalServerError;
        }
    }
    else
    {
        return HttpStatus::NotFound;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsPageHandler::WriteModelEntityInfo(const Ptr<ModelEntity>& modelEntity, const Ptr<HtmlPageWriter>& htmlWriter)
{
    htmlWriter->Begin(HtmlElement::Table);
        htmlWriter->Begin(HtmlElement::TableRow);
            htmlWriter->Element(HtmlElement::TableData, "Resource: ");
            htmlWriter->Element(HtmlElement::TableData, modelEntity->GetResourceId().Value());
        htmlWriter->End(HtmlElement::TableRow);
    htmlWriter->End(HtmlElement::Table);
    
    // write character specific info
    if (modelEntity->HasCharacter())
    {
        const Ptr<Character>& character = modelEntity->GetCharacter();

        htmlWriter->Element(HtmlElement::Heading3, "Character Instance");

        // write state of running animations and a button to enable/disable the anim sequencer debug visualization
        htmlWriter->Element(HtmlElement::Heading4, "Animation Clips");
        const Ptr<CharacterInstance>& characterInst = modelEntity->GetCharacterInstance();
        if (characterInst->AnimController().AnimSequencer().IsDebugHudEnabled())
        {            
            String linkStr;
            linkStr.Format("/graphics?animSeqHud=%d,off", modelEntity->GetId());
            htmlWriter->AddAttr("href", linkStr);
            htmlWriter->Element(HtmlElement::Anchor, "Hide AnimSequencer Debug HUD");
        }
        else
        {
            String linkStr;
            linkStr.Format("/graphics?animSeqHud=%d,on", modelEntity->GetId());
            htmlWriter->AddAttr("href", linkStr);
            htmlWriter->Element(HtmlElement::Anchor, "Show AnimSequencer Debug HUD");
        }
        htmlWriter->LineBreak();

        htmlWriter->AddAttr("border", "1");
        htmlWriter->AddAttr("rules", "cols");
        htmlWriter->Begin(HtmlElement::Table);
        htmlWriter->AddAttr("bgcolor", "lightsteelblue");
        htmlWriter->Begin(HtmlElement::TableRow);
        htmlWriter->Element(HtmlElement::TableHeader, "Name");
        htmlWriter->Element(HtmlElement::TableHeader, "Track Index");
        htmlWriter->Element(HtmlElement::TableHeader, "Infinite");
        htmlWriter->Element(HtmlElement::TableHeader, "BlendWeight");
        htmlWriter->Element(HtmlElement::TableHeader, "TimeFactor");        
        htmlWriter->End(HtmlElement::TableRow);    

        IndexT jobIndex;
        const Array<Ptr<AnimJob> >& jobs = characterInst->AnimController().AnimSequencer().GetAllAnimJobs();
        for (jobIndex = 0; jobIndex < jobs.Size(); jobIndex++)
        {               
            htmlWriter->Begin(HtmlElement::TableRow);        
            htmlWriter->AddAttr("valign", "top");    
            htmlWriter->Element(HtmlElement::TableData, jobs[jobIndex]->GetName().Value());
            htmlWriter->Element(HtmlElement::TableData, String::FromInt(jobs[jobIndex]->GetTrackIndex()));
            htmlWriter->Element(HtmlElement::TableData, String::FromBool(jobs[jobIndex]->IsInfinite()));
            htmlWriter->Element(HtmlElement::TableData, String::FromFloat(jobs[jobIndex]->GetBlendWeight()));
            htmlWriter->Element(HtmlElement::TableData, String::FromFloat(jobs[jobIndex]->GetTimeFactor()));
            htmlWriter->End(HtmlElement::TableRow);            
        }                                          
        htmlWriter->End(HtmlElement::Table);

        // write anim driven motion info
        if (modelEntity->IsAnimDrivenMotionTrackingEnabled())
        {
            htmlWriter->Element(HtmlElement::Heading4, "Anim Driven Motion Tracking");
            htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->TableRow2("Joint Name:", modelEntity->GetAnimDrivenMotionJointName().Value());
            htmlWriter->TableRow2("Motion Vector:", String::FromFloat4(modelEntity->GetAnimDrivenMotionVector()));
            htmlWriter->End(HtmlElement::Table);
        }
        else
        {
            htmlWriter->Element(HtmlElement::Heading4, "Anim Driven Motion (Disabled)");
        }

        // write character joint tracking info
        if (modelEntity->IsCharJointTrackingEnabled())
        {
            if (modelEntity->IsCharJointDataValid())
            {
                htmlWriter->Element(HtmlElement::Heading4, "Character Joint Tracking");
                htmlWriter->AddAttr("border", "1");
                htmlWriter->AddAttr("rules", "cols");
                htmlWriter->Begin(HtmlElement::Table);
                    htmlWriter->AddAttr("bgcolor", "lightsteelblue");
                    htmlWriter->Begin(HtmlElement::TableRow);
                        htmlWriter->Element(HtmlElement::TableHeader, "Joint Name");
                        htmlWriter->Element(HtmlElement::TableHeader, "Joint Index");
                        htmlWriter->Element(HtmlElement::TableHeader, "Local Pos");
                        htmlWriter->Element(HtmlElement::TableHeader, "Global Pos");
                    htmlWriter->End(HtmlElement::TableRow);

                    const Array<CharJointInfo>& jointInfos = modelEntity->GetCharJointInfos();
                    IndexT i;
                    for (i = 0; i < jointInfos.Size(); i++)
                    {
                        const CharJointInfo& jointInfo = jointInfos[i];
                        htmlWriter->Begin(HtmlElement::TableRow);
                            htmlWriter->Element(HtmlElement::TableData, jointInfo.GetJointName().Value());
                            htmlWriter->Element(HtmlElement::TableData, String::FromFloat4(jointInfo.GetLocalMatrix().get_position()));
                            htmlWriter->Element(HtmlElement::TableData, String::FromFloat4(jointInfo.GetGlobalMatrix().get_position()));
                        htmlWriter->End(HtmlElement::TableRow);
                    }
                htmlWriter->End(HtmlElement::Table);
            }
            else
            {
                htmlWriter->Element(HtmlElement::Heading4, "Character Joint Tracking (Data Not Updated This Frame)");
            }
        }
        else
        {
            htmlWriter->Element(HtmlElement::Heading4, "Character Joint Tracking (Disabled)");
        }

        // write character skins, all animations available
        htmlWriter->Element(HtmlElement::Heading4, "Character Infos");
        htmlWriter->AddAttr("border", "1");
        htmlWriter->AddAttr("rules", "cols");
        htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->AddAttr("bgcolor", "lightsteelblue");
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableHeader, "Clips");
                htmlWriter->Element(HtmlElement::TableHeader, "Variations");
                htmlWriter->Element(HtmlElement::TableHeader, "Skins");
                htmlWriter->Element(HtmlElement::TableHeader, "Skin Lists");
            htmlWriter->End(HtmlElement::TableRow);

            htmlWriter->Begin(HtmlElement::TableRow);
                // write available animations
                htmlWriter->AddAttr("valign", "top");
                htmlWriter->Begin(HtmlElement::TableData);
                    IndexT clipIndex;
                    const Ptr<AnimResource>& anim = character->AnimationLibrary().GetAnimResource();
                    for (clipIndex = 0; clipIndex < anim->GetNumClips(); clipIndex++)
                    {
                        const AnimClip& clip = anim->GetClipByIndex(clipIndex);
                        String playAnimLink;
                        playAnimLink.Format("/graphics?playAnim=%d,%s", modelEntity->GetId(), clip.GetName().Value());
                        htmlWriter->AddAttr("href", playAnimLink);
                        htmlWriter->Element(HtmlElement::Anchor, clip.GetName().Value());
                        htmlWriter->LineBreak();
                    }
                htmlWriter->End(HtmlElement::TableData);

                // write variations
                htmlWriter->AddAttr("valign", "top");
                htmlWriter->Begin(HtmlElement::TableData);
                IndexT varListIndex;
                for (varListIndex = 0; varListIndex < character->VariationLibrary().GetNumVariations(); varListIndex++)
                {
                    const Util::StringAtom& variationName = character->VariationLibrary().GetVariationNameAtIndex(varListIndex);
                    String applyVariationLink;
                    applyVariationLink.Format("/graphics?applyVariation=%d,%s", modelEntity->GetId(), variationName.Value());
                    htmlWriter->AddAttr("href", applyVariationLink);
                    htmlWriter->Element(HtmlElement::Anchor, variationName.Value());
                    htmlWriter->LineBreak();
                }
                htmlWriter->End(HtmlElement::TableData);
                
                // write skins
                htmlWriter->AddAttr("valign", "top");
                htmlWriter->Begin(HtmlElement::TableData);
                    IndexT skinIndex;
                    for (skinIndex = 0; skinIndex < character->SkinLibrary().GetNumSkins(); skinIndex++)
                    {
                        const CharacterSkin& skin = character->SkinLibrary().GetSkin(skinIndex);
                        String toggleSkinLink;
                        toggleSkinLink.Format("/graphics?toggleSkin=%d,%s", modelEntity->GetId(), skin.GetName().Value());
                        htmlWriter->AddAttr("href", toggleSkinLink);
                        htmlWriter->Element(HtmlElement::Anchor, skin.GetName().Value());
                        htmlWriter->LineBreak();
                    }
                htmlWriter->End(HtmlElement::TableData);

                // write skin lists
                htmlWriter->AddAttr("valign", "top");
                htmlWriter->Begin(HtmlElement::TableData);
                    IndexT skinListIndex;
                    for (skinListIndex = 0; skinListIndex < character->SkinLibrary().GetNumSkinLists(); skinListIndex++)
                    {
                        const CharacterSkinList& skinList = character->SkinLibrary().GetSkinList(skinListIndex);
                        String applySkinListLink;
                        applySkinListLink.Format("/graphics?applySkinList=%d,%s", modelEntity->GetId(), skinList.GetName().Value());
                        htmlWriter->AddAttr("href", applySkinListLink);
                        htmlWriter->Element(HtmlElement::Anchor, skinList.GetName().Value());
                        htmlWriter->LineBreak();
                    }
                htmlWriter->End(HtmlElement::TableData);
                                                                 
            htmlWriter->End(HtmlElement::TableRow);
        htmlWriter->End(HtmlElement::Table);
    }
    // for static models write their model hierarchy
    else
    {
        this->WriteModelInstanceHierarchy(modelEntity->GetId(), modelEntity->GetModelInstance(), htmlWriter);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsPageHandler::WriteLightEntityInfo(const Ptr<AbstractLightEntity>& lightEntity, const Ptr<HtmlPageWriter>& htmlWriter)
{
    const matrix44& m = lightEntity->GetTransform();
    float xSize = m.get_xaxis().length();
    float ySize = m.get_yaxis().length();
    float zSize = m.get_zaxis().length();
    vector size(xSize, ySize, zSize);

    htmlWriter->Begin(HtmlElement::Table);
        htmlWriter->Begin(HtmlElement::TableRow);
            htmlWriter->Element(HtmlElement::TableData, "Type: ");
            htmlWriter->Element(HtmlElement::TableData, LightType::ToString(lightEntity->GetLightType()));
        htmlWriter->End(HtmlElement::TableRow);
        htmlWriter->Begin(HtmlElement::TableRow);
            htmlWriter->Element(HtmlElement::TableData, "Position: ");
            htmlWriter->Element(HtmlElement::TableData, String::FromFloat4(m.get_position()));
        htmlWriter->End(HtmlElement::TableRow);
        htmlWriter->Begin(HtmlElement::TableRow);
            htmlWriter->Element(HtmlElement::TableData, "Size: ");
            htmlWriter->Element(HtmlElement::TableData, String::FromFloat4(size));
        htmlWriter->End(HtmlElement::TableRow);
        htmlWriter->Begin(HtmlElement::TableRow);
            htmlWriter->Element(HtmlElement::TableData, "Direction: ");
            htmlWriter->Element(HtmlElement::TableData, String::FromFloat4(m.get_zaxis()));
        htmlWriter->End(HtmlElement::TableRow);
        htmlWriter->Begin(HtmlElement::TableRow);
            htmlWriter->Element(HtmlElement::TableData, "Color: ");
            htmlWriter->Element(HtmlElement::TableData, String::FromFloat4(lightEntity->GetColor()));             
        htmlWriter->End(HtmlElement::TableRow);
        htmlWriter->Begin(HtmlElement::TableRow);
            htmlWriter->Element(HtmlElement::TableData, "Cast Shadows: ");
            htmlWriter->Element(HtmlElement::TableData, String::FromBool(lightEntity->GetCastShadows()));
        htmlWriter->End(HtmlElement::TableRow);
    htmlWriter->End(HtmlElement::Table);
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsPageHandler::WriteCameraEntityInfo(const Ptr<CameraEntity>& cameraEntity, const Ptr<HtmlPageWriter>& htmlWriter)
{
    // FIXME!
}

//------------------------------------------------------------------------------
/**
    Lookup model entity by numeric entity id, returns invalid Ptr<>
    if not found or the ModelEntity has no character.
*/
Ptr<ModelEntity>
GraphicsPageHandler::LookupCharacterModelEntityById(GraphicsEntity::Id entityId)
{
    Ptr<ModelEntity> result;
    GraphicsServer* server = GraphicsServer::Instance();
    if (server->HasEntity(entityId))
    {
        const Ptr<GraphicsEntity>& entity = server->GetEntityById(entityId);
        n_assert(entity.isvalid());
        if (entity->GetType() == GraphicsEntityType::Model)
        {
            const Ptr<ModelEntity>& modelEntity = entity.downcast<ModelEntity>();
            if (modelEntity->HasCharacter())
            {
                result = modelEntity;
            }
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
*/
HttpStatus::Code
GraphicsPageHandler::PlayAnim(const Ptr<HttpRequest>& request, const String& argString)
{
    GraphicsServer* server = GraphicsServer::Instance();

    // parse arg string, must be "entityId,clipIndex"
    Array<String> tokens = argString.Tokenize(",");
    if (tokens.Size() != 2)
    {
        return HttpStatus::BadRequest;
    }
    GraphicsEntity::Id entityId = tokens[0].AsInt();
    const StringAtom& clipName = tokens[1];
    Ptr<ModelEntity> modelEntity = this->LookupCharacterModelEntityById(entityId);
    if (!modelEntity.isvalid())
    {
        return HttpStatus::NotFound;
    }

    // check if the animation exists...
    if (!modelEntity->GetCharacter()->AnimationLibrary().GetAnimResource()->HasClip(clipName))
    {
        return HttpStatus::NotFound;
    }

    // finally... start the clip!    
    Ptr<PlayClipJob> animJob = PlayClipJob::Create();
    animJob->SetClipName(clipName);
    animJob->SetLoopCount(0);
    animJob->SetFadeInTime(200);
    animJob->SetFadeOutTime(200);
    animJob->SetEnqueueMode(AnimJobEnqueueMode::IgnoreIfSameClipActive);
    modelEntity->GetCharacterInstance()->AnimController().EnqueueAnimJob(animJob.upcast<AnimJob>());

    // finally we need to write the entity page again
    return this->WriteEntityInfo(request, entityId);
}

//------------------------------------------------------------------------------
/**
*/
HttpStatus::Code
GraphicsPageHandler::ToggleSkin(const Ptr<HttpRequest>& request, const String& argString)
{
    GraphicsServer* server = GraphicsServer::Instance();

    // parse arg string, must be "entityId,skinName"
    Array<String> tokens = argString.Tokenize(",");
    if (tokens.Size() != 2)
    {
        return HttpStatus::BadRequest;
    }
    GraphicsEntity::Id entityId = tokens[0].AsInt();
    const StringAtom& skinName = tokens[1];
    Ptr<ModelEntity> modelEntity = this->LookupCharacterModelEntityById(entityId);
    if (!modelEntity.isvalid())
    {
        return HttpStatus::NotFound;
    }

    // check if the skin exists...
    if (!modelEntity->GetCharacter()->SkinLibrary().HasSkin(skinName))
    {
        return HttpStatus::NotFound;
    }

    // finally toggle skin visibility
    modelEntity->GetCharacterInstance()->SkinSet().ToggleSkin(skinName);

    // finally we need to write the entity page again
    return this->WriteEntityInfo(request, entityId);
}

//------------------------------------------------------------------------------
/**
*/
HttpStatus::Code
GraphicsPageHandler::ApplySkinList(const Ptr<HttpRequest>& request, const String& argString)
{
    // parse arg string, must be "entityId,skinListName"
    Array<String> tokens = argString.Tokenize(",");
    if (tokens.Size() != 2)
    {
        return HttpStatus::BadRequest;
    }
    GraphicsEntity::Id entityId = tokens[0].AsInt();
    const StringAtom& skinListName = tokens[1];
    
    // lookup entity and make sure it has a character
    Ptr<ModelEntity> modelEntity = this->LookupCharacterModelEntityById(entityId);
    if (!modelEntity.isvalid())
    {
        return HttpStatus::NotFound;
    }

    // finally toggle skin visibility
    modelEntity->GetCharacterInstance()->SkinSet().ApplySkinList(skinListName);

    // finally we need to write the entity page again
    return this->WriteEntityInfo(request, entityId);
}

//------------------------------------------------------------------------------
/**
    Enable/disable the anim sequencer visualization hud.
*/
HttpStatus::Code
GraphicsPageHandler::ShowHideAnimSequencerHUD(const Ptr<HttpRequest>& request, const String& argString)
{
    GraphicsServer* server = GraphicsServer::Instance();

    // parse arg string, must be "entityId,skinListName"
    Array<String> tokens = argString.Tokenize(",");
    if (tokens.Size() != 2)
    {
        return HttpStatus::BadRequest;
    }
    GraphicsEntity::Id entityId = tokens[0].AsInt();
    bool hudEnabled = tokens[1] == "on";

    // lookup entity and make sure it has a character
    Ptr<ModelEntity> modelEntity = this->LookupCharacterModelEntityById(entityId);
    if (!modelEntity.isvalid())
    {
        return HttpStatus::NotFound;
    }
    
    // finally enable or disable the anim sequencer debug hud
    modelEntity->GetCharacterInstance()->AnimController().AnimSequencer().SetDebugHudEnabled(hudEnabled);

    // finally we need to write the entity page again
    return this->WriteEntityInfo(request, entityId);
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsPageHandler::WriteModelInstanceHierarchy(int entityId, const Ptr<Models::ModelInstance>& modelInstance, const Ptr<Http::HtmlPageWriter>& htmlWriter)
{
    htmlWriter->Element(HtmlElement::Heading3, "ModelNodeInstances:");      
    htmlWriter->AddAttr("border", "1");
    htmlWriter->AddAttr("rules", "cols");
    htmlWriter->Begin(HtmlElement::Table);      
        htmlWriter->AddAttr("bgcolor", "lightsteelblue");
        htmlWriter->Begin(HtmlElement::TableRow);
        htmlWriter->Element(HtmlElement::TableData, "ModelNodeType");
        htmlWriter->Element(HtmlElement::TableData, "Name");
        htmlWriter->End(HtmlElement::TableRow);
        
        const Array<Ptr<Models::ModelNodeInstance> > nodes = modelInstance->GetNodeInstances();
        IndexT i;
        for (i = 0; i < nodes.Size(); ++i)
        {
            htmlWriter->Begin(HtmlElement::TableRow);  

            htmlWriter->Begin(HtmlElement::TableData); 
            htmlWriter->AddAttr("href", "/graphics?modelnodeinstance=" + String::FromInt(entityId) + ";" + String::FromInt(i));
            htmlWriter->Element(HtmlElement::Anchor, nodes[i]->GetRtti()->GetName());     
            htmlWriter->End(HtmlElement::TableData);
            htmlWriter->Begin(HtmlElement::TableData); 
            htmlWriter->AddAttr("href", "/graphics?modelnodeinstance=" + String::FromInt(entityId) + ";" + String::FromInt(i));
            htmlWriter->Element(HtmlElement::Anchor, nodes[i]->GetName().Value());    
            htmlWriter->End(HtmlElement::TableData);
            htmlWriter->End(HtmlElement::TableRow);            	
        }                                          
    htmlWriter->End(HtmlElement::Table);
}

//------------------------------------------------------------------------------
/**
*/
Http::HttpStatus::Code 
GraphicsPageHandler::WriteModelNodeInstanceInfo(const Ptr<Http::HttpRequest>& request, const String& argString)
{         
    GraphicsServer* server = GraphicsServer::Instance();
    Array<String>  tokens = argString.Tokenize(";");
    n_assert(tokens.Size() == 2);
    const Ptr<GraphicsEntity>& gfxEntity = server->GetEntityById(tokens[0].AsInt());
    if (gfxEntity.isvalid())
    {   
        Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
        htmlWriter->SetStream(request->GetResponseContentStream());        
        if (htmlWriter->Open())
        {
            // get node from index
            const Ptr<ModelInstance>& instance = gfxEntity.cast<ModelEntity>()->GetModelInstance();
            const Array<Ptr<ModelNodeInstance> >& nodes = instance->GetNodeInstances();
            const Ptr<ModelNodeInstance>& node = nodes[tokens[1].AsInt()];

            htmlWriter->Element(HtmlElement::Heading1, node->GetName().AsString());
            // write node info for all types
            if (node->IsA(TransformNodeInstance::RTTI))
            {
                this->WriteTransformNodeInfo(node.cast<TransformNodeInstance>(), htmlWriter);
            }
            if (node->IsA(AnimatorNodeInstance::RTTI))
            {
                this->WriteAnimatorNodeInfo(node.cast<AnimatorNodeInstance>(), htmlWriter);
            }
            if (node->IsA(StateNodeInstance::RTTI))
            {
                this->WriteStateNodeInfo(node.cast<StateNodeInstance>(), htmlWriter);
            }  
            if (node->IsA(ShapeNodeInstance::RTTI))
            {
                this->WriteShapeNodeInfo(node.cast<ShapeNodeInstance>(), htmlWriter);
            }
            htmlWriter->Close();
            return HttpStatus::OK;            
        }
        else
        {
            return HttpStatus::InternalServerError;
        }
    }
    else
    {
        return HttpStatus::NotFound;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsPageHandler::WriteTransformNodeInfo(const Ptr<TransformNodeInstance>& transformNode, const Ptr<Http::HtmlPageWriter>& htmlWriter)
{       
    htmlWriter->Element(HtmlElement::Heading2, "TransformNode");
    htmlWriter->Begin(HtmlElement::Table);
        htmlWriter->Begin(HtmlElement::TableRow);
            htmlWriter->Element(HtmlElement::TableData, "Position: ");
            htmlWriter->Element(HtmlElement::TableData, String::FromFloat4(transformNode->GetPosition()));
        htmlWriter->End(HtmlElement::TableRow);

        htmlWriter->Begin(HtmlElement::TableRow);
        htmlWriter->Element(HtmlElement::TableData, "Rotation: ");
        htmlWriter->Element(HtmlElement::TableData, String::FromFloat4(float4(transformNode->GetRotate().x(),
                                                                              transformNode->GetRotate().y(),
                                                                              transformNode->GetRotate().z(),
                                                                              transformNode->GetRotate().w())));
        htmlWriter->End(HtmlElement::TableRow);

        htmlWriter->Begin(HtmlElement::TableRow);
        htmlWriter->Element(HtmlElement::TableData, "Scale: ");
        htmlWriter->Element(HtmlElement::TableData, String::FromFloat4(transformNode->GetScale()));
        htmlWriter->End(HtmlElement::TableRow);

        // TODO: rest of members...
    htmlWriter->End(HtmlElement::Table);
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsPageHandler::WriteAnimatorNodeInfo(const Ptr<AnimatorNodeInstance>& animNode, const Ptr<Http::HtmlPageWriter>& htmlWriter)
{
    htmlWriter->Element(HtmlElement::Heading2, "AnimatorNode:");    
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsPageHandler::WriteStateNodeInfo(const Ptr<StateNodeInstance>& stateNodeInst, const Ptr<Http::HtmlPageWriter>& htmlWriter)
{
    const Ptr<Models::StateNode>& stateNode = stateNodeInst->GetModelNode().cast<StateNode>();
    htmlWriter->Element(HtmlElement::Heading2, "StateNode:");
    htmlWriter->Begin(HtmlElement::Table);
        htmlWriter->Begin(HtmlElement::TableRow);
            htmlWriter->Element(HtmlElement::TableData, "Shader: ");
            htmlWriter->Element(HtmlElement::TableData, stateNode->GetMaterialName().AsString());
        htmlWriter->End(HtmlElement::TableRow);
    // TODO: rest of members...
    htmlWriter->End(HtmlElement::Table);
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsPageHandler::WriteShapeNodeInfo(const Ptr<ShapeNodeInstance>& shapeNode, const Ptr<Http::HtmlPageWriter>& htmlWriter)
{

}

//------------------------------------------------------------------------------
/**
*/
Http::HttpStatus::Code 
GraphicsPageHandler::ApplyVariation(const Ptr<Http::HttpRequest>& request, const Util::String& argString)
{
    // parse arg string, must be "entityId,variationName"
    Array<String> tokens = argString.Tokenize(",");
    if (tokens.Size() != 2)
    {
        return HttpStatus::BadRequest;
    }
    GraphicsEntity::Id entityId = tokens[0].AsInt();
    const StringAtom& variationName = tokens[1];

    // lookup entity and make sure it has a character
    Ptr<ModelEntity> modelEntity = this->LookupCharacterModelEntityById(entityId);
    if (!modelEntity.isvalid())
    {
        return HttpStatus::NotFound;
    }

    // finally toggle skin visibility
    modelEntity->GetCharacterInstance()->SetVariationSetName(variationName);

    // finally we need to write the entity page again
    return this->WriteEntityInfo(request, entityId);
}
} // namespace Characters