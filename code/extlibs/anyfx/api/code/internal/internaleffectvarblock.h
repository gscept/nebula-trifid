#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::InternalEffectVarblock
    
    EffectVarblock backend, inherit this class to provide an implementation
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <string.h>
#include <string>
#include "EASTL/vector.h"
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
	const std::string& GetName() const;
	/// returns signature
	const std::string& GetSignature() const;

protected:
	friend class EffectVarblock;
	friend class EffectStreamLoader;
	friend class InternalEffectVariable;
	friend class InternalEffectProgram;
	friend class EffectVarblockStreamLoader;	

	/// sets up varblock from program, override in subclass
	virtual void Setup(eastl::vector<InternalEffectProgram*> programs);
	/// sets up varblock from programs using a pre-existing varblock
	virtual void SetupSlave(eastl::vector<InternalEffectProgram*> programs, InternalEffectVarblock* master);

	/// binds varblocks prior to updating
	virtual void Apply();
	/// updates varblocks back-end buffer
	virtual void Commit();
    /// performs synchronization so as to not overwrite data
    virtual void PreDraw();
    /// puts a sync barrier post drawing
    virtual void PostDraw();

	/// sets if buffer should flush manually
	virtual void SetFlushManually(bool b);
	/// flushes buffer
	virtual void FlushBuffer();
	
	/// updates single variable
	virtual void SetVariable(InternalEffectVariable* var, void* value);
	/// updates variable array
	virtual void SetVariableArray(InternalEffectVariable* var, void* value, size_t size);
    /// set variable in array at index
    virtual void SetVariableIndexed(InternalEffectVariable* var, void* value, unsigned i);
	/// activates variable, this makes the uniform location be the one found in the given program
	virtual void Activate(InternalEffectProgram* program);

	std::string name;
	std::string signature;
	eastl::vector<InternalEffectVariable*> variables;
	eastl::vector<InternalEffectVarblock*> childBlocks;
	InternalEffectVarblock* masterBlock;
	bool isShared;
	bool noSync;
	bool isSlave;
	bool active;
    bool manualFlushing;
    unsigned numBackingBuffers;
	int refCount;
	static unsigned globalVarblockCounter;

	struct InternalVarblockData
	{
		char* data;
		unsigned size;
	}* dataBlock;

	/// returns a pointer to the internal varblock data storage
	virtual InternalVarblockData* GetData();
}; 

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
InternalEffectVarblock::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
InternalEffectVarblock::GetSignature() const
{
	return this->signature;
}

//------------------------------------------------------------------------------
/**
*/
inline InternalEffectVarblock::InternalVarblockData* 
InternalEffectVarblock::GetData()
{
	return this->dataBlock;
}


} // namespace AnyFX
//------------------------------------------------------------------------------