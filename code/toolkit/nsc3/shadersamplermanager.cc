//------------------------------------------------------------------------------
//  shadersamplermanager.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadersamplermanager.h"
#include "io/ioserver.h"

namespace Tools
{
__ImplementClass(Tools::ShaderSamplerManager, 'SSMM', Core::RefCounted);
__ImplementSingleton(Tools::ShaderSamplerManager);

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
ShaderSamplerManager::ShaderSamplerManager()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ShaderSamplerManager::~ShaderSamplerManager()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    Load all shader samplers from the XML files in the provided path.
*/
bool
ShaderSamplerManager::LoadSamplers(const URI& uri)
{
    IoServer* ioServer = IoServer::Instance();
    Array<String> files = ioServer->ListFiles(uri, "*.xml");
    IndexT i;
    for (i = 0; i < files.Size(); i++)
    {
        n_printf("Parsing sampler file '%s'\n", files[i].AsCharPtr());

        // build fragment file uri
        URI fileUri = uri;
        fileUri.AppendLocalPath(files[i]);

        // create a stream with an XML reader and load fragments from it
        // note that there may be several fragment definitions per file
        Ptr<Stream> stream = ioServer->CreateStream(fileUri);
        Ptr<XmlReader> xmlReader = XmlReader::Create();
        xmlReader->SetStream(stream);
        if (xmlReader->Open())
        {
            // make sure it's an Nebula3 material system file...
            if (xmlReader->HasNode("/Nebula3MaterialSystem"))
            {
                xmlReader->SetToNode("/Nebula3MaterialSystem");

                // for each sampler node...
                if (xmlReader->SetToFirstChild("Sampler")) do
                {
                    // create a new ShaderFragment object
                    Ptr<ShaderSampler> shaderSampler = ShaderSampler::Create();
                    if (shaderSampler->Parse(xmlReader))
                    {
                        if (!this->samplers.Contains(shaderSampler->GetName()))
                        {
                            this->samplers.Add(shaderSampler->GetName(), shaderSampler);
                        }
                        else
                        {
                            n_printf("WARNING: duplicate sampler '%s' in files '%s' and '%s'!\n",
                                shaderSampler->GetName().AsCharPtr(),
                                fileUri.AsString().AsCharPtr(),
                                this->samplers[shaderSampler->GetName()]->GetFileURI().AsString().AsCharPtr());
                        }
                    }
                    else
                    {
                        n_printf("ERROR parsing sampler in file '%s' (skipping)", fileUri.AsString().AsCharPtr());
                    }
                }
                while (xmlReader->SetToNextChild("Sampler"));
            }
        }
    }
    return true;
}

} // namespace Tools