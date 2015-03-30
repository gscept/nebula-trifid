#pragma once
//------------------------------------------------------------------------------
/**
    @class Shady::ShadyNode
    
    Implements the shady-equivalent of a node.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "node/node.h"
#include "variable/vartype.h"
#include "util/variant.h"
namespace Shady
{
class ShadyNode : public Nody::Node
{
	__DeclareClass(ShadyNode);
public:
	/// constructor
	ShadyNode();
	/// destructor
	virtual ~ShadyNode();

    /// sets up the node from a variation
    virtual void Setup(const Ptr<Nody::Variation>& variation);
    /// sets up a node from a super variation
    virtual void Setup(const Ptr<Nody::SuperVariation>& superVariation);
    /// discards the node 
    virtual void Discard();

	/// generate graphics
	void GenerateGraphics();
	/// destroy graphics
	void DestroyGraphics();

    /// resolve the type which needs to be used by this node
    Nody::VarType ResolveWorkingType();

private:
    friend class ShadyProjectLoader;
    friend class ShadyProjectSaver;

    /// sets default values for the various variable types
    void SetupDefaultValue(const Nody::VarType& type, Util::Variant& value);

    static uint GlobalParamCount;
}; 
} // namespace Shady
//------------------------------------------------------------------------------