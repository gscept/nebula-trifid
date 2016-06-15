#pragma once
//------------------------------------------------------------------------------
/**
	@class VR::VrView

	Subclass of Graphics::View, used to render a shader into separate render targets, then produce them on a HMD screen

	(C) 2016 Individual contributors, see AUTHORS file
*/
#include "graphics/view.h"

using namespace Graphics;
namespace VR
{
class VrView : public Graphics::View
{
	__DeclareClass(VrView);
public:
    enum CameraEnum
    {
        DefaultCamera = 0,
        LeftCamera = 1,
        RightCamera = 2,
    };
	/// constructor
	VrView();
	/// destructor
	virtual ~VrView();

	/// handle setting the frame shader
	virtual void SetFrameShader(const Ptr<Frame::FrameShader>& frameShader);
	/// render the view into its render target
	virtual void Render(IndexT frameIndex);
    /// access to camera settings
    Graphics::CameraSettings & CameraSetting(CameraEnum cm);
	
private:
	/// called when attached to graphics server
	virtual void OnAttachToServer();
	/// called when detached from graphics server
	virtual void OnRemoveFromServer();

	Ptr<CoreGraphics::RenderTarget> leftEyeTarget;
	Ptr<CoreGraphics::RenderTarget> rightEyeTarget;
	Ptr<Frame::FramePassBase> lastPass;    

    Graphics::CameraSettings camSettings[3];
	Math::matrix44 leftEyeProj;
	Math::matrix44 rightEyeProj;
    Math::matrix44 leftEyeDisplace;
    Math::matrix44 rightEyeDisplace;

	uint32_t HMDWidth;
	uint32_t HMDHeight;
};

//------------------------------------------------------------------------------
/**
*/
inline
Graphics::CameraSettings &
VrView::CameraSetting(VrView::CameraEnum cm)
{
    return this->camSettings[cm];
}

} // namespace VR