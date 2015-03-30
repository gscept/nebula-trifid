#pragma once
//------------------------------------------------------------------------------
/**
	@class DistributedTools::DistributedTestToolApp

    Simple test tool, that are able to run distributed. Manipulates textfiles.

    (C) 2009 Radon Labs GmbH
*/
//------------------------------------------------------------------------------
#include "distributedtools/distributedtoolkitapp.h"

//------------------------------------------------------------------------------
namespace DistributedTools
{
class DistributedTestToolApp : public DistributedToolkitApp
{
public:
    // constructor
    DistributedTestToolApp();
    // convert text files
    virtual void DoWork();
protected:
    /// print help text
    virtual void ShowHelp();
};
} // namespace DistributedTools