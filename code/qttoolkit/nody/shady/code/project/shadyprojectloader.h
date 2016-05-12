#pragma once
//------------------------------------------------------------------------------
/**
    @class Shady::ShadyProjectLoader
    
    Loads a Shady project from binary file.
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "project/projectloader.h"
namespace Shady
{
class Project;
class ShadyProjectLoader : public Nody::ProjectLoader
{
	__DeclareClass(ShadyProjectLoader);
public:
	/// constructor
	ShadyProjectLoader();
	/// destructor
	virtual ~ShadyProjectLoader();

private:
    /// helper function for reading global state
    void ReadGlobalState(const Ptr<IO::BinaryReader>& reader, const Ptr<Nody::Project>& project);
	/// helper function for reading implementation specific information
	void ReadImplementation(const Ptr<IO::BinaryReader>& reader, const Ptr<Nody::Project>& project);
}; 
} // namespace Shady
//------------------------------------------------------------------------------