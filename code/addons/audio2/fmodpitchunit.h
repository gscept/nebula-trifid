#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio2::FmodPitchUnit

    information about which pitch unit to apply
    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/

//------------------------------------------------------------------------------
namespace Audio2
{
    enum FmodPitchUnit
    {
        PitchUnitUnknown = -1,
        // Pitch is specified in raw underlying units.
        PitchUnitRaw = 0,
        // Pitch is specified in units of octaves.
        PitchUnitOctaves,
        // Pitch is specified in units of semitones.
        PitchUnitSemitones,
        // Pitch is specified in units of tones.
        PitchUnitTones,

        PitchUnitCount
    };

} // namespace Audio2
//------------------------------------------------------------------------------
