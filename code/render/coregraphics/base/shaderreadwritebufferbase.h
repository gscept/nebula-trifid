#pragma once
//------------------------------------------------------------------------------
/**
	@class Base::ShaderReadWriteBufferBase
	
	A shader buffer represents a read/write buffer assignable inside a shader.
	
	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
namespace Base
{
class ShaderReadWriteBufferBase : public Core::RefCounted
{
	__DeclareClass(ShaderReadWriteBufferBase);
public:
	/// constructor
	ShaderReadWriteBufferBase();
	/// destructor
	virtual ~ShaderReadWriteBufferBase();

	/// setup buffer
	void Setup();
	/// discard buffer
	void Discard();

	/// set the size of the buffer
    void SetSize(const uint size);
	/// get the size of the buffer
    const uint GetSize() const;

	/// returns buffer handle
	void* GetHandle() const;

	/// update buffer
    void Update(void* data, uint offset, uint size);
    /// cycle to next buffer
    void CycleBuffers();

    static const int NumBuffers = 3;

protected:
    bool isSetup;
    uint size;
    IndexT bufferIndex;
};

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderReadWriteBufferBase::SetSize(const uint size)
{
	n_assert(!this->isSetup);
	this->size = size;
}

//------------------------------------------------------------------------------
/**
*/
inline const uint
ShaderReadWriteBufferBase::GetSize() const
{
	return this->size;
}

//------------------------------------------------------------------------------
/**
*/
inline void*
ShaderReadWriteBufferBase::GetHandle() const
{
	return 0;
}

} // namespace Base