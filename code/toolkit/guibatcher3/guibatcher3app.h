#pragma once
//------------------------------------------------------------------------------
/**
    @class TookitUtil::GuiBatcher3App
    
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
#include "distributedtools/distributedtoolkitapp.h"
#include "texutil/textureconverter.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class GuiBatcher3App : public DistributedTools::DistributedToolkitApp
{

public:

	/// opens the application
	bool Open();
	/// runs the application
	void DoWork();
	/// constructor
	GuiBatcher3App();
	/// destructor
	virtual ~GuiBatcher3App();
private:
	/// parse command line arguments
	bool ParseCmdLineArgs();
	/// setup project info object
	bool SetupProjectInfo();
	/// print help text
	void ShowHelp();

	ToolkitUtil::TextureConverter textureConverter;
}; 
} // namespace TookitUtil
//------------------------------------------------------------------------------