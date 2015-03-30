#pragma once
//------------------------------------------------------------------------------
/**
    @class Toolkit::N2TableConverterApp
    
    Application class for Nebula2 Excel table conversion.
    
    (C) 2009 Radon Labs GmbH
*/
#include "toolkitutil/toolkitapp.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class N2TableConverterApp : public ToolkitUtil::ToolkitApp
{
public:
    /// run the application
    virtual void Run();

private:
    /// print help text
    virtual void ShowHelp();
}; 

} // namespace Toolkit

//------------------------------------------------------------------------------
    