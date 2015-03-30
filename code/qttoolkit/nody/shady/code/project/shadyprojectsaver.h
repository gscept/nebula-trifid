#pragma once
//------------------------------------------------------------------------------
/**
    @class Shady::ShadyProjectSaver
    
    Saves a Shady specific project to binary file.
    
    (C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "project/projectsaver.h"
namespace Shady
{
class Project;
class ShadyProjectSaver : public Nody::ProjectSaver
{
	__DeclareClass(ShadyProjectSaver);
public:
	/// constructor
	ShadyProjectSaver();
	/// destructor
	virtual ~ShadyProjectSaver();

private:
    /// helper function for reading global state
    void WriteGlobalState(const Ptr<IO::BinaryWriter>& writer, const Ptr<Nody::Project>& project);
}; 
} // namespace Shady
//------------------------------------------------------------------------------