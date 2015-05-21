#pragma once
//------------------------------------------------------------------------------
/**
    @class TookitUtil::GameBatcher
           
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
#include "distributedtools/distributedtoolkitapp.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class GameBatcherApp : public DistributedTools::DistributedToolkitApp
{

public:

	/// opens the application
	bool Open();
	/// runs the application
	void DoWork();
	/// constructor
	GameBatcherApp();
	/// destructor
	virtual ~GameBatcherApp();

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