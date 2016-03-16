//------------------------------------------------------------------------------
//  shadyproject.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadyproject.h"
#include "scene/nodescene.h"
#include "node/shadynode.h"
#include "variation/variationdatabase.h"
#include "scene/graphics/nodegraphicssceneview.h"

using namespace Nody;
namespace Shady
{
__ImplementClass(Shady::ShadyProject, 'SPRO', Nody::Project);
__ImplementSingleton(Shady::ShadyProject);

//------------------------------------------------------------------------------
/**
*/
ShadyProject::ShadyProject()
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ShadyProject::~ShadyProject()
{
	__DestructSingleton
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyProject::Apply(const Ptr<NodeScene>& scene)
{
    n_assert(scene.isvalid());

    // clear scene
    scene->Clear();

    // make temporary dictionary of nodes
    Util::Dictionary<Util::Guid, Ptr<ShadyNode>> nodeMap;

    // setup nodes
    IndexT i;
    for (i = 0; i < this->nodes.Size(); i++)
    {
        Ptr<ShadyNode> node = ShadyNode::Create();;
        const NodeState& state = this->nodes[i];
        nodeMap.Add(state.id, node);

        // setup node
        if (state.superNode)
        {
            const Ptr<SuperVariation>& var = VariationDatabase::Instance()->GetSuperVariationByName(state.variation);
            node->Setup(var);
        }
        else
        {
            const Ptr<Variation>& var = VariationDatabase::Instance()->GetVariationByName(state.variation);
            node->Setup(var);
        }

		// add node to scene
		scene->AddNode(node.upcast<Nody::Node>());

        // setup node values
        IndexT j;
        for (j = 0; j < state.values.Size(); j++)
        {
            node->SetValue(state.values.KeyAtIndex(j), state.values.ValueAtIndex(j));
        }

        // set position now when we have a graphical representation
        node->GetGraphics()->SetPosition(state.pos);
    }

    // setup links
    for (i = 0; i < this->links.Size(); i++)
    {
        const LinkState& state = this->links[i];

        // get nodes
        const Ptr<ShadyNode>& from = nodeMap[state.from];
        const Ptr<ShadyNode>& to = nodeMap[state.to];

        const Ptr<VariableInstance>& var1 = from->GetVariation()->GetOutput(state.fromName);
        Ptr<VariableInstance> var2;
        if (to->IsSuperNode())  var2 = to->GetSuperVariation()->GetInput(state.toName);
        else                    var2 = to->GetVariation()->GetInput(state.toName);

        // create link in scene, this is slightly ugly since the previous link object is really not necessary...
        scene->CreateLink(var1, var2);
    }

	NodeGraphicsSceneView* view = static_cast<NodeGraphicsSceneView*>(scene->GetNodeSceneGraphics()->views()[0]);
	QPointF center;
	center.setX(this->globalState.viewCenter.x());
	center.setY(this->globalState.viewCenter.y());
	view->SetOrigin(center);

    // clear project when we have applied, just to save memory
    this->Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyProject::Store(const Ptr<NodeScene>& scene)
{
    n_assert(scene.isvalid());

    // clear project first
    this->Clear();

    // get nodes and save to project
    const Util::Array<Ptr<Node>>& nodes = scene->GetNodes();
    IndexT i;
    for (i = 0; i < nodes.Size(); i++)
    {
        const Ptr<Node>& node = nodes[i];   
        this->AddNode(node);
    }

    const Util::Array<Ptr<Link>>& links = scene->GetLinks();
    for (i = 0; i < links.Size(); i++)
    {
        const Ptr<Link>& link = links[i];
        this->AddLink(link);
    }
}

} // namespace Shady