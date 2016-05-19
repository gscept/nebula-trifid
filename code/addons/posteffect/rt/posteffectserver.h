#pragma once
//------------------------------------------------------------------------------
/**
    @class PostEffect::PostEffectServer
        
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "coregraphics/shadervariable.h"
#include "materials/materialvariableinstance.h"
#include "coregraphics/shadervariableinstance.h"
#include "graphics/modelentity.h"
#include "timing/time.h"
#include "posteffect/rt/params/colorparams.h"
#include "posteffect/rt/params/depthoffieldparams.h"
#include "posteffect/rt/params/fogparams.h"
#include "posteffect/rt/params/hdrparams.h"
#include "posteffect/rt/params/lightparams.h"
#include "posteffect/rt/params/skyparams.h"
#include "posteffect/rt/params/aoparams.h"
#include "frame/frameshader.h"
#include "util/delegate.h"

// uncomment this to use the lighttransform in the posteffect presets
//#define USE_POSTEFFECT_GLOBALLIGHTTRANSFORM

// forward declaration
namespace Graphics
{
    class GlobalLightEntity;
}
namespace Resources
{
    class ManagedTexture;
}

//------------------------------------------------------------------------------
namespace PostEffect
{
class PostEffectServer : public Core::RefCounted
{
    __DeclareClass(PostEffectServer);
    __DeclareSingleton(PostEffectServer);

public:

    enum PostEffectType
    {
        Color = 0,
        DoF = 1,
        Fog = 2,
        Hdr = 3,
        Light = 4,
        Sky = 5,
        AO = 6,
        Fadeout = 7,
        FadeoutAndWait = 8,
        Fadein = 9,

        NumPostEffectTypes,
        InvalidPostEffectType
    };

    enum FadeMode
    {
        FadeOut = 0,
        FadeIn,
        NoFade
    };

    /// constructor
    PostEffectServer();
    /// destructor
    virtual ~PostEffectServer();  
    
    /// open the posteffect server
    void Open();
    /// close the posteffect server
    void Close();
	/// start fade
	void StartFade(Timing::Time time, float start, float target);
	/// start fading with a callback
	void StartFadeCallback(Timing::Time time, float start, float target, Util::Delegate<Timing::Time> callback);

    /// on frame
    void OnFrame(Timing::Time time);

    /// get current fademode    
    FadeMode GetFadeMode() const;

    /// starts blending for specified effect
    void StartBlending(const Ptr<ParamBase>& target, Timing::Time fadeTime, PostEffectType postEffectType);
    /// stops blending for specified effect
    void StopBlending(PostEffectType postEffectType);
	/// stop all blending operations
	void StopAllBlending();

	/// set the sky entity
	void SetSkyEntity(const Ptr<Graphics::ModelEntity>& entity);
    /// preload a texture
    void PreloadTexture(const Util::String& resource);
    /// unload one texture
    void UnloadTexture(const Util::String& resource);
    /// unload all textures
    void UnloadTextures();
	/// retreive texture by name
	Ptr<Resources::ManagedTexture> FindTexture(const Util::String& resource);

	/// set global light entity post effect system uses
	void SetGlobalLightEntity(const Ptr<Graphics::GlobalLightEntity>& l);
	/// get global light entity post effect system uses, can be null
	const Ptr<Graphics::GlobalLightEntity>& GetGlobalLightEntity();

private:
    /// apply special parameter
    void ApplyParameters(PostEffectType type);

    /// set the depth of field parameters
    void ApplyDepthOfFieldParameters();    
    /// set the color parameters
    void ApplyColorParameters(); 
    /// set the fog parameters
    void ApplyFogParameters();
    /// set the hdr parameters
    void ApplyHdrParameters();
    /// set the light parameters
    void ApplyLightParameters();
    /// set the sky parameters
    void ApplySkyParameters();
    /// set ao parameters
    void ApplyAOParameters();

    Timing::Time lastTime;

    /// compute fadevalue
    float ComputeFadeValue(Timing::Time frameTime);
    /// simple fade parameters
    Timing::Time fadeTime;
    float currentFadeValue;
	float currentFadeTarget;
    FadeMode curFadeMode;    

    /// finds the current sky entities
    bool FindCurrentSkyEntities();

    /// struct for actuall parameters and assigned targets
    struct ParameterSet
    {
        Ptr<ParamBase> current;
        Ptr<ParamBase> target;
        Timing::Time lastTime ;
        Timing::Time blendEndTime;
        Timing::Time blendTime;
    };

    /// compute fade values, returns true if something changed
    bool ComputeBlending(PostEffectType type);	
    /// holds all of the parameter sets
    Util::FixedArray<ParameterSet> postEffects;

    /// handle to frame shader
    Ptr<Frame::FrameShader> frameShader;

    /// shader variable handles
    Ptr<CoreGraphics::ShaderVariable> fadeShaderVariable;
    Ptr<CoreGraphics::ShaderVariable> saturationShaderVariable;
	Ptr<CoreGraphics::ShaderVariable> maxLuminanceShaderVar;
    Ptr<CoreGraphics::ShaderVariable> balanceShaderVariable;
    Ptr<CoreGraphics::ShaderVariable> fogDistancesVariable;
    Ptr<CoreGraphics::ShaderVariable> fogColorVariable;
    Ptr<CoreGraphics::ShaderVariable> hdrVerticalScaleVariable;
	Ptr<CoreGraphics::ShaderVariable> hdrHorizontalScaleVariable;
    Ptr<CoreGraphics::ShaderVariable> hdrColorVariable;
    Ptr<CoreGraphics::ShaderVariable> hdrThresholdVariable;
    Ptr<CoreGraphics::ShaderVariable> dofShaderVariable;
    Ptr<Materials::SurfaceConstant> skyContrast;
    Ptr<Materials::SurfaceConstant> skyBrightness;
    Ptr<Materials::SurfaceConstant> skyBlendFactor;
    Ptr<Materials::SurfaceConstant> skyBlendTexture;
    Ptr<Materials::SurfaceConstant> skyBaseTexture;
    Ptr<Materials::SurfaceConstant> skyRotationFactor;
    bool skyLoaded; // flag is true if sky model was loaded and all shader params could be initialized

	/// handle to sky entity
	Ptr<Graphics::ModelEntity> skyEntity;

    /// handle to global light
    Ptr<Graphics::GlobalLightEntity> globalLight;

	/// callback to run when the fading is done
	Util::Delegate<Timing::Time> callback;

	// keep track of preloaded textures
	Util::Array<Ptr<Resources::ManagedTexture>> texturePool;
};


//------------------------------------------------------------------------------
/**
*/
inline void 
PostEffectServer::StartFade( Timing::Time time, float start, float target )
{
	this->curFadeMode = (PostEffectServer::FadeMode)(start < target);
	this->currentFadeValue = start;
	this->currentFadeTarget = target;
	this->fadeTime = time;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PostEffect::PostEffectServer::StartFadeCallback( Timing::Time time, float start, float target, Util::Delegate<Timing::Time> callback )
{
	this->StartFade(time, start, target);
	this->callback = callback;
}

//------------------------------------------------------------------------------
/**
*/
inline PostEffectServer::FadeMode 
PostEffectServer::GetFadeMode() const
{
    return this->curFadeMode;
}

//------------------------------------------------------------------------------
/**
*/
inline void
PostEffectServer::SetGlobalLightEntity(const Ptr<Graphics::GlobalLightEntity>& l)
{
	this->globalLight = l;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Ptr<Graphics::GlobalLightEntity>&
PostEffectServer::GetGlobalLightEntity()
{
	return this->globalLight;
}

} // namespace Debug
//------------------------------------------------------------------------------
