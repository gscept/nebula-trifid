//------------------------------------------------------------------------------
//  shadercompiler.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadercompiler.h"
#include "io/ioserver.h"
#include "io/xmlreader.h"

#if __DX11__
#include <d3dx11.h>
#include <D3Dcompiler.h>
#endif

#if __ANYFX__
#include "afxcompiler.h"
#endif

using namespace Util;
using namespace IO;
namespace ToolkitUtil
{

//------------------------------------------------------------------------------
/**
*/
ShaderCompiler::ShaderCompiler() :
	language("HLSL"),
    srcShaderCustomDir(""),
    srcFrameShaderCustomDir(""),
    srcMaterialCustomDir(""),    
	platform(Platform::Win32),
	force(false),
	debug(false),
	quiet(false)
{
	// empty
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
*/
bool 
ShaderCompiler::CheckRecompile( const Util::String& srcPath, const Util::String& dstPath )
{
	if (!this->force)
	{
		// check file stamps
		IoServer* ioServer = IoServer::Instance();
		if (ioServer->FileExists(dstPath))
		{
			FileTime srcTime = ioServer->GetFileWriteTime(srcPath);
			FileTime dstTime = ioServer->GetFileWriteTime(dstPath);
			if (dstTime > srcTime)
			{
				return false;
			}
		}
	}
	return true;
}
//------------------------------------------------------------------------------
/**
*/
bool 
ShaderCompiler::CompileShaders()
{
	n_assert(this->srcShaderBaseDir.IsValid());
	n_assert(this->dstShaderDir.IsValid());
	const Ptr<IoServer>& ioServer = IoServer::Instance();
	this->shaderNames.Clear();

	// check if source dir exists
	if (!ioServer->DirectoryExists(this->srcShaderBaseDir))
	{
		n_printf("WARNING: shader source directory '%s' not found!\n", this->srcShaderBaseDir.AsCharPtr());
		return false;
	}

	// make sure the target directory exists
	ioServer->CreateDirectory(this->dstShaderDir);

	// attempt compile base shaders
	bool retval = false;
	if (this->language == "HLSL")
	{
		retval = this->CompileHLSL(this->srcShaderBaseDir);
	}
	else if (this->language == "GLSL")
	{
		retval = this->CompileGLSL(this->srcShaderBaseDir);
	}

    if (this->srcFrameShaderCustomDir.IsValid())
    {
        // attempt to compile custom shaders
        if (this->language == "HLSL")
        {
            this->CompileHLSL(this->srcShaderCustomDir);
        }
        else if (this->language == "GLSL")
        {
            this->CompileGLSL(this->srcShaderCustomDir);
        }
    }    

	if (retval)
	{
		retval = this->WriteShaderDictionary();
	}

	return retval;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ShaderCompiler::CompileFrameShaders()
{
	n_assert(this->srcFrameShaderBaseDir.IsValid());
	n_assert(this->dstFrameShaderDir.IsValid());
	const Ptr<IoServer>& ioServer = IoServer::Instance();

	// check if source dir exists
	if (!ioServer->DirectoryExists(this->srcFrameShaderBaseDir))
	{
		n_printf("WARNING: frame shader source directory '%s' not found!\n", this->srcFrameShaderBaseDir.AsCharPtr());
		return false;
	}

	// make sure target dir exists
	ioServer->CreateDirectory(this->dstFrameShaderDir);

	// for each base frame shader...
	bool success = true;
	Array<String> srcFiles = ioServer->ListFiles(this->srcFrameShaderBaseDir, "*.xml");
	IndexT i;
	for (i = 0; i < srcFiles.Size(); i++)
	{
		// build absolute source and target filenames
		String srcPath;
		srcPath.Format("%s/%s", this->srcFrameShaderBaseDir.AsCharPtr(), srcFiles[i].AsCharPtr());
		String dstPath;
		dstPath.Format("%s/%s", this->dstFrameShaderDir.AsCharPtr(), srcFiles[i].AsCharPtr());
		success &= ioServer->CopyFile(srcPath, dstPath);
		n_printf("Copied base frame shader: %s ---> %s \n", srcPath.AsCharPtr(), dstPath.AsCharPtr());
	}

    if (this->srcFrameShaderCustomDir.IsValid())
    {
        // for each custom frame shader...
        srcFiles = ioServer->ListFiles(this->srcFrameShaderCustomDir, "*.xml");
        for (i = 0; i < srcFiles.Size(); i++)
        {
            // build absolute source and target filenames
            String srcPath;
            srcPath.Format("%s/%s", this->srcFrameShaderCustomDir.AsCharPtr(), srcFiles[i].AsCharPtr());
            String dstPath;
            dstPath.Format("%s/%s", this->dstFrameShaderDir.AsCharPtr(), srcFiles[i].AsCharPtr());
            ioServer->CopyFile(srcPath, dstPath);
            n_printf("Copied custom frame shader: %s ---> %s \n", srcPath.AsCharPtr(), dstPath.AsCharPtr());
        }
    }
    
	return success;
}


//------------------------------------------------------------------------------
/**
*/
bool 
ShaderCompiler::CompileMaterials()
{
	n_assert(this->srcMaterialBaseDir.IsValid());
	n_assert(this->dstMaterialDir.IsValid());
	const Ptr<IoServer>& ioServer = IoServer::Instance();

	// check if source dir exists
	if (!ioServer->DirectoryExists(this->srcMaterialBaseDir))
	{
		n_printf("WARNING: materials source directory '%s' not found!\n", this->srcMaterialBaseDir.AsCharPtr());
		return false;
	}

    Util::String baseMaterialTemplateSrcDir = this->srcMaterialBaseDir;
    Util::String customMaterialTemplateSrcDir = this->srcMaterialCustomDir;
    Util::String materialTemplateDstDir = this->dstMaterialDir;

    // remove old directories
    ioServer->DeleteDirectory(materialTemplateDstDir);

	// make new
    ioServer->CreateDirectory(materialTemplateDstDir);

	// for each base material table...
	bool success = true;
    Array<String> srcFiles = ioServer->ListFiles(baseMaterialTemplateSrcDir, "*.xml");
	IndexT i;
	for (i = 0; i < srcFiles.Size(); i++)
	{
		// build absolute source and target filenames
		String srcPath;
        srcPath.Format("%s/%s", baseMaterialTemplateSrcDir.AsCharPtr(), srcFiles[i].AsCharPtr());
		String dstPath;
        dstPath.Format("%s/%s", materialTemplateDstDir.AsCharPtr(), srcFiles[i].AsCharPtr());
		success &= ioServer->CopyFile(srcPath, dstPath);
		n_printf("Copied base material template table: %s ---> %s \n", srcPath.AsCharPtr(), dstPath.AsCharPtr());
	}

    if (this->srcMaterialCustomDir.IsValid())
    {
        // for each custom material table...
        srcFiles = ioServer->ListFiles(customMaterialTemplateSrcDir, "*.xml");
        for (i = 0; i < srcFiles.Size(); i++)
        {
            // build absolute source and target filenames
            String srcPath;
            srcPath.Format("%s/%s", customMaterialTemplateSrcDir.AsCharPtr(), srcFiles[i].AsCharPtr());
            String dstPath;
            
            // format material to be extended with _custom
            String file = srcFiles[i];
            file.StripFileExtension();
            dstPath.Format("%s/%s_custom.xml", materialTemplateDstDir.AsCharPtr(), file.AsCharPtr());

            // copy file
            ioServer->CopyFile(srcPath, dstPath);
            n_printf("Copied custom material table: %s ---> %s \n", srcPath.AsCharPtr(), dstPath.AsCharPtr());
        }
    }
    
	return success;
}


//------------------------------------------------------------------------------
/**
*/
bool 
ShaderCompiler::CompileHLSL(const Util::String& srcPath)
{
	const Ptr<IoServer>& ioServer = IoServer::Instance();

	// copy sdh files to destination
	bool retval = true;

	// list files in directory
	Array<String> srcFiles = ioServer->ListFiles(srcPath, "*.fx");

#ifndef __DX11__
	n_printf("Error: Cannot compile DX11 shaders without DX11 support\n");
	return false;
#endif

	// go through all files and compile
	IndexT j;
	for (j = 0; j < srcFiles.Size(); j++)
	{
		// get file
		String file = srcFiles[j];

		// compile
		n_printf("Compiling: %s\n", file.AsCharPtr());

		// format string with file
		String srcFile = srcPath + "/" + file;

		// format destination
		String destFile = this->dstShaderDir + "/" + file;
		destFile.StripFileExtension();
		file.StripFileExtension();

		// add to dictionary
		this->shaderNames.Append(file);

		if (this->CheckRecompile(srcFile, destFile))
		{
#if __DX11__
			ID3D10Blob* compiledShader = 0;
			ID3D10Blob* errorBlob = 0;

			HRESULT hr = D3DX11CompileFromFile(
				AssignRegistry::Instance()->ResolveAssignsInString(srcFile).AsCharPtr(),
				NULL,
				NULL,
				NULL,
				"fx_5_0",
				D3DCOMPILE_OPTIMIZATION_LEVEL3,
				NULL,
				NULL,
				&compiledShader,
				&errorBlob,
				&hr);

			if (FAILED(hr))
			{
				if (errorBlob)
				{
					String error;
					error.AppendRange((const char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize());
					n_printf("%s\n", error.AsCharPtr());
				}
				else
				{
					n_printf("Unhandled compilation error!\n");
				}
				retval = false;
			}
			else
			{


				// write compiled shader to file
				Ptr<Stream> stream = ioServer->CreateStream(destFile);
				stream->SetAccessMode(Stream::WriteAccess);

				// open stream
				if (stream->Open())
				{
					// write shader
					stream->Write(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize());

					// close file
					stream->Close();
				}
				else
				{
					n_printf("Couldn't open file %s\n", destFile.AsCharPtr());
					retval = false;
				}
			}
#endif
		}
		else
		{
			n_printf("No need to recompile %s\n", file.AsCharPtr());
			retval = true;
		}

	}
	return retval;
}

//------------------------------------------------------------------------------
/**
	Implemented using AnyFX
*/
bool 
ShaderCompiler::CompileGLSL(const Util::String& srcPath)
{
	const Ptr<IoServer>& ioServer = IoServer::Instance();

	// list files in directory
	Array<String> srcFiles = ioServer->ListFiles(srcPath, "*.fx");

#ifndef __ANYFX__
	n_printf("Error: Cannot compile DX11 shaders without DX11 support\n");
	return false;
#endif

#if __ANYFX__
	// start AnyFX compilation
	AnyFXBeginCompile();

	// go through all files and compile
	IndexT j;
	for (j = 0; j < srcFiles.Size(); j++)
	{
		// get file
		String file = srcFiles[j];

		// compile
		n_printf("Compiling: %s\n", file.AsCharPtr());

		// format string with file
		String srcFile = srcPath + "/" + file;

		// format destination
		String destFile = this->dstShaderDir + "/" + file;
		destFile.StripFileExtension();
		file.StripFileExtension();

		// add to dictionary
		this->shaderNames.Append(file);

		if (this->CheckRecompile(srcFile, destFile))
		{
			URI src(srcFile);
			URI dst(destFile);
            URI shaderFolder("root:work/shaders/gl");
			std::vector<std::string> defines;
            Util::String define;
            define.Format("-D GLSL");
			defines.push_back(define.AsCharPtr());

            // first include this folder
            define.Format("-I%s/", URI(srcPath).LocalPath().AsCharPtr());
            defines.push_back(define.AsCharPtr());

            // then include the N3/work/shaders/gl folder
            define.Format("-I%s/", shaderFolder.LocalPath().AsCharPtr());
            defines.push_back(define.AsCharPtr());
			AnyFXErrorBlob* errors = NULL;

            GLint minor, major;
            glGetIntegerv(GL_MAJOR_VERSION, &major);
            glGetIntegerv(GL_MINOR_VERSION, &minor);

			// this will get the highest possible value for the GL version, now clamp the minor and major to the one supported by glew
			major = Math::n_min(major, 4);
			minor = Math::n_min(minor, 4);
			
			// create vendor string
			Util::String vendor = (const char*)glGetString(GL_VENDOR);
			if (vendor.FindStringIndex("NVIDIA") != InvalidIndex) vendor = "NVIDIA";
			else if (vendor.FindStringIndex("AMD") != InvalidIndex) vendor = "AMD";
			else if (vendor.FindStringIndex("Intel") != InvalidIndex) vendor = "INTEL";

            Util::String target;
            target.Format("gl%d%d", major, minor);
			bool res = AnyFXCompile(src.GetHostAndLocalPath().AsCharPtr(), dst.GetHostAndLocalPath().AsCharPtr(), target.AsCharPtr(), vendor.AsCharPtr(), defines, &errors);
			if (!res)
			{
				if (errors)
				{
					n_printf("%s\n", errors->buffer);
					delete errors;
					errors = 0;
				}
				return false;
			}
            else if (errors)
            {
                n_printf("%s\n", errors->buffer);
                delete errors;
                errors = 0;
            }
		}
		else
		{
			n_printf("No need to recompile %s\n", file.AsCharPtr());
		}
	}

	// stop AnyFX compilation
	AnyFXEndCompile();
#else
#error "No GLSL compiler implemented! (use definition __ANYFX__ to fix this)"
#endif

	return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ShaderCompiler::WriteShaderDictionary()
{
	String filename;
	filename.Format("%s/shaders.dic", this->dstShaderDir.AsCharPtr());
	Ptr<Stream> stream = IoServer::Instance()->CreateStream(filename);
	Ptr<TextWriter> textWriter = TextWriter::Create();
	textWriter->SetStream(stream);
	if (textWriter->Open())
	{
		String prefix = "shd:";
		IndexT i;
		for (i = 0; i < this->shaderNames.Size(); i++)
		{
			textWriter->WriteLine(prefix + this->shaderNames[i]);        
		}
		textWriter->Close();
		return true;
	}
	return false;
}


} // namespace ToolkitUtil