//------------------------------------------------------------------------------
//  argsblock.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scripting/argsblock.h"

namespace Scripting
{
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ArgsBlock::ArgsBlock()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ArgsBlock::~ArgsBlock()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ArgsBlock::IsEmpty() const
{
    return this->args.IsEmpty();
}

//------------------------------------------------------------------------------
/**
*/
void
ArgsBlock::Clear()
{
	this->nameTable.Clear();
    return this->args.Clear();
}

//------------------------------------------------------------------------------
/**
    Add argument to the args block. This method is usually called
    in the OnRegister() method of a script command object to setup
    the arguments and results block of the command.
*/
void
ArgsBlock::AddArg(const String& name, Arg::Type type)
{
    n_assert(!this->nameTable.Contains(name));
    Arg arg;
    arg.SetType(type);
	int idx = this->args.Size();
	this->args.Append(KeyValuePair<Util::String,Arg>(name,arg));
	this->nameTable.Add(name,idx);    
}

//------------------------------------------------------------------------------
/**
*/
SizeT
ArgsBlock::GetNumArgs() const
{
    return this->args.Size();
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
ArgsBlock::GetArgName(IndexT index) const
{
    return this->args[index].Key();
}

//------------------------------------------------------------------------------
/**
*/
bool
ArgsBlock::HasArg(const String& name) const
{
    return this->nameTable.Contains(name);
}

//------------------------------------------------------------------------------
/**
*/
const Arg&
ArgsBlock::GetArgValue(IndexT index) const
{
    return this->args[index].Value();
}

//------------------------------------------------------------------------------
/**
*/
const Arg&
ArgsBlock::GetArgValue(const Util::String& name) const
{
    return this->args[this->nameTable[name]].Value();
}

//------------------------------------------------------------------------------
/**
*/
Arg&
ArgsBlock::ArgValue(IndexT index)
{
    return this->args[index].Value();
}

//------------------------------------------------------------------------------
/**
*/
Arg&
ArgsBlock::ArgValue(const Util::String& name)
{
    return this->args[this->nameTable[name]].Value();
}

} // namespace Scripting
