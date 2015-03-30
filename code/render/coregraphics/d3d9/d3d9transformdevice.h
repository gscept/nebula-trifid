#pragma once
//------------------------------------------------------------------------------
/**
    @class Win360::D3D9TransformDevice
    
    D3D9/Xbox360 version of TransformDevice.
    
    (C) 2007 Radon Labs GmbH
*/
#include "coregraphics/base/transformdevicebase.h"
#include "coregraphics/shadervariable.h"
#include "core/singleton.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9TransformDevice : public Base::TransformDeviceBase
{
    __DeclareClass(D3D9TransformDevice);
    __DeclareSingleton(D3D9TransformDevice);
public:
    /// constructor
    D3D9TransformDevice();
    /// destructor
    virtual ~D3D9TransformDevice();

    /// open the transform device
    bool Open();
    /// close the transform device
    void Close();

    /// updates shared shader variables dependent on view matrix
    void ApplyViewSettings();
    /// apply any model transform needed, implementation is platform dependend
    void ApplyModelTransforms(const Ptr<CoreGraphics::ShaderInstance>& shdInst);

private:
    /// shared shader variables
    Ptr<CoreGraphics::ShaderVariable> shdEyePos;
    Ptr<CoreGraphics::ShaderVariable> viewMatrix;
    Ptr<CoreGraphics::ShaderVariable> invViewMatrix;
    Ptr<CoreGraphics::ShaderVariable> projectionMatrix;
    Ptr<CoreGraphics::ShaderVariable> invProjectionMatrix;
    Ptr<CoreGraphics::ShaderVariable> focalLengthShaderVar;

    CoreGraphics::ShaderVariable::Semantic mvpSemantic;
    CoreGraphics::ShaderVariable::Semantic modelSemantic;
    CoreGraphics::ShaderVariable::Semantic modelViewSemantic;
    CoreGraphics::ShaderVariable::Semantic viewSemantic;
    CoreGraphics::ShaderVariable::Semantic invViewSemantic;
    CoreGraphics::ShaderVariable::Semantic invModelViewSemantic;
    CoreGraphics::ShaderVariable::Semantic viewProjSemantic;
    CoreGraphics::ShaderVariable::Semantic eyePosSemantic;
    CoreGraphics::ShaderVariable::Semantic halfPixelSizeSemantic;
    CoreGraphics::ShaderVariable::Semantic projectionSemantic;
    CoreGraphics::ShaderVariable::Semantic invProjectionSemantic;
};

} // namespace Direct3D9
//------------------------------------------------------------------------------
    