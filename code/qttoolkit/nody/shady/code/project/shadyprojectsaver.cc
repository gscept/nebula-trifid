//------------------------------------------------------------------------------
//  shadyprojectsaver.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadyprojectsaver.h"
#include "project/project.h"
#include "node/shadynode.h"
#include "shadyproject.h"

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
ShadyProjectSaver::WriteGlobalState(const Ptr<IO::BinaryWriter>& writer, const Ptr<Nody::Project>& project)
{
    Nody::ProjectSaver::WriteGlobalState(writer, project);
    writer->WriteUInt(ShadyNode::GlobalParamCount);
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyProjectSaver::WriteImplementation(const Ptr<IO::BinaryWriter>& writer, const Ptr<Nody::Project>& project)
{
	// load shady specific stuff
	const Ptr<Shady::ShadyProject>& sproj = project.downcast<Shady::ShadyProject>();

	// save material
	writer->WriteUInt('MATE');
	writer->WriteUInt(sproj->material.batches.Size());
	IndexT i;
	for (i = 0; i < sproj->material.batches.Size(); i++)
	{
		writer->WriteString(sproj->material.batches[i]);
	}
}

} // namespace Shady