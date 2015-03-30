#pragma once
#ifndef SCRIPTING_ARGSBLOCK_H
#define SCRIPTING_ARGSBLOCK_H
//------------------------------------------------------------------------------
/**
    @class Scripting::ArgsBlock
    
    Used to pass arguments into a script command and to pass the
    result of the command back.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "util/array.h"
#include "util/keyvaluepair.h"
#include "util/hashtable.h"
#include "scripting/arg.h"

//------------------------------------------------------------------------------
namespace Scripting
{
class ArgsBlock
{
public:
    /// constructor
    ArgsBlock();
    /// destructor
    ~ArgsBlock();
    /// return true if the argument block is empty
    bool IsEmpty() const;
    /// clear the argument block
    void Clear();
    /// add an argument to the block
    void AddArg(const Util::String& name, Arg::Type type);
    /// return number of arguments
    SizeT GetNumArgs() const;
    /// return true if argument exists by name
    bool HasArg(const Util::String& name) const;
    /// get argument name at index
    const Util::String& GetArgName(IndexT index) const;
    /// get argument value at index (read-only)
    const Arg& GetArgValue(IndexT index) const;
    /// get argument value by name (read-only)
    const Arg& GetArgValue(const Util::String& name) const;
    /// argument value at index (read/write)
    Arg& ArgValue(IndexT index);
    /// argument value by name (read/write)
    Arg& ArgValue(const Util::String& name);

private:
	Util::Array<Util::KeyValuePair<Util::String,Arg> > args;
	Util::HashTable<Util::String,int> nameTable;    
};

} // namespace Scripting
//------------------------------------------------------------------------------
#endif

    