//------------------------------------------------------------------------------
//  shadyprojectloader.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadyprojectloader.h"
#include "project/project.h"
#include "node/shadynode.h"
#include "shadyproject.h"

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
ShadyProjectLoader::ReadGlobalState(const Ptr<IO::BinaryReader>& reader, const Ptr<Nody::Project>& project)
{
    Nody::ProjectLoader::ReadGlobalState(reader, project);
    ShadyNode::GlobalParamCount = reader->ReadUInt();
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyProjectLoader::ReadImplementation(const Ptr<IO::BinaryReader>& reader, const Ptr<Nody::Project>& project)
{
	// save shady specific stuff
	const Ptr<Shady::ShadyProject>& sproj = project.downcast<Shady::ShadyProject>();
	while (!reader->Eof())
	{
		uint flag = reader->ReadUInt();
		switch (flag)
		{
		case 'MATE':
		{
			SizeT numpasses = reader->ReadUInt();
			IndexT i;
			for (i = 0; i < numpasses; i++)
			{
				sproj->material.batches.Append(reader->ReadString());
			}
		}
		case '<IMP':
		default:
			return;
		}
	}
}

} // namespace Shady