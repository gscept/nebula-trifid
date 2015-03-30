#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio2::FmodEventProjectId
  
    path relatively to "audio:" of a ".fev" project-file without the extension ".fev"
    
    example:
        for this project file:
            "audio:level1/level1.fev"
        the FmodEventProjectId must be constructed like this:
            FmodEventProjectId("level1/level1");

    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/    
#include "util/string.h"

//------------------------------------------------------------------------------
namespace Audio2
{
class FmodEventProjectId
{
public:    
    /// constructor
    FmodEventProjectId();
    /// constructor with arguments
    FmodEventProjectId(const Util::String &_id);
    /// constructor with char-ptr
    FmodEventProjectId(const char* ptr);
    /// dtor
    ~FmodEventProjectId();

    /// assignment from char ptr
    void operator=(const char* ptr);
    /// equality operator
    bool operator==(const FmodEventProjectId &other) const;
    /// inequality operator
    bool operator!=(const FmodEventProjectId &other) const;
    /// less than operator
    bool operator<(const FmodEventProjectId &other) const;
    /// greater than operator
    bool operator>(const FmodEventProjectId &other) const;
    /// sets the FmodEventProjectId, if your 
    void Set(const Util::String &name);
    /// return true if the FmodEventProjectId is valid
    bool IsValid() const;
    /// clear content
    void Clear();
    /// get the path amd file name relatively to "audio:"
    const Util::String& GetFullFilePathAndName() const;
    /// just the project name without file-extension and path
    const Util::String& GetName() const;

private:
    Util::String projectName;
    Util::String projectFullFilePathAndName;
};

//------------------------------------------------------------------------------
/**
*/
inline
FmodEventProjectId::FmodEventProjectId()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
FmodEventProjectId::FmodEventProjectId(const Util::String &projectName)
{
    this->Set(projectName);
}

//------------------------------------------------------------------------------
/**
*/
inline
FmodEventProjectId::FmodEventProjectId(const char* ptr)
{
    this->Set(Util::String(ptr));
}

//------------------------------------------------------------------------------
/**
*/
inline
FmodEventProjectId::~FmodEventProjectId()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventProjectId::IsValid() const
{
    return this->projectName.IsValid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
FmodEventProjectId::Set(const Util::String &_projectName)
{
    this->projectName = _projectName;
    this->projectFullFilePathAndName = this->projectName + "/" + this->projectName + Util::String(".fev");
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
FmodEventProjectId::Clear()
{
    this->projectName.Clear();
    this->projectFullFilePathAndName.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
FmodEventProjectId::GetFullFilePathAndName() const
{
    return this->projectFullFilePathAndName;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String& 
FmodEventProjectId::GetName() const
{
    return this->projectName;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventProjectId::operator==(const FmodEventProjectId &other) const
{
    return (this->projectName == other.projectName);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventProjectId::operator!=(const FmodEventProjectId &other) const
{
    return (this->projectName != other.projectName);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventProjectId::operator<(const FmodEventProjectId &other) const
{
    return (this->projectName < other.projectName);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
FmodEventProjectId::operator>(const FmodEventProjectId &other) const
{
    return (this->projectName > other.projectName);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
FmodEventProjectId::operator=(const char* ptr)
{
    this->Set(Util::String(ptr));
}


} // namespace Audio2
//------------------------------------------------------------------------------
