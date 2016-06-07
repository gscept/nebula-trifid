//------------------------------------------------------------------------------
//  framecompute.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame/framecompute.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shadersemantics.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/displaydevice.h"

namespace Frame
{
__ImplementClass(Frame::FrameCompute, 'FCOM', Frame::FramePassBase);

using namespace Util;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
FrameCompute::FrameCompute() :
   displayRelative(false) 
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FrameCompute::~FrameCompute()
{     
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
FrameCompute::Setup()
{
    n_assert(this->computeSizes[0] != 0);
    n_assert(this->computeSizes[1] != 0);
    n_assert(this->computeSizes[2] != 0);

    // setup computation sizes if this is a screen-relative computation
    if (this->displayRelative)
    {
        DisplayDevice* dispDevice = DisplayDevice::Instance();
		const CoreGraphics::DisplayMode& mode = dispDevice->GetCurrentWindow()->GetDisplayMode();
        this->computeSizes[0] = int(mode.GetWidth() * this->relativeWidth);
        this->computeSizes[1] = int(mode.GetHeight() * this->relativeHeight);
        this->computeSizes[2] = 1; // set Z to be 1 since we are working in 2D
    }
}

//------------------------------------------------------------------------------
/**
*/
void
FrameCompute::Render(IndexT frameIndex)
{
    RenderDevice* renderDevice = RenderDevice::Instance();
	ShaderServer* shaderServer = ShaderServer::Instance();

    // apply and commit shader
    shaderServer->SetActiveShader(this->shader);
    this->shader->Apply();

    // begin updating shader
    this->shader->BeginUpdate();

    // apply shader variables
    IndexT varIndex;
    for (varIndex = 0; varIndex < this->shaderVariables.Size(); varIndex++)
    {
        this->shaderVariables[varIndex]->Apply();
    }
    this->shader->EndUpdate();

    // commit shader variables
    this->shader->Commit();

#define DivAndRoundUp(a, b) (a % b != 0) ? (a / b + 1) : (a / b)

	// run
	renderDevice->Compute(DivAndRoundUp(this->computeSizes[0], this->groupSizes[0]),
						  DivAndRoundUp(this->computeSizes[1], this->groupSizes[1]),
						  DivAndRoundUp(this->computeSizes[2], this->groupSizes[2]));

}

//------------------------------------------------------------------------------
/**
*/
void 
FrameCompute::OnWindowResize(SizeT width, SizeT height)
{
    // setup computation sizes if this is a screen-relative computation
    if (this->displayRelative)
    {
        DisplayDevice* dispDevice = DisplayDevice::Instance();

        this->computeSizes[0] = int(width * this->relativeWidth);
        this->computeSizes[1] = int(height * this->relativeHeight);
        this->computeSizes[2] = 1; // set Z to be 1 since we are working in 2D
    }
}

} // namespace Frame
