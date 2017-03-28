//------------------------------------------------------------------------------
//  projectsaver.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "projectsaver.h"
#include "project.h"
#include "io/ioserver.h"
#include "io/binarywriter.h"

using namespace Util;
using namespace IO;
namespace Nody
{
__ImplementClass(Nody::ProjectSaver, 'PRSA', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ProjectSaver::ProjectSaver()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ProjectSaver::~ProjectSaver()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ProjectSaver::Save(const Ptr<Project>& project, const IO::URI& path)
{
    n_assert(project.isvalid());
    n_assert(path.IsValid());

    // open stream
    Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(path);
    stream->SetAccessMode(IO::Stream::WriteAccess);

    if (stream->Open())
    {
        // setup writer
        Ptr<BinaryWriter> writer = BinaryWriter::Create();
        writer->SetStream(stream);
        writer->Open();

        // mark start of file
        writer->WriteUInt('NODY');

		// write global state
        writer->WriteUInt('GLOB');        
        this->WriteGlobalState(writer, project);

		// write implementation
		writer->WriteUInt('>IMP');
		this->WriteImplementation(writer, project);
		writer->WriteUInt('<IMP');

		// write nodes
        writer->WriteUInt('NODE');        
        this->WriteNodes(writer, project);
       
		// write links
        writer->WriteUInt('LINK');
        this->WriteLinks(writer, project);

        // close stream
        stream->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ProjectSaver::WriteGlobalState(const Ptr<IO::BinaryWriter>& writer, const Ptr<Project>& project)
{
    // empty, in the generic case there is no global state
	writer->WriteFloat2(project->globalState.viewCenter);
}

//------------------------------------------------------------------------------
/**
*/
void
ProjectSaver::WriteNodes(const Ptr<IO::BinaryWriter>& writer, const Ptr<Project>& project)
{
    // write nodes
    SizeT numNodes = project->nodes.Size();
    writer->WriteInt(numNodes);

    IndexT i;
    for (i = 0; i < numNodes; i++)
    {
        const Project::NodeState& node = project->nodes[i];
        writer->WriteFloat2(node.pos);
        writer->WriteGuid(node.id);
        writer->WriteString(node.variation);
        writer->WriteBool(node.superNode);

        // write constant values
        SizeT numValues = node.values.Size();
        writer->WriteInt(numValues);
        IndexT j;
        for (j = 0; j < numValues; j++)
        {
            const Util::Variant& value = node.values.ValueAtIndex(j);

            // write name of variable
            writer->WriteString(node.values.KeyAtIndex(j));

            // write value type and value
            writer->WriteUInt(value.GetType());
            switch (value.GetType())
            {
            case Variant::Float:
                writer->WriteFloat(value.GetFloat());
                break;
            case Variant::FloatArray:
                writer->WriteFloatArray(value.GetFloatArray());
                break;
            case Variant::Float2:
                {
                    Util::Array<float> vec(2, 0, 0);
                    const Math::float2 val = value.GetFloat2();
                    vec[0] = val.x();
                    vec[1] = val.y();
                    writer->WriteFloatArray(vec);
                    break;
                }                
            case Variant::Float4:
                {
                    Util::Array<float> vec(4, 0, 0);
                    const Math::float4 val = value.GetFloat4();
                    vec[0] = val.x();
                    vec[1] = val.y();
                    vec[2] = val.z();
                    vec[3] = val.w();
                    writer->WriteFloatArray(vec);
                    break;
                }                
            case Variant::IntArray:
                writer->WriteIntArray(value.GetIntArray());
                break;
            case Variant::Int:
                writer->WriteInt(value.GetInt());
                break;
            case Variant::Bool:
                writer->WriteBool(value.GetBool());
                break;
            case Variant::BoolArray:
                writer->WriteBoolArray(value.GetBoolArray());
                break;
            case Variant::String:
                writer->WriteString(value.GetString());
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ProjectSaver::WriteLinks( const Ptr<IO::BinaryWriter>& writer, const Ptr<Project>& project )
{
    // write links
    SizeT numLinks = project->links.Size();
    writer->WriteInt(numLinks);

    IndexT i;
    for (i = 0; i < numLinks; i++)
    {
        const Project::LinkState& link = project->links[i];
        writer->WriteGuid(link.from);
        writer->WriteGuid(link.to);
        writer->WriteString(link.fromName);
        writer->WriteString(link.toName);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ProjectSaver::WriteImplementation(const Ptr<IO::BinaryWriter>& writer, const Ptr<Project>& project)
{
	// override in subclass
}

} // namespace Nody