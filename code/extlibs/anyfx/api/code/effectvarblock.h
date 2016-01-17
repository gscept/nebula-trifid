#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectVarblock
    
    The EffectVarblock couples several variables together, which is managed, updated and can be shared by several effects.
	Varblocks can be used to optimize variables updates such as View and Projection matrices.
	In other words, the EffectVarblock acts as a potentially reusable buffer.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------

#ifdef AFX_API
#error "afxapi.h included before effectvarblock.h"
#endif

#include "EASTL/string.h"
#include "annotable.h"
namespace AnyFX
{

struct VarblockVariableBinding
{
    eastl::string name;
    unsigned offset;
    unsigned size;
    unsigned arraySize;
    char* value;

    VarblockVariableBinding() : value(NULL) {};
    ~VarblockVariableBinding()
    {
        if (NULL != value) delete value;
    }
};

class AutoRef;
class InternalEffectVarblock;
class EffectVarblock : public Annotable
{
public:

	/// constructor
	EffectVarblock();
	/// destructor
	virtual ~EffectVarblock();

	/// commits varblock
	void Commit();
	/// returns name of varblock
	const eastl::string& GetName() const;
    /// returns signature of varblock
    const eastl::string& GetSignature() const;
    /// get size (in bytes) of varblock
    const size_t GetSize() const;

    /// returns true if variable has any use whatsoever in the underlying structure
    const bool IsActive() const;

    /// return list of variable binding information within this varblock
    eastl::vector<VarblockVariableBinding> GetVariables() const;
	/// get buffer handle
	void* GetHandle() const;

    /// set buffer, must be an implementation specific
	void SetBuffer(void* handle);

	/// discard varblock
	void Discard();

private:
    friend class EffectStreamLoader;
	friend class EffectVarblockStreamLoader;
	friend class InternalEffectProgram;

	InternalEffectVarblock* internalVarblock;
}; 


} // namespace AnyFX
//------------------------------------------------------------------------------