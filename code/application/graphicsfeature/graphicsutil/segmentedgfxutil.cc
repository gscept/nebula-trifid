//------------------------------------------------------------------------------
//  util/segmentedgfxutil.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphicsutil/segmentedgfxutil.h"
#include "graphics/graphicsserver.h"
#include "graphics/modelentity.h"
#include "graphics/stage.h"
#include "resources/resourceid.h"
#include "models/nodes/transformnode.h"
#include "core/factory.h"
#include "models/managedmodel.h"
#include "models/model.h"
#include "models/modelserver.h"

namespace GraphicsFeature
{
using namespace Models;
using namespace Graphics;
//------------------------------------------------------------------------------
/**
*/
SegmentedGfxUtil::SegmentedGfxUtil()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Graphics::ModelEntity> >
SegmentedGfxUtil::CreateAndSetupGraphicsEntities(const Util::String& resName, const Math::matrix44& worldMatrix, IndexT pickingId, const Ptr<Graphics::Stage> stage, bool instanced, bool castShadows)
{
    Util::Array<Ptr<Graphics::ModelEntity> > graphicsEntities;


    // load whole model
    //Ptr<Models::ManagedModel> model = ModelServer::Instance()->LoadManagedModel(ResourceId(resName));    

    //  TODO:
    // collect segment hierarchy nodes from resource    
    //if (model->GetModel()->HasNode("model"))
    //{
    //    Util::Array<Ptr<Models::ModelNode> > modelNodes = model->GetModel()->GetNodes();
    //    Util::Array<Ptr<Models::ModelNode> > segments;                
    //    for (int i = 0; i < modelNodes.Size(); i++)
    //    {
    //        if (modelNodes[i]->HHasHints(nSceneNode::LevelSegment)))
    //        {
    //            segments.Append(curNode);
    //        }
    //    }

    //    // check if segments actually exist
    //    if (segments.Size() > 0)
    //    {
    //        nTransformNode* shadowNode = (nTransformNode*) resource->GetNode()->Find("shadow");
    //    
    //        // create one graphics entity for each segment
    //        int num = segments.Size();
    //        int i;
    //        for (i = 0; i < num; i++)
    //        {
    //            // build a resource name for the segment
    //            Util::String segResName = resName;
    //            segResName.TrimRight("/");
    //            segResName.Append("/model/");
    //            segResName.Append(segments[i]->GetName());
    //            
    //            // create and setup graphics entity
				//Ptr<Graphics::ModelEntity> ge = Graphics::ModelEntity::Create();
    //            ge->SetResourceId(ResourceId(segResName));
    //            ge->SetTransform(worldMatrix);

    //            // see if a shadow node of the same name exists
    //            if (shadowNode && shadowNode->Find(segments[i]->GetName()))
    //            {
    //                Util::String shdResName = resName;
    //                shdResName.TrimRight("/");
    //                shdResName.Append("/shadow/");
    //                shdResName.Append(segments[i]->GetName());
    //                ge->SetShadowResourceName(shdResName);
    //            }
    //            if (stage.isvalid())
    //            {
    //                stage->AttachEntity(ge.upcast<Graphics::GraphicsEntity>());
    //            }
    //            graphicsEntities.Append(ge.upcast<Graphics::GraphicsEntity>());
    //        }            
    //        return graphicsEntities;
    //    }
    //}
    Util::String fullResourceName("mdl:");
    fullResourceName.Append(resName);
    fullResourceName.Append(".n3");

    // fallthrough: don't create segments
	Ptr<Graphics::ModelEntity> ge = Graphics::ModelEntity::Create();
	ge->SetInstanced(instanced);
    ge->SetResourceId(Resources::ResourceId(fullResourceName));
    ge->SetTransform(worldMatrix);
	ge->SetPickingId(pickingId);
	ge->SetCastsShadows(castShadows);
    if (stage.isvalid())
    {
        stage->AttachEntity(ge.upcast<Graphics::GraphicsEntity>());
    }
    graphicsEntities.Append(ge);
    
    return graphicsEntities;
}

}; // namespace GraphicsFeature
