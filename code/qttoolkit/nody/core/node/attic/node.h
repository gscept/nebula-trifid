#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::Node
    
    A Node is a template class for nodes.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
namespace Nody
{
class NodeInstance;
class Variation;
class Node : public Core::RefCounted
{
	__DeclareClass(Node);
public:
	/// constructor
	Node();
	/// destructor
	virtual ~Node();

	/// creates a node instance
	virtual Ptr<NodeInstance> CreateInstance();

	/// return variation pointer
	const Ptr<Variation>& GetVariation() const;

	/// sets the icon resource name
	void SetIcon(const Util::String& icon);
	/// gets the icon resource name
	const Util::String& GetIcon() const;
	/// sets the node name
	void SetName(const Util::String& name);
	/// gets the node name
	const Util::String& GetName() const;
protected:
	Ptr<Variation> variation;
	Util::String defaultVariation;
	Util::String icon;
	Util::String name;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
Node::SetIcon( const Util::String& icon )
{
	n_assert(icon.IsValid());
	this->icon = icon;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
Node::GetIcon() const
{
	return this->icon;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Node::SetName( const Util::String& name )
{
	n_assert(name.IsValid());
	this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
Node::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Variation>& 
Node::GetVariation() const
{

}
} // namespace Nody
//------------------------------------------------------------------------------