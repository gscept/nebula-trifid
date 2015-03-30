#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::InternalEffectVarbuffer
    
    Internal representation of variable buffer. Base class for all implementations of variable buffers.
    
    (C) 2014 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "EASTL/vector.h"
#include "autoref.h"
#include <string>
namespace AnyFX
{
class InternalEffectVariable;
class InternalEffectProgram;
class InternalEffectVarbuffer : public AutoRef
{
public:
	/// constructor
	InternalEffectVarbuffer();
	/// destructor
	virtual ~InternalEffectVarbuffer();
    
    /// returns name of varbuffer
    const std::string& GetName() const;

protected:
	friend class EffectVarbuffer;
	friend class EffectStreamLoader;
	friend class InternalEffectProgram;
    friend class EffectVarbufferStreamLoader;	

	/// sets up varblock from program, override in subclass
	virtual void Setup(eastl::vector<InternalEffectProgram*> programs);
	/// sets up varblock from programs using a pre-existing varblock
	virtual void SetupSlave(eastl::vector<InternalEffectProgram*> programs, InternalEffectVarbuffer* master);

    /// applies varbuffer into the current context
    virtual void Apply();
    /// commits changes made to the varbuffer
    virtual void Commit();

	/// set buffer 
	virtual void SetBuffer(void* handle);


    /// activates variable, this makes the uniform location be the one found in the given program
    virtual void Activate(InternalEffectProgram* program);

	static unsigned globalVarbufferCounter;
	eastl::vector<InternalEffectVariable*> variables;
    InternalEffectVarbuffer* masterBuffer;
    std::string name;
	unsigned size;

	void* currentBufferHandle;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
InternalEffectVarbuffer::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline void
InternalEffectVarbuffer::SetBuffer(void* handle)
{
	this->currentBufferHandle = handle;
}

} // namespace AnyFX
//------------------------------------------------------------------------------