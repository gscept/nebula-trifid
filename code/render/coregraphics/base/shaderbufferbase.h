#pragma once
//------------------------------------------------------------------------------
/**
	@class Base::ShaderBufferBase
	
	A shader buffer represents a read/write buffer assignable inside a shader.
	
	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
namespace Base
{
class ShaderBufferBase : public Core::RefCounted
{
	__DeclareClass(ShaderBufferBase);
public:
	/// constructor
	ShaderBufferBase();
	/// destructor
	virtual ~ShaderBufferBase();

	/// setup buffer
	void Setup();
	/// discard buffer
	void Discard();

	/// set the size of the buffer
	void SetSize(const SizeT size);
	/// get the size of the buffer
	const SizeT GetSize() const;

	/// returns buffer handle
	void* GetHandle() const;

	/// update buffer
	void UpdateBuffer(void* data, SizeT offset, SizeT length);

protected:
	SizeT size;
	bool isSetup;
};


//------------------------------------------------------------------------------
/**
*/
inline void
ShaderBufferBase::SetSize(const SizeT size)
{
	n_assert(!this->isSetup);
	this->size = size;
}

//------------------------------------------------------------------------------
/**
*/
inline const SizeT
ShaderBufferBase::GetSize() const
{
	return this->size;
}

//------------------------------------------------------------------------------
/**
*/
inline void*
ShaderBufferBase::GetHandle() const
{
	return 0;
}

} // namespace Base