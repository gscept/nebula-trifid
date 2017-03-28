#pragma once
//------------------------------------------------------------------------------
/**
    @class Script::DialogDesc

    Highlevel description of a dialog. This is one entry in the dialog table
    created by the dialog managers.

    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "util/guid.h"
#include "util/string.h"
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace Script
{
class DialogDesc : public Core::RefCounted
{
    __DeclareClass(DialogDesc);
public:
    /// constructor
    DialogDesc();
    /// set dialog guid
    void SetGuid(const Util::Guid& guid);
    /// get dialog guid
    const Util::Guid& GetGuid() const;
    /// set dialog id
    void SetId(const Util::String& id);
    /// get dialog id
    const Util::String& GetId() const;
    /// set dialog speaker
    void SetSpeaker(const Util::String& speaker);
    /// get dialog speaker
    const Util::String& GetSpeaker() const;
    /// set dialog speaker group 
    void SetGroup(const Util::String& group);
    /// get dialog speaker group 
    const Util::String& GetGroup() const;
    /// set dialog locked state
    void SetLocked(bool b);
    /// get dialog locked state
    bool IsLocked() const;
    /// set conversation flag
    void SetConversation(bool b);
    /// is conversation 
    bool IsConversation() const;


private:
    Util::Guid guid;
    Util::String id;
    Util::String name;
    Util::String speaker;
    Util::String group;
    bool isLocked;
    bool isConversation;
};

//------------------------------------------------------------------------------
/**
*/
inline void
DialogDesc::SetGuid(const Util::Guid& g)
{
    this->guid = g;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Guid&
DialogDesc::GetGuid() const
{
    return this->guid;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DialogDesc::SetId(const Util::String& s)
{
    this->id = s;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
DialogDesc::GetId() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DialogDesc::SetSpeaker(const Util::String& s)
{
    this->speaker = s;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
DialogDesc::GetSpeaker() const
{
    return this->speaker;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DialogDesc::SetGroup(const Util::String& g)
{
    this->group = g;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
DialogDesc::GetGroup() const
{
    return this->group;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DialogDesc::SetLocked(bool b)
{
    this->isLocked = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
DialogDesc::IsLocked() const
{
    return this->isLocked;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DialogDesc::SetConversation(bool b)
{
    this->isConversation = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
DialogDesc::IsConversation() const
{
    return this->isConversation;
}

} // namespace Script
//------------------------------------------------------------------------------