//------------------------------------------------------------------------------
//  ShaderCompiler.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ShaderCompiler.h"
#include "..\..\foundation\io\filestream.h"
#include "..\..\foundation\io\uri.h"
#include <D3Dcompiler.h>

namespace Tools
{

//------------------------------------------------------------------------------
/**
*/
ShaderCompiler::ShaderCompiler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ShaderCompiler::~ShaderCompiler()
{
    n_assert(!this->IsOpen());
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderCompiler::Run()
{

	for (int i = 0; i < this->args.GetNumArgs()-2; i++)
	{
		Util::String temp = this->args.GetStringAtIndex(i);
		temp.Append("\n");
		//n_printf(temp.AsCharPtr());
	}
    /// get the relevant parts of the command line arguments
	SizeT argSize = this->args.GetNumArgs();

	/// the last argument is the source of the shader, that is, the .fx file
	Util::String src = this->args.GetStringAtIndex(argSize-1);

	/// the second to last argument is the destionation for the compiled shader
	Util::String dst = this->args.GetStringAtIndex(argSize-2);

	Util::String output = "compiling ";
	output.Append(src);
	output.Append(" to ");
	output.Append(dst);
	output.Append("...\n");

	n_printf(output.AsCharPtr());

	ID3D10Blob *effectBlob = 0, *errorsBlob = 0;
	/// compile the shader
	HRESULT hr = D3DX11CompileFromFile(
		src.AsCharPtr(), 0, 0, 0, "fx_5_0", 0, 0, 0, &effectBlob, &errorsBlob, &hr);
	if (FAILED(hr) || (errorsBlob!=0 && errorsBlob->GetBufferSize() > 0))
	{
		n_printf((char*)errorsBlob->GetBufferPointer());
	}
	else
	{
		/// if the compilation succeeded, the compiled shader is written to the destination file
		IO::FileStream* filestream = IO::FileStream::Create();
		filestream->SetURI(dst);		
		filestream->SetAccessMode(IO::FileStream::WriteAccess);
		filestream->Open();		
		filestream->Write(effectBlob->GetBufferPointer(), effectBlob->GetBufferSize());
		filestream->Flush();
		filestream->Close();
	}	
	
	if (errorsBlob) errorsBlob->Release();

	

}

} // namespace Tools