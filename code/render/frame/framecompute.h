#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FrameCompute
    
    A frame compute performs a compute shader calculation inline with other frame based effects.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "frame/framepassbase.h"
#include "renderutil/drawfullscreenquad.h"

//------------------------------------------------------------------------------
namespace Frame
{
class FrameCompute : public FramePassBase
{
    __DeclareClass(FrameCompute);
public:

    /// constructor
    FrameCompute();
    /// destructor
    virtual ~FrameCompute();
    /// setup the post effect
    void Setup();
    /// render the post effect
	virtual void Render(IndexT frameIndex);

    /// set the relative width of the computation
    void SetRelativeWidth(float f);
    /// set the relative height of the computation
    void SetRelativeHeight(float f);

    /// set compute sizes
    void SetComputeSize(uint x, uint y, uint z);
    /// set group sizes
    void SetGroupSize(uint x, uint y, uint z);

    /// handle display resizing
	void OnWindowResize(SizeT width, SizeT height);

private:
    bool displayRelative;
    float relativeWidth;
    float relativeHeight;
    uint computeSizes[3];
    uint groupSizes[3];
};

//------------------------------------------------------------------------------
/**
*/
inline void 
FrameCompute::SetComputeSize( uint x, uint y, uint z )
{
    this->computeSizes[0] = x;
    this->computeSizes[1] = y;
    this->computeSizes[2] = z;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
FrameCompute::SetGroupSize( uint x, uint y, uint z )
{
    this->groupSizes[0] = x;
    this->groupSizes[1] = y;
    this->groupSizes[2] = z;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
FrameCompute::SetRelativeWidth( float f )
{
    this->displayRelative = true;
    this->relativeWidth = f;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
FrameCompute::SetRelativeHeight( float f )
{
    this->displayRelative = true;
    this->relativeHeight = f;
}

} // namespace Frame
//------------------------------------------------------------------------------

     