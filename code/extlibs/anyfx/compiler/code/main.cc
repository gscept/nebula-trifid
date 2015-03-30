//------------------------------------------------------------------------------
/**
    AnyFX Compiler entry point
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "cmdlineargs.h"
#include "util.h"
#include "afxcompiler.h"

//------------------------------------------------------------------------------
/**
*/
int
main(int argc, char** argv)
{
	AnyFX::CmdLineArgs args(argc, argv);
	std::string file;
	std::string output;
	std::string target;
	std::vector<std::string> defines;

	AnyFXBeginCompile();
	defines = args.GetArguments("-D");

	bool validArgs = false;
	if (args.HasArgument("-f"))
	{
		file = args.GetArgument("-f");
		if (args.HasArgument("-o"))
		{
			output = args.GetArgument("-o");
			if (args.HasArgument("-target"))
			{
				target = args.GetArgument("-target");
				AnyFXErrorBlob* errors;
				AnyFXCompile(file, output, target, defines, &errors);
				if (errors)
				{
					printf(errors->buffer);
				}
				delete errors;
			}
			else
			{
				AnyFX::Emit("Compiler must have target platform\n");
			}
		}
		else
		{
			AnyFX::Emit("Compiler must have output path\n");
		}
	}
	else
	{
		AnyFX::Emit("Compiler must have target file\n");
	}

	AnyFXEndCompile();

	return 0;
}

