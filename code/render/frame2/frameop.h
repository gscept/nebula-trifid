#pragma once
//------------------------------------------------------------------------------
/**
	A frame op is a base class for frame operations, use as base class for runnable
	sequences within a frame script.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
namespace Frame2
{
class FrameOp : public Core::RefCounted
{
	__DeclareClass(FrameOp);
public:
	/// constructor
	FrameOp();
	/// destructor
	virtual ~FrameOp();

	/// set name
	void SetName(const Util::String& name);

	/// run operation
	void Run(const IndexT frameIndex);
	/// handle display resizing
	void DisplayResized(const SizeT width, const SizeT height);
protected:
	Util::String name;
};

//------------------------------------------------------------------------------
/**
*/
inline void
FrameOp::SetName(const Util::String& name)
{
	this->name = name;
}

} // namespace Frame2