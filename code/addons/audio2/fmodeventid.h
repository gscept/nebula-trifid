#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio2::FmodEventId

    event path in its ".fev" project-file


    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
#include "audio2/fmodeventprojectid.h"

//------------------------------------------------------------------------------
namespace Audio2
{
class FmodEventId
{
public:
    /// constructor
    FmodEventId();
    /// constructor with arguments
    FmodEventId(const Audio2::FmodEventProjectId &_projectId, const Util::String &_eventPath);
    /// dtor
    ~FmodEventId();

    /// sets the FmodEventId, if your
    void Set(const Audio2::FmodEventProjectId &_projectId, const Util::String &_eventPath);
    /// return true if the FmodEventId is valid
    bool IsValid() const;
    /// clear content
    void Clear();

    /// get the id of the eventproject
    const Audio2::FmodEventProjectId& GetEventProjectId() const;
    /// get path in eventproject to event
    const Util::String& GetEventPath() const;

private:
    FmodEventProjectId projectId;
    Util::String eventPath;
};

//------------------------------------------------------------------------------
/**
*/
inline
FmodEventId::FmodEventId() :
    projectId(),
    eventPath()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
FmodEventId::FmodEventId(const Audio2::FmodEventProjectId &_projectId, const Util::String &_eventPath) :
    projectId(_projectId),
    eventPath(_eventPath)
{

}

//------------------------------------------------------------------------------
/**
*/
inline
FmodEventId::~FmodEventId()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
FmodEventId::IsValid() const
{
    return (this->projectId.IsValid() && this->eventPath.IsValid());
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FmodEventId::Set(const Audio2::FmodEventProjectId &_projectId, const Util::String &_eventPath)
{
    this->projectId = _projectId;
    this->eventPath = _eventPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FmodEventId::Clear()
{
    this->projectId.Clear();
    this->eventPath.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
const Audio2::FmodEventProjectId&
FmodEventId::GetEventProjectId() const
{
    return this->projectId;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
FmodEventId::GetEventPath() const
{
    return this->eventPath;
}


} // namespace Audio2
//------------------------------------------------------------------------------
