#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::Project
    
    Used to contain the program state, and can be saved and loaded.
    
    (C) 2014 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "util/guid.h"
#include "util/variant.h"
#include "io/uri.h"
#include "core/singleton.h"

namespace Nody
{
class Node;
class Link;
class NodeScene;
class VariableInstance;
class Project : public Core::RefCounted
{
	__DeclareClass(Project);
    __DeclareSingleton(Project);
public:

	/// constructor
	Project();
	/// destructor
	virtual ~Project();

    /// sets path to project
    void SetPath(const IO::URI& path);
    /// get path to project
    const IO::URI& GetPath() const;

    /// clear project
    virtual void Clear(bool clearPath = false);

    /// apply project to scene, sets up nodes and links
    /// use with care since it will add all the nodes and links to the scene, even if they are already there!!!
    virtual void Apply(const Ptr<NodeScene>& scene);
    /// stores all nodes and links in the scene in this project
    virtual void Store(const Ptr<NodeScene>& scene);

	/// request a parameter name, will take the name + number and increase number if it is encountered
	static Util::String RequestParameterName(const Ptr<VariableInstance>& var, const Util::String& name);
	/// register a variable name
	static void DeregisterParameterName(const Ptr<VariableInstance>& var);

	// per project state
	struct GlobalState
	{
		Math::float2 viewCenter;
	} globalState;

protected:
    friend class ProjectLoader;
    friend class ProjectSaver;

    /// add node to project, generates an internal GUID for it
    void AddNode(const Ptr<Node>& node);
    /// add link to project, assumes both end nodes have already been added
    void AddLink(const Ptr<Link>& link);

    // per link saved data
    struct LinkState
    {
        Util::Guid from;
        Util::Guid to;
        Util::String fromName;
        Util::String toName;
    };

    // per node saved data
    struct NodeState
    {
        Math::float2 pos;
        Util::Guid id;
        bool superNode;
        Util::String variation;
        Util::Dictionary<Util::String, Util::Variant> values;
    };

    IO::URI path;
    Util::Array<NodeState> nodes;
    Util::Array<LinkState> links;

    Util::Dictionary<Ptr<Node>, NodeState> nodeStateMapping;

	static Util::Dictionary<Ptr<VariableInstance>, Util::String> ParameterNames;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
Project::SetPath(const IO::URI& path)
{
    this->path = path;
}

//------------------------------------------------------------------------------
/**
*/
inline const IO::URI& 
Project::GetPath() const
{
    return this->path;
}
} // namespace Nody
//------------------------------------------------------------------------------