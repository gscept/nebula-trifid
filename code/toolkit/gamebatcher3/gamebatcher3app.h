#pragma once
//------------------------------------------------------------------------------
/**
    @class TookitUtil::FBXBatcher3App
    
    Entry point for FBX batcher
    
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
#include "distributedtools/distributedtoolkitapp.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class GameBatcher3App : public DistributedTools::DistributedToolkitApp
{

public:

	/// opens the application
	bool Open();
	/// runs the application
	void DoWork();
	/// constructor
	GameBatcher3App();
	/// destructor
	virtual ~GameBatcher3App();
private:
	/// parse command line arguments
	bool ParseCmdLineArgs();
	/// setup project info object
	bool SetupProjectInfo();
	/// print help text
	void ShowHelp();
}; 
} // namespace TookitUtil
//------------------------------------------------------------------------------