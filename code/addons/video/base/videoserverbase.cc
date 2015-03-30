//------------------------------------------------------------------------------
//  videoserverbase.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "video/base/videoserverbase.h"

namespace Base
{
__ImplementClass(Base::VideoServerBase, 'VISB', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
VideoServerBase::VideoServerBase() :
    isOpen(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
VideoServerBase::~VideoServerBase()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
VideoServerBase::Open()
{
    n_assert(!this->isOpen);
    this->SetupVideoSystem();
    this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void
VideoServerBase::Close()
{
    n_assert(this->IsOpen());


    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
VideoServerBase::OnFrame(Timing::Time time)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
VideoServerBase::OnRenderBefore(Timing::Time time)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
VideoServerBase::SetupVideoSystem()
{
    // implement in subclass
}

//------------------------------------------------------------------------------
/**    
*/
void 
VideoServerBase::StartVideo(const Util::StringAtom& resName, 
                            const Math::float2& upperLeft,
                            const Math::float2& upperRight,
                            const Math::float2& lowerLeft,
                            const Math::float2& lowerRight,
                            bool fromFile,
                            bool autoDelete,
                            bool loop)
{
    // implement in subclass
}

//------------------------------------------------------------------------------
/**    
*/
void
VideoServerBase::StopVideo(const Util::StringAtom& resName,
                           bool del)
{
    // implement in subclass
}

//------------------------------------------------------------------------------
/**    
*/
void
VideoServerBase::PauseVideo(const Util::StringAtom& resName)
{
    // implement in subclass
}

//------------------------------------------------------------------------------
/**    
*/
void 
VideoServerBase::ResumeVideo(const Util::StringAtom& resName)
{
    // implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
bool
VideoServerBase::DeleteVideo(const Util::StringAtom& resName)
{
    // implement in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
VideoServerBase::IsVideoPlaying(const Util::StringAtom& resName)
{
    // implement in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
VideoServerBase::IsVideoPausing(const Util::StringAtom& resName)
{
    // implement in subclass
    return false;
}
} // namespace Base
