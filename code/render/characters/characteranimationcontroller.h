#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::CharacterAnimationController
    
    Allows to control the animation of a character instance, this is
    basically a convenience wrapper around Animation::AnimSequencer.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "animation/animsequencer.h"
#include "characters/characteranimationlibrary.h"
#include "jobs/jobport.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharacterAnimationController
{
public:
    /// constructor
    CharacterAnimationController();
    /// destructor
    ~CharacterAnimationController();

    /// setup the anim controller object
    void Setup(const Ptr<Jobs::JobPort>& jobPort, const CharacterAnimationLibrary& animLib);
    /// discard the anim controller object
    void Discard();
    /// return true if object has been setup
    bool IsValid() const;

    /// setup the character instance for anim driven motion
    void SetupAnimDrivenMotion(IndexT animDrivenMotionJointIndex);
    /// return true if anim driven motion is enabled on the character instance
    bool IsAnimDrivenMotionEnabled() const;
    /// get computed animation driven motion vector
    const Math::vector& GetAnimDrivenMotionVector();

    /// enqueue an anim job
    void EnqueueAnimJob(const Ptr<Animation::AnimJob>& animJob);
    /// stop all anim jobs on a given track
    void StopTrack(IndexT trackIndex, bool allowFadeOut=true);
    /// stop all anim jobs
    void StopAllTracks(bool allowFadeOut=true);
	/// pause all anim jobs on a given track
	void PauseTrack(IndexT trackIndex);
	/// pause all anim jobs
	void PauseAllTracks();

	/// sets current time for anim jobs
	void SetTime(Timing::Tick time);
	/// get the time which is currently set in the anim controller
	Timing::Tick GetTime() const;

    /// direct access to embedded anim sequencer object
    Animation::AnimSequencer& AnimSequencer();

private:
    friend class CharacterInstance;

    /// update the animation controller
    bool Update();
    /// update the anim-driven motion vector (called by Update)
    void UpdateAnimDrivenMotionVector();

    Animation::AnimSequencer animSequencer;
    Ptr<Jobs::JobPort> extJobPort;          // pointer to external job port
    IndexT animDrivenMotionJointIndex;
    Math::vector animDrivenMotionVector;
    bool animDrivenMotionVectorDirty;
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
CharacterAnimationController::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline Animation::AnimSequencer&
CharacterAnimationController::AnimSequencer()
{
    return this->animSequencer;
}

//------------------------------------------------------------------------------
/**
*/
inline Timing::Tick
CharacterAnimationController::GetTime() const
{
    return this->animSequencer.GetTime();
}

//------------------------------------------------------------------------------
/**
    NOTE: caller needs to make sure that the animation evaluation jobs
    have finished!
*/
inline const Math::vector& 
CharacterAnimationController::GetAnimDrivenMotionVector()
{
    if (this->animDrivenMotionVectorDirty)
    {
        this->UpdateAnimDrivenMotionVector();
    }
    return this->animDrivenMotionVector;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
CharacterAnimationController::IsAnimDrivenMotionEnabled() const
{
    return InvalidIndex != this->animDrivenMotionJointIndex;
}

} // namespace Characters
//------------------------------------------------------------------------------
    