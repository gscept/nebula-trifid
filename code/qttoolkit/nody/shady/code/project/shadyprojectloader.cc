//------------------------------------------------------------------------------
//  shadyprojectloader.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadyprojectloader.h"
#include "project/project.h"
#include "node/shadynode.h"

namespace Shady
{
__ImplementClass(Shady::ShadyProjectLoader, 'SHPL', Nody::ProjectLoader);

//------------------------------------------------------------------------------
/**
*/
ShadyProjectLoader::ShadyProjectLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ShadyProjectLoader::~ShadyProjectLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyProjectLoader::ReadGlobalState( const Ptr<IO::BinaryReader>& reader, const Ptr<Nody::Project>& project )
{
    Nody::ProjectLoader::ReadGlobalState(reader, project);
    ShadyNode::GlobalParamCount = reader->ReadUInt();
}
} // namespace Shady