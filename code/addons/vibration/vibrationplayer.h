#pragma once
//------------------------------------------------------------------------------
/**
    @class Vibration::VibrationPlayer
    
    Client-side vibration player for fire-and-forget vibrations. FoF-vibs don't have
    a client-side representation after they are fired, so the application
    cannot manipulate a vibration is active.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "timing/time.h"

//------------------------------------------------------------------------------
namespace Vibration
{
class VibrationPlayer : public Core::RefCounted
{
    __DeclareClass(VibrationPlayer);
    __DeclareSingleton(VibrationPlayer);
public:
    /// constructor
    VibrationPlayer();
    /// destructor
    virtual ~VibrationPlayer();
    
    /// activate a vibration
    void AddEnvelopeVibration(IndexT playerIndex,
                              float hiFreq,
                              Timing::Time hiAttack,
                              Timing::Time hiSustain,
                              Timing::Time hiRelease,
                              float lowFreq,
                              Timing::Time lowAttack,
                              Timing::Time lowSustain,
                              Timing::Time lowRelease);

    /// clear vibrations
    void ClearVibration(IndexT playerIndex,
                        bool hiFreq = true,
                        bool lowFreq = true);
};

} // namespace Vibration
//------------------------------------------------------------------------------