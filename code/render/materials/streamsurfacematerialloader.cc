//------------------------------------------------------------------------------
//  streamsurfacematerialloader.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "streamsurfacematerialloader.h"
#include "io/iointerface.h"
#include "io/memorystream.h"
#include "materials/surfacematerial.h"
#include "io/xmlreader.h"
#include "materialserver.h"
#include "resources/resourcemanager.h"
#include "io/bxmlreader.h"

using namespace IO;
using namespace Resources;
using namespace Messaging;
using namespace Util;
namespace Materials
{
__ImplementClass(Materials::StreamSurfaceMaterialLoader, 'SSML', Resources::ResourceLoader);

//------------------------------------------------------------------------------
/**
*/
StreamSurfaceMaterialLoader::StreamSurfaceMaterialLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
StreamSurfaceMaterialLoader::~StreamSurfaceMaterialLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamSurfaceMaterialLoader::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamSurfaceMaterialLoader::OnLoadRequested()
{
    n_assert(this->GetState() == Resource::Initial);
    n_assert(this->resource.isvalid());
    if (this->resource->IsAsyncEnabled())
    {
        // perform asynchronous load
        // send off an asynchronous loader job
        n_assert(!this->readStreamMsg.isvalid());
        this->readStreamMsg = ReadStream::Create();
        this->readStreamMsg->SetURI(this->resource->GetResourceId().Value());
        this->readStreamMsg->SetStream(MemoryStream::Create());
        IoInterface::Instance()->Send(this->readStreamMsg.upcast<Message>());

        // go into Pending state
        this->SetState(Resource::Pending);
        return true;
    }
    else
    {
        // perform synchronous load
        Ptr<Stream> stream = IoServer::Instance()->CreateStream(this->resource->GetResourceId().Value());
        if (this->SetupMaterialFromStream(stream))
        {
            this->SetState(Resource::Loaded);
            return true;
        }
        // fallthrough: synchronous loading failed
        this->SetState(Resource::Failed);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StreamSurfaceMaterialLoader::OnLoadCancelled()
{
    n_assert(this->GetState() == Resource::Pending);
    n_assert(this->readStreamMsg.isvalid());
    IoInterface::Instance()->Cancel(this->readStreamMsg.upcast<Message>());
    this->readStreamMsg = 0;
    ResourceLoader::OnLoadCancelled();
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamSurfaceMaterialLoader::OnPending()
{
    n_assert(this->GetState() == Resource::Pending);
    n_assert(this->readStreamMsg.isvalid());
    bool retval = false;

    // check if asynchronous loader job has finished
    if (this->readStreamMsg->Handled())
    {
        // ok, loader job has finished
        if (this->readStreamMsg->GetResult())
        {
            // IO operation was successful
            if (this->SetupMaterialFromStream(this->readStreamMsg->GetStream()))
            {
                // everything ok!
                this->SetState(Resource::Loaded);
                retval = true;
            }
            else
            {
                // this probably wasn't a Model file...
                this->SetState(Resource::Failed);
            }
        }
        else
        {
            // error during IO operation
            this->SetState(Resource::Failed);
        }
        // we no longer need the loader job message
        this->readStreamMsg = 0;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamSurfaceMaterialLoader::SetupMaterialFromStream(const Ptr<IO::Stream>& stream)
{
    n_assert(stream.isvalid());
    n_assert(stream->CanBeMapped());

    const Ptr<SurfaceMaterial>& surface = this->resource.downcast<SurfaceMaterial>();

	Ptr<BXmlReader> reader = BXmlReader::Create();
    reader->SetStream(stream);
    if (reader->Open())
    {
        // make sure it's a valid frame shader file
        if (!reader->HasNode("/NebulaT/Surface"))
        {
            n_error("StreamSurfaceMaterialLoader: '%s' is not a valid surface!", stream->GetURI().AsString().AsCharPtr());
            return false;
        }

        // send to first node
        reader->SetToNode("/NebulaT/Surface");

        // load surface
        Util::StringAtom materialTemplate = reader->GetString("template");
        const Ptr<Material>& material = MaterialServer::Instance()->GetMaterialByName(materialTemplate);
        const Util::Dictionary<Util::StringAtom, Material::MaterialParameter>& parameters = material->GetParameters();
        if (reader->SetToFirstChild("Param"))
        {
            Util::StringAtom paramName = reader->GetString("name");
            n_assert2(parameters.Contains(paramName), 
                Util::String::Sprintf("No parameter matching name '%s' exists in the material template '%s'", paramName.AsString().AsCharPtr(), material->GetName().AsString().AsCharPtr()).AsCharPtr()
                );
            const Material::MaterialParameter& param = parameters[paramName];
            Variant var = param.defaultVal;
            switch (param.defaultVal.GetType())
            {
                case Variant::Float:
                    var.SetFloat(reader->GetFloat("value"));
                    break;
                case Variant::Int:
                    var.SetInt(reader->GetInt("value"));
                    break;
                case Variant::Bool:
                    var.SetBool(reader->GetBool("value"));
                    break;
                case Variant::Float4:
                    var.SetFloat4(reader->GetFloat4("value"));
                    break;
                case Variant::Float2:
                    var.SetFloat2(reader->GetFloat2("value"));
                    break;
                case Variant::Matrix44:
                    var.SetMatrix44(reader->GetMatrix44("value"));
                    break;
                case Variant::String:
                {
                    // get texture
                    Ptr<ManagedTexture> tex = ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Texture::RTTI, reader->GetString("value")).downcast<ManagedTexture>();
                    surface->SetTexture(paramName, tex);
                    var.SetType(Variant::Object);
					var.SetObject(tex->GetTexture());
                    break;
                }
            }

            // add to the static values in the surface
            surface->staticValues.Add(paramName, var);
        }
        surface->Setup(material);
    }
    else
    {
        return false;
    }

    // everything went fine!
    return true;
}

} // namespace Materials