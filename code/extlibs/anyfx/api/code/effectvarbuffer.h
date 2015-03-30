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

#include <string>
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
    const std::string& GetName() const;

    /// set buffer, must be an implementation specific
	void SetBuffer(void* handle);

	struct OpenGLBuffer
	{
		int handle;
		unsigned offset;
		unsigned size;
		bool bindRange;

		OpenGLBuffer() : bindRange(false), offset(0), size(0) {};

	};

	struct DirectXBuffer
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