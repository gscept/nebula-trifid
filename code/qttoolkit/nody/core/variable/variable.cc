//------------------------------------------------------------------------------
//  variable.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "variable.h"
#include "variableinstance.h"

namespace Nody
{
__ImplementClass(Nody::Variable, 'VARI', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Variable::Variable() :
    flags(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Variable::~Variable()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
Variable::Discard()
{
	this->instances.Clear();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<VariableInstance> 
Variable::CreateInstance()
{
	Ptr<VariableInstance> instance = VariableInstance::Create();
	Ptr<Variable> thisPtr(this);
	instance->Setup(thisPtr);
	this->instances.Append(instance);
	return instance;
}


} // namespace Nody