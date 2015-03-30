#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::Link
    
    Represents the link between two variables, also explains how variables are connected with eachother
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "graphics/linkgraphics.h"
#include "variable/variableinstance.h"

namespace Nody
{
class VariationInstance;
class LinkGraphics;
class Link : public Core::RefCounted
{
	__DeclareClass(Link);
public:
	/// constructor
	Link();
	/// destructor
	virtual ~Link();

	/// discard link
	void Discard();

	/// links variables
	bool LinkVariables(const Ptr<VariableInstance>& from, const Ptr<VariableInstance>& to);

	/// get from-variable
	const Ptr<VariableInstance>& GetFromVariable() const;
	/// get to-variable
	const Ptr<VariableInstance>& GetToVariable() const;

	/// return pointer to graphics
	const Ptr<LinkGraphics>& GetGraphics() const;
	/// generate graphics
	virtual void GenerateGraphics();
	/// destroy graphics
	virtual void DestroyGraphics();

private:
	Ptr<LinkGraphics> graphics;
	Ptr<VariableInstance> from;
	Ptr<VariableInstance> to;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const Ptr<LinkGraphics>& 
Link::GetGraphics() const
{
	return this->graphics;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<VariableInstance>& 
Link::GetFromVariable() const
{
	return this->from;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<VariableInstance>& 
Link::GetToVariable() const
{
	return this->to;
}

} // namespace Nody
//------------------------------------------------------------------------------