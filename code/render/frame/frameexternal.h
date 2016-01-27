#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FrameExternal
    
    Executes another frame shader from within a current one. 
	Use with caution since calling itself may result in an infinite loop.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "framepassbase.h"
#include "frameshader.h"
namespace Frame
{
class FrameExternal : public Frame::FramePassBase
{
	__DeclareClass(FrameExternal);
public:
	/// constructor
	FrameExternal();
	/// destructor
	virtual ~FrameExternal();

	/// discard the frame pass
	void Discard();
	/// render the pass
    void Render(IndexT frameIndex);

	/// sets the frame shader name
	void SetFrameShaderName(const Resources::ResourceId& name);
	/// gets the frame shader name
	const Resources::ResourceId& GetFrameShaderName() const;

private:
	Resources::ResourceId frameShaderName;
	Ptr<Frame::FrameShader> frameShader;
}; 


//------------------------------------------------------------------------------
/**
*/
inline void 
FrameExternal::SetFrameShaderName( const Resources::ResourceId& name )
{
	this->frameShaderName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId& 
FrameExternal::GetFrameShaderName() const
{
	return this->frameShaderName;
}

} // namespace Frame
//------------------------------------------------------------------------------