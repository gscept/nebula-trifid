//------------------------------------------------------------------------------
//  projectloader.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "projectloader.h"
#include "io/ioserver.h"
#include "util/variant.h"
#include "project.h"
#include "node/node.h"
#include "link/link.h"

using namespace Util;
using namespace IO;
namespace Nody
{
__ImplementClass(Nody::ProjectLoader, 'PRLO', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ProjectLoader::ProjectLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ProjectLoader::~ProjectLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ProjectLoader::Load(const Ptr<Project>& project, const IO::URI& path)
{
    n_assert(project.isvalid());
    n_assert(path.IsValid());

    // open stream
    Ptr<Stream> stream = IoServer::Instance()->CreateStream(path);
    stream->SetAccessMode(Stream::ReadAccess);

    if (stream->Open())
    {
        // setup writer
        Ptr<BinaryReader> reader = BinaryReader::Create();
        reader->SetStream(stream);
        reader->Open();

        // read header
        uint magic = reader->ReadUInt();
        n_assert(magic == 'NODY');

        // rather neat way of reading contents of a file without requiring anything.
        // this format allows us to have the internal file structure any way we want.
        while(!reader->Eof())
        {
            uint section = reader->ReadUInt();
            switch (section)
            {
            case 'GLOB':
                this->ReadGlobalState(reader, project);
                break;
            case 'NODE':
                this->ReadNodes(reader, project);
                break;
            case 'LINK':
                this->ReadLinks(reader, project);
                break;
			case '>IMP':
				this->ReadImplementation(reader, project);
				break;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ProjectLoader::ReadGlobalState(const Ptr<IO::BinaryReader>& reader, const Ptr<Project>& project)
{
    // empty, this is purposely empty in Nody since it should be filled by an application specific project
	project->globalState.viewCenter = reader->ReadFloat2();
}

//------------------------------------------------------------------------------
/**
*/
void
ProjectLoader::ReadNodes(const Ptr<IO::BinaryReader>& reader, const Ptr<Project>& project)
{
    SizeT numNodes = reader->ReadUInt();

    // read nodes
    IndexT i;
    for (i = 0; i < numNodes; i++)
    {
        Project::NodeState state;
        state.pos = reader->ReadFloat2();
        state.id = reader->ReadGuid();
        state.variation = reader->ReadString();
        state.superNode = reader->ReadBool();

        // create node, keep it empty until we apply to scene
        Ptr<Node> node = Node::Create();

        SizeT numValues = reader->ReadUInt();
        IndexT j;
        for (j = 0; j < numValues; j++)
        {
            // get name of value
            String valueName = reader->ReadString();

            // get type of value
            Variant::Type type = (Util::Variant::Type)reader->ReadUInt();
            Variant value;
            switch (type)
            {
            case Variant::Float:
                value.SetFloat(reader->ReadFloat());
                break;
            case Variant::FloatArray:
                value.SetFloatArray(reader->ReadFloatArray());
                break;
            case Variant::Float2:                       // convert to float array
                {
                    Math::float2 val = reader->ReadFloat2();
                    Array<float> vec(2, 0, 0);
                    vec[0] = val.x();
                    vec[1] = val.y();
                    value.SetFloatArray(vec);
                    break;
                }                
            case Variant::Float4:                       // convert to float array
                {
                    Math::float4 val = reader->ReadFloat4();
                    Array<float> vec(4, 0, 0);
                    vec[0] = val.x();
                    vec[1] = val.y();
                    vec[2] = val.z();
                    vec[3] = val.w();
                    value.SetFloatArray(vec);
                    break;
                }                
            case Variant::Int:
                value.SetInt(reader->ReadInt());
                break;
            case Variant::IntArray:
                value.SetIntArray(reader->ReadIntArray());
                break;
            case Variant::Bool:
                value.SetBool(reader->ReadBool());
                break;
            case Variant::BoolArray:
                value.SetBoolArray(reader->ReadBoolArray());
                break;
            case Variant::String:
                value.SetString(reader->ReadString());
                break;
            }

            // set value in node
            state.values.Add(valueName, value);
        }

        // add key-value pair to dictionary
        project->nodes.Append(state);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ProjectLoader::ReadLinks(const Ptr<IO::BinaryReader>& reader, const Ptr<Project>& project)
{
    SizeT numLinks = reader->ReadUInt();    
    IndexT i;
    for (i = 0; i < numLinks; i++)
    {
        Project::LinkState state;
        state.from = reader->ReadGuid();
        state.to = reader->ReadGuid();
        state.fromName = reader->ReadString();
        state.toName = reader->ReadString();

        // create empty link pointer, we only need it for the dictionary
        Ptr<Link> link = 0;
        
        // add to project
        project->links.Append(state);
    }    
}

//------------------------------------------------------------------------------
/**
*/
void
ProjectLoader::ReadImplementation(const Ptr<IO::BinaryReader>& reader, const Ptr<Project>& project)
{
	// override in subclass
}

} // namespace Nody