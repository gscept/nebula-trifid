//------------------------------------------------------------------------------
//  project.cc
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "project.h"
#include "node/node.h"
#include "link/link.h"

namespace Nody
{
__ImplementClass(Nody::Project, 'PROJ', Core::RefCounted);
__ImplementSingleton(Nody::Project);

Util::Dictionary<Ptr<VariableInstance>, Util::String> Project::ParameterNames;
//------------------------------------------------------------------------------
/**
*/
Project::Project()
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
Project::~Project()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void 
Project::Clear(bool clearPath)
{
    if (clearPath) this->path.Clear();
    this->nodes.Clear();
    this->links.Clear();
    this->nodeStateMapping.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
Project::AddNode(const Ptr<Node>& node)
{
    n_assert(node.isvalid());
    NodeState state; 
    state.pos = node->GetGraphics()->GetPosition();
    state.id.Generate();
    state.superNode = node->IsSuperNode();
    if (state.superNode)   state.variation = node->GetSuperVariation()->GetOriginalSuperVariation()->GetName();
    else                   state.variation = node->GetVariation()->GetOriginalVariation()->GetName();

    const Util::Dictionary<Util::String, Util::Variant>& values = node->GetValueDictionary();
    IndexT i;
    for (i = 0; i < values.Size(); i++)
    {
        state.values.Add(values.KeyAtIndex(i), values.ValueAtIndex(i));
    }

    // todo, add values stored in node
    //

    this->nodes.Append(state);
    this->nodeStateMapping.Add(node, state);
}
//------------------------------------------------------------------------------
/**
*/
void
Project::AddLink(const Ptr<Link>& link)
{
    n_assert(link.isvalid());

    // get nodes
    const Ptr<Node>& from = link->GetFromVariable()->GetNode();
    const Ptr<Node>& to = link->GetToVariable()->GetNode();

    // assume they have already been added to the project
    n_assert(this->nodeStateMapping.Contains(from));
    n_assert(this->nodeStateMapping.Contains(to));

    LinkState state;
    state.from = this->nodeStateMapping[from].id;
    state.to = this->nodeStateMapping[to].id;
    state.fromName = link->GetFromVariable()->GetOriginalVariable()->GetName();
    state.toName = link->GetToVariable()->GetOriginalVariable()->GetName();

    // add link
    this->links.Append(state);
}

//------------------------------------------------------------------------------
/**
*/
void
Project::Apply(const Ptr<NodeScene>& scene)
{
    // empty, apply contents of project to scene depending on which application we want to use
}

//------------------------------------------------------------------------------
/**
*/
void
Project::Store(const Ptr<NodeScene>& scene)
{
    // empty, implement per application to store the data from the scene into the project
}

//------------------------------------------------------------------------------
/**
*/
Util::String
Project::RequestParameterName(const Ptr<VariableInstance>& var, const Util::String& name)
{
	Util::String newName = name;
	IndexT counter = 0;
	const Util::Array<Util::String>& reservedNames = Project::ParameterNames.ValuesAsArray();
	while (reservedNames.FindIndex(newName) != InvalidIndex)
	{
		newName = Util::String::Sprintf("%s%d", name.AsCharPtr(), counter++);
	}
	
	// add to 
	Project::ParameterNames.Add(var, newName);
	return newName;
}

//------------------------------------------------------------------------------
/**
*/
void
Project::DeregisterParameterName(const Ptr<VariableInstance>& var)
{
	if (Project::ParameterNames.Contains(var))	Project::ParameterNames.Erase(var);
}

} // namespace Nody