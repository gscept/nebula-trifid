#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaLogger
  
    A central logger class for the Maya plugin. Redirects all messages
    both to the Maya console and to Nebula3.
    
    (C) 2009 Radon Labs GmbH
*/    
#include "core/refcounted.h"
#include "core/singleton.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaLogger : public Core::RefCounted
{
    __DeclareClass(MayaLogger);
    __DeclareSingleton(MayaLogger);
public:
    /// constructor
    MayaLogger();
    /// destructor
    virtual ~MayaLogger();

    /// setup the Maya logger
    void Setup();
    /// discard the Maya logger
    void Discard();
    /// return true if object has been setup
    bool IsValid() const;

    /// print a simple message
    void __cdecl Print(const char* fmt, ...);
    /// put a warning message
    void __cdecl Warning(const char* fmt, ...);
    /// put an error message and cancel execution
    void __cdecl Error(const char* fmt, ...);

private:
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaLogger::IsValid() const
{
    return this->isValid;
}

} // namespace Maya
//------------------------------------------------------------------------------
