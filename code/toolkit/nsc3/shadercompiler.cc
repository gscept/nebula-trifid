//------------------------------------------------------------------------------
//  shadercompiler.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadercompiler.h"
#include "io/ioserver.h"
#include "shader.h"
#include "shadercodegenerator.h"

namespace Tools
{
__ImplementClass(Tools::ShaderCompiler, 'SCMP', Core::RefCounted);

using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ShaderCompiler::ShaderCompiler() :
    verbose(false)
{
    this->fragmentManager = ShaderFragmentManager::Create();
    this->samplerManager  = ShaderSamplerManager::Create();
}

//------------------------------------------------------------------------------
/**
*/
ShaderCompiler::~ShaderCompiler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Load all fragments from "proj:work/shaderlib/fragments".
*/
bool
ShaderCompiler::LoadFragments()
{
    URI uri = this->projDirectory;
    uri.AppendLocalPath("work/shaderlib/fragments");
    if (this->fragmentManager->LoadFragmentsAndGroups(uri))
    {
        return true;
    }
    else
    {
        n_printf("ERROR: Failed to load shader fragments!\n");
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Load sampler definitions from "proj:work/shaderlib/samplers".
*/
bool
ShaderCompiler::LoadSamplers()
{
    URI uri = this->projDirectory;
    uri.AppendLocalPath("work/shaderlib/samplers");
    if (this->samplerManager->LoadSamplers(uri))
    {
        return true;
    }
    else
    {
        n_printf("ERROR: Failed to load shader samplers!\n");
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Load shader definitions from shader files matching pattern 
    (relativ to "proj:work/shaderlib/shaders"). The shader will
    be appended to the shaders array.
*/
bool
ShaderCompiler::LoadShaders(const String& pattern)
{
    n_assert(pattern.IsValid());
    IoServer* ioServer = IoServer::Instance();

    // gather shaders matching the provided pattern
    URI uri = this->projDirectory;
    uri.AppendLocalPath("work/shaderlib/shaders");
    Array<String> files = ioServer->ListFiles(uri, pattern);
    IndexT i;
    for (i = 0; i < files.Size(); i++)
    {
        n_printf("Parsing shader file '%s'.\n", files[i].AsCharPtr());

        // build shader file uri
        URI fileUri = uri;
        fileUri.AppendLocalPath(files[i]);

        // create XML reader on file
        Ptr<Stream> stream = ioServer->CreateStream(fileUri);
        Ptr<XmlReader> xmlReader = XmlReader::Create();
        xmlReader->SetStream(stream);
        if (xmlReader->Open())
        {
            // make sure it's an Nebula3 material system file...
            if (xmlReader->HasNode("/Nebula3MaterialSystem"))
            {
                xmlReader->SetToNode("/Nebula3MaterialSystem");

                // for each shader definition...
                if (xmlReader->SetToFirstChild("Shader")) do
                {
                    // create a new Shader object
                    Ptr<Shader> shader = Shader::Create();
                    if (shader->Parse(xmlReader))
                    {
                        shader->SetFileUri(fileUri);
                        if (!this->shaders.Contains(shader->GetName()))
                        {
                            this->shaders.Add(shader->GetName(), shader);
                        }
                        else
                        {
                            n_printf("WARNING: duplicate shader '%s' in file '%s' and '%s'!\n",
                                shader->GetName().AsCharPtr(),
                                fileUri.AsString().AsCharPtr(),
                                this->shaders[shader->GetName()]->GetFileUri().AsString().AsCharPtr());
                        }
                    }
                    else
                    {
                        n_printf("ERROR parsing shader in file '%s' (skipping)", fileUri.AsString().AsCharPtr());
                    }
                }
                while (xmlReader->SetToNextChild("Shader"));
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Actually compile the loaded shaders.
*/
bool
ShaderCompiler::Compile()
{
    ShaderFragmentManager* fragManager = ShaderFragmentManager::Instance();

    // for each possible group fragment combination...
    if (fragManager->BeginIterateGroupFragments()) do
    {
        // for each shader...
        IndexT i;
        for (i = 0; i < this->shaders.Size(); i++)
        {
            const Ptr<Shader>& shader = this->shaders.ValueAtIndex(i);;
            n_printf("Compiling shader: %s...\n", shader->GetName().AsCharPtr());

            // reset the shader for recompilation
            shader->ResetNodes();

            // setup default nodes (Vertex, Globals, Shared, Samplers, ...)
            shader->ClearDefaultNodes();
            shader->SetupDefaultNodes();

            // setup the manual bindings
            if (!shader->SetupManualBindings())
            {
                n_printf("Error creating manual bindings '%s' in file '%s'!\n", 
                    shader->GetName().AsCharPtr(), 
                    shader->GetFileUri().AsString().AsCharPtr());
                return false;
            }

            // setup the remaining default bindings
            if (!shader->SetupDefaultBindings())
            {
                n_printf("Error creating default bindings '%s' in file '%s'!\n", 
                    shader->GetName().AsCharPtr(), 
                    shader->GetFileUri().AsString().AsCharPtr());
                return false;
            }

            // write the shader source code
            ShaderCodeGenerator codeGenerator;
            if (this->IsVerbose())
            {
                codeGenerator.SetDumpShaderStructure(true);
            }
            if (!codeGenerator.GenerateSourceCode(this->GetProjectDirectory(), shader))
            {
                n_printf("Error writing shader source!\n");
                return false;
            }
        }
    }
    while (fragManager->ContinueIterateGroupFragments());
    return true;
}

} // namespace Tools
