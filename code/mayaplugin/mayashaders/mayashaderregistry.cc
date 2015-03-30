//------------------------------------------------------------------------------
//  mayashaderregistry.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayashaderregistry.h"
#include "io/ioserver.h"

namespace Maya
{
__ImplementClass(Maya::MayaShaderRegistry, 'MSRG', Core::RefCounted);
__ImplementSingleton(Maya::MayaShaderRegistry);

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
MayaShaderRegistry::MayaShaderRegistry() :  
    isValid(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
MayaShaderRegistry::~MayaShaderRegistry()
{
    if (this->IsValid())
    {
        this->Discard();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaShaderRegistry::Setup()
{
    n_assert(!this->IsValid());
    n_assert(this->shaders.IsEmpty());
    n_assert(this->shaderIndexMap.IsEmpty());

    this->isValid = true;
    IoServer* ioServer = IoServer::Instance();

    // first try to open project specific shader definition file
    String projShadersFile = "proj:work/shaders/n3shaders.xml";
    String toolkitShadersFile = "toolkit:work/shaders/n3shaders.xml";
    if (ioServer->FileExists(projShadersFile))
    {
        this->SetupFromXml(projShadersFile);
    }
    else if (ioServer->FileExists(toolkitShadersFile))
    {
        this->SetupFromXml(toolkitShadersFile);
    }
    else
    {
        n_error("n3shaders.xml not found in 'proj:work/shaders' or 'toolkit:work/shaders'\n"); 
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MayaShaderRegistry::Discard()
{
    n_assert(this->IsValid());
    this->shaders.Clear();
    this->shaderIndexMap.Clear();
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaShaderRegistry::SetupFromXml(const String& path)
{
    Ptr<Stream> stream = IoServer::Instance()->CreateStream(path);
    Ptr<XmlReader> xmlReader = XmlReader::Create();
    xmlReader->SetStream(stream);
    if (xmlReader->Open())
    {
        if (!xmlReader->HasNode("/Nebula3Shaders"))
        {
            n_error("'%s' is not a Nebula3 shader definition file!\n", path.AsCharPtr());
        }
        xmlReader->SetToNode("/Nebula3Shaders");
        if (xmlReader->SetToFirstChild("Shader")) do
        {
            MayaShader newShader;
            newShader.SetupFromXml(xmlReader);
            this->shaders.Append(newShader);
            this->shaderIndexMap.Add(newShader.GetId(), this->shaders.Size() - 1);
        }
        while (xmlReader->SetToNextChild("Shader"));
        xmlReader->Close();
    }
    else
    {
        n_error("Failed to open '%s' as XML file!\n", path.AsCharPtr());
    }
}

} // namespace Maya