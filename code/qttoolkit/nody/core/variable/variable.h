#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::Variable
    
    Describes an input/output variable.

    A variable has a generic set of flags which can be set. However, Nody doesn't have any flags,
    but only the ability to store them. Flags should be defined in your application, and used similarly.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "variableinstance.h"
#include "vartype.h"
namespace Nody
{
class Variable : public Core::RefCounted
{
	__DeclareClass(Variable);
public:

	enum IOFlag
	{
		Input,      // input to a variation
		Output,     // output from a variation
        Hidden      // none of the above, can be used as parameters
	};

	/// constructor
	Variable();
	/// destructor
	virtual ~Variable();

	/// discard variable
	virtual void Discard();

	/// sets the IO-flag
	void SetIOFlag(const IOFlag& flag);
	/// gets the IO-flag
	const IOFlag& GetIOFlag() const;
	/// sets the data type of the variable
	void SetType(const VarType& type);
	/// returns the data type of the variable
	const VarType& GetType() const;
	/// sets name of variable
	void SetName(const Util::String& name);
	/// gets name of variable
	const Util::String& GetName() const;

    /// sets variable flags
    void SetFlags(const uint& flags);
    /// get variable flags
    const uint& GetFlags() const;

    /// returns true if this variable is of any Any-type, meaning Any, AnyVec or AnyMat
    const bool IsAllowingInference() const;
    /// returns true if variable can be initialized
    const bool IsAllowingInit() const;

	/// create instance
	Ptr<VariableInstance> CreateInstance();

	/// returns list of associated instances
	const Util::Array<Ptr<VariableInstance> > GetInstances() const;
private:
	friend class VariableInstance;

	Util::Array<Ptr<VariableInstance> > instances;
	Util::String name;
	VarType type;
    uint flags;
	IOFlag ioFlag;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<VariableInstance>> 
Variable::GetInstances() const
{
	return this->instances;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Variable::SetIOFlag(const Variable::IOFlag& flag)
{
	this->ioFlag = flag;
}

//------------------------------------------------------------------------------
/**
*/
inline const Variable::IOFlag& 
Variable::GetIOFlag() const
{
	return this->ioFlag;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Variable::SetType(const VarType& type)
{
	this->type = type;
}

//------------------------------------------------------------------------------
/**
*/
inline const VarType& 
Variable::GetType() const
{
	return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Variable::SetName(const Util::String& name)
{
	n_assert(name.IsValid());
	this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
Variable::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Variable::SetFlags( const uint& flags )
{
    this->flags = flags;
}

//------------------------------------------------------------------------------
/**
*/
inline const uint& 
Variable::GetFlags() const
{
    return this->flags;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
Variable::IsAllowingInference() const
{
    return (this->type.IsA(VarType::Any) || this->type.IsA(VarType::AnyVec) || this->type.IsA(VarType::AnyMat));
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
Variable::IsAllowingInit() const
{
    return !(this->type.GetType() >= VarType::Sampler2D && this->type.GetType() <= VarType::UnknownType);
}

} // namespace Nody
//------------------------------------------------------------------------------