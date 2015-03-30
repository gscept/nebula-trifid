//------------------------------------------------------------------------------
//  shadyprojectsaver.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadyprojectsaver.h"
#include "project/project.h"
#include "node/shadynode.h"

namespace Shady
{
__ImplementClass(Shady::ShadyProjectSaver, 'SHPS', Nody::ProjectSaver);

//------------------------------------------------------------------------------
/**
*/
ShadyProjectSaver::ShadyProjectSaver()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ShadyProjectSaver::~ShadyProjectSaver()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyProjectSaver::WriteGlobalState( const Ptr<IO::BinaryWriter>& writer, const Ptr<Nody::Project>& project )
{
    Nody::ProjectSaver::WriteGlobalState(writer, project);
    writer->WriteUInt(ShadyNode::GlobalParamCount);
}
} // namespace Shady