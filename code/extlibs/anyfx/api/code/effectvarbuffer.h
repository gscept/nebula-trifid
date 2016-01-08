#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectVarbuffer
    
    A variable buffer is a buffer of variables which can be variable in size.
    A varbuffer can also be written/read to from a shader.
    
    (C) 2014 Gustav Sterbrant
*/
//------------------------------------------------------------------------------

#ifdef AFX_API
#error "afxapi.h included before effectvarbuffer.h"
#endif

#include "EASTL/string.h"
#include "annotable.h"
namespace AnyFX
{
class InternalEffectVarbuffer;
class EffectVarbuffer : public Annotable
{
public:
	/// constructor
	EffectVarbuffer();
	/// destructor
	virtual ~EffectVarbuffer();

    /// discard varblock
    void Discard();

    /// commits varblock
    void Commit();
    /// returns name of varblock
    const eastl::string& GetName() const;

	/// returns true if variable has any use whatsoever in the underlying structure
	const bool IsActive() const;

    /// set buffer, must be an implementation specific
	void SetBuffer(void* handle);

	struct OpenGLBufferBinding
	{
		int handle;
		unsigned offset;
		unsigned size;
		bool bindRange;

		OpenGLBufferBinding() : bindRange(false), offset(0), size(0) {};
	};

	struct DirectXBufferBinding
	{
		void* handle;
	};

private:
    friend class EffectVarbufferStreamLoader;
	friend class InternalEffectProgram;

    InternalEffectVarbuffer* internalVarbuffer;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------