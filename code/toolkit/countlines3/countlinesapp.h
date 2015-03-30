#pragma once
//------------------------------------------------------------------------------
/**	
    @class Toolkit::CountLinesApp
    
    App class for N3 line counter.
    
    (C) 2009 Radon Labs GmbH
*/
#include "app/consoleapplication.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class CountLinesApp : public App::ConsoleApplication
{
public:
    /// run the application
    virtual void Run();
    
private:
    /// store line count information
    class LineCounts
    {
    public:
        /// constructor
        LineCounts() : all(0), general(0), win32(0), xbox360(0), win360(0), wii(0), ps3(0) {};
    public:
        Util::String dir;
        SizeT all;
        SizeT general;
        SizeT win32;
        SizeT xbox360;
        SizeT win360;
        SizeT wii;
        SizeT ps3;
    };

    /// line counts for one layer
    class Layer
    {
    public:
        Util::String name;
        Util::Array<LineCounts> dirs;
    };

    /// count lines in a source code layer
    void CountLinesLayer(const Util::String& dir);
    /// count lines in a subsystem directory
    LineCounts CountLinesSubsystem(const Util::String& dir);
    /// count lines in a flat source code directory
    SizeT CountLinesFlat(const Util::String& dir);
    /// check if this is an ignored directory (like .svn, etc..)
    bool IsIgnoreDir(const Util::String& dir) const;

    Util::Array<Layer> layers;
};

} // namespace Toolkit
//------------------------------------------------------------------------------
    