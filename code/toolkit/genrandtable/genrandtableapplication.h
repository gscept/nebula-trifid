#pragma once
#ifndef TOOLS_GENRANDTABLEAPPLICATION_H
#define TOOLS_GENRANDTABLEAPPLICATION_H
//------------------------------------------------------------------------------
/**
    @class Tools::GenRandTableApplication
    
    A little tool to generate a random table. Expected args:
    
    -size: number of entries in random table
    -file: output file name
    
    (C) 2008 Radon Labs GmbH
*/
#include "app/consoleapplication.h"

//------------------------------------------------------------------------------
namespace Tools
{
class GenRandTableApplication : public App::ConsoleApplication
{
public:
    /// run the application
    void Run();
};

} // namespace Tools
//------------------------------------------------------------------------------
#endif
    