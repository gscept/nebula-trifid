#pragma once
//------------------------------------------------------------------------------
/**
    @class Toolkit::SequenceConverterApp
    
   Sequence and Cutscene file converter application class.
   This should be used, when attributes of trackelements has been changed.
    
    (C) 2009 Radon Labs GmbH
*/
#include "toolkitutil/toolkitapp.h"
#include "toolkitutil/xmlnodemodifier.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class SequenceConverterApp : public ToolkitUtil::ToolkitApp
{
public:
    /// constructor
    SequenceConverterApp();
    /// destructor
    ~SequenceConverterApp();
    /// run the application
    virtual void Run();
    /// close the application
    virtual void Close();

private:
    /// parse command line arguments
    bool ParseCmdLineArgs();
    /// print help text
    void ShowHelp();
    /// init modifier
    bool InitModifier();
    /// perform conversion
    bool ConvertSequence(Util::String fileName);
    /// gets project path from nebula toolkit
    IO::URI GetProjDir(void);

    /// flag if application waits for user input before closing
    bool waitForKey;
    /// file to be converted, convert all files if empty 
    Util::String sourceFileName;
    /// path to the source directory with the xml files that should be parsed
    IO::URI srcDir;
    /// path to the destination directory with the xml files that should be written to
    IO::URI dstDir;
    /// path to the xml specification file containing conversion rules
    IO::URI specFilePath;
    /// modifier which performs the conversion
    Ptr<ToolkitUtil::XMLNodeModifier> modifier;
};

} // namespace Toolkit
//------------------------------------------------------------------------------
