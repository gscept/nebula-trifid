#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::ProjectLoader
    
    Loads projects in the binary format and sets up the internal structure to be that of the file
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "io/uri.h"
#include "io/binaryreader.h"
namespace Nody
{
class Project;
class ProjectLoader : public Core::RefCounted
{
	__DeclareClass(ProjectLoader);
public:
	/// constructor
	ProjectLoader();
	/// destructor
	virtual ~ProjectLoader();

    /// loads project from path
    void Load(const Ptr<Project>& project, const IO::URI& path);

protected:
    /// helper function for reading global state
    virtual void ReadGlobalState(const Ptr<IO::BinaryReader>& reader, const Ptr<Project>& project);
    /// helper function for reading nodes
    virtual void ReadNodes(const Ptr<IO::BinaryReader>& reader, const Ptr<Project>& project);
    /// helper function for reading links
    virtual void ReadLinks(const Ptr<IO::BinaryReader>& reader, const Ptr<Project>& project);
	/// helper function for reading implementation specific information
	virtual void ReadImplementation(const Ptr<IO::BinaryReader>& reader, const Ptr<Project>& project);
}; 
} // namespace Nody
//------------------------------------------------------------------------------