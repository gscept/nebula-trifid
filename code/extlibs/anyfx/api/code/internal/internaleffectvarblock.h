#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::InternalEffectVarblock
    
    EffectVarblock backend, inherit this class to provide an implementation
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <string.h>
#include "EASTL/vector.h"
#include "EASTL/string.h"
#include "EASTL/map.h"
#include "autoref.h"
#include "settings.h"
namespace AnyFX
{
class Effect;
class InternalEffectVariable;
class InternalEffectProgram;
class InternalEffectVarblock : public AutoRef
{
public:
	/// constructor
	InternalEffectVarblock();
	/// destructor
	virtual ~InternalEffectVarblock();

	bool isDirty;

private:
	/// creates signature which is used for shared varblocks
	void SetupSignature();
	/// returns name of varblock
	const eastl::string& GetName() const;
	/// returns signature
    const eastl::string& GetSignature() const;
    /// returns size
    const size_t GetSize() const;
protected:
	friend class EffectVarblock;
    friend class EffectVariable;
	friend class EffectStreamLoader;
	friend class InternalEffectVariable;
	friend class InternalEffectProgram;
	friend class EffectVarblockStreamLoader;	

	/// sets up varblock from program, override in subclass
	virtual void Setup(eastl::vector<InternalEffectProgram*> programs);
	/// sets up varblock from programs using a pre-existing varblock
	virtual void SetupSlave(eastl::vector<InternalEffectProgram*> programs, InternalEffectVarblock* master);

    /// sets up default variables, must be called from the implementation since different implementations may change the variable offsets
    virtual void SetupDefaultValues();

    /// set buffer 
    virtual void SetBuffer(void* handle);

	/// binds varblocks prior to updating
	virtual void Apply();
	/// updates varblocks back-end buffer
	virtual void Commit();

	/// activates variable, this makes the uniform location be the one found in the given program
	virtual void Activate(InternalEffectProgram* program);

    eastl::string name;
    eastl::string signature;
    size_t size;
	eastl::vector<InternalEffectVariable*> variables;
	eastl::map<eastl::string, InternalEffectVariable*> variablesByName;
	eastl::vector<InternalEffectVarblock*> childBlocks;
	InternalEffectVarblock* masterBlock;
	bool isShared;
	bool isSlave;
	bool active;
    bool manualFlushing;
    unsigned numBackingBuffers;
	static unsigned globalVarblockCounter;

	void** bufferHandle;

}; 

//------------------------------------------------------------------------------
/**
*/
inline const eastl::string&
InternalEffectVarblock::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const size_t
InternalEffectVarblock::GetSize() const
{
    return this->masterBlock->size;
}

//------------------------------------------------------------------------------
/**
*/
inline const eastl::string&
InternalEffectVarblock::GetSignature() const
{
	return this->signature;
}

//------------------------------------------------------------------------------
/**
*/
inline void
InternalEffectVarblock::SetBuffer(void* handle)
{
	*this->bufferHandle = handle;
	this->masterBlock->isDirty = true;
}

} // namespace AnyFX
//------------------------------------------------------------------------------