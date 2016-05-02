#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::ProjectSaver
    
    Implements a class which saves projects in a binary format.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "io/binarywriter.h"
namespace Nody
{
class Project;
class ProjectSaver : public Core::RefCounted
{
	__DeclareClass(ProjectSaver);
public:
	/// constructor
	ProjectSaver();
	/// destructor
	virtual ~ProjectSaver();

    /// saves a project by providing a project object and a binary saver
    void Save(const Ptr<Project>& project, const IO::URI& path);

protected:
    /// helper function for writing global state
    virtual void WriteGlobalState(const Ptr<IO::BinaryWriter>& writer, const Ptr<Project>& project);
    /// helper function for writing nodes
    virtual void WriteNodes(const Ptr<IO::BinaryWriter>& writer, const Ptr<Project>& project);
    /// helper function for writing links
    virtual void WriteLinks(const Ptr<IO::BinaryWriter>& writer, const Ptr<Project>& project);
	/// helper function for writing implementation information
	virtual void WriteImplementation(const Ptr<IO::BinaryWriter>& writer, const Ptr<Project>& project);
}; 
} // namespace Nody
//------------------------------------------------------------------------------