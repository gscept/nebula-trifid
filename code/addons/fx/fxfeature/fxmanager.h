#pragma once
//------------------------------------------------------------------------------
/**
    @class FxFeature::FxManager

    This manager is the client frontend for the FX::FxServer on the render
    thread site. Handle all communication to the FX subsystem through this
    manager!
    
    (C) 2009 RadonLabs GmbH
*/
#include "core/refcounted.h"
#include "core/ptr.h"
#include "core/singleton.h"
#include "math/vector.h"
#include "timing/time.h"
#include "resources/resourceid.h"

//------------------------------------------------------------------------------
namespace FxFeature
{
class FxManager : public Core::RefCounted
{
	__DeclareClass(FxManager);
    __DeclareSingleton(FxManager);
public:
    /// constructor
    FxManager();
    /// destructor
    virtual ~FxManager();
    /// open the fx manager
    virtual bool Open();
    /// close the fx manager
    virtual void Close();

    //////////////////////////////////////////////////////////////////////////  effects

    /// create shake effect
    void EmitCameraShakeEffect( const Math::vector& intensity,
                                const Math::vector& rotation,
                                Timing::Time duration,
                                float range,
                                const Math::matrix44& transform);

    /// create and send a vibration effect
    void EmitVibrationEffect(   float highFreqIntensity,
                                Timing::Time highFreqDuration,
                                float lowFreqIntensity,
                                Timing::Time lowFreqDuration,
                                IndexT playerIndex);

	/// create and send an audio effect
	void EmitAudioEffect( const Math::point& position,
						  int amplitude,
						  const Resources::ResourceId& sound,
						  Timing::Time duration);

	/// create and send a graphics effect
	void EmitGraphicsEffect( const Math::matrix44& transform,
							  const Resources::ResourceId& model,
							  Timing::Time duration);


    //////////////////////////////////////////////////////////////////////////  effect mixer
    /// attach a camerashake mixer
    void SetupCameraShakeMixer( const Math::vector& maxDisplacement, 
                                const Math::vector& maxThumble);

private:
    bool isOpen;
};

}; // namespace FxFeature

//------------------------------------------------------------------------------
