//------------------------------------------------------------------------------
//  shader.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include <istream>
#include <string>
#include <algorithm>
#include <sstream>
#include "GL/glew.h"
#include "shader.h"
#include "programrow.h"
#include "parameter.h"
#include "datatype.h"
#include "util.h"
#include "structure.h"
#include "varblock.h"
#include "typechecker.h"
#include "generator.h"
#include "constant.h"
#include "varbuffer.h"
#include "subroutine.h"

#define max(x, y) x > y ? x : y

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
Shader::Shader() :
	codeOffset(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Shader::~Shader()
{
	// empty
}
//------------------------------------------------------------------------------
/**
*/
void
Shader::SetHeader( const Header& header )
{
    int major = header.GetMajor();
    Header::Type type = header.GetType();
    if (type == Header::GLSL)
    {
        if (major == 4)
        {
            this->target = GLSL4;
        }
        else if (major == 3)
        {
            this->target = GLSL3;
        }
        else if (major == 2)
        {
            this->target = GLSL2;
        }
        else if (major == 1)
        {
            this->target = GLSL1;
		}
    }
    else if (type == Header::HLSL)
    {
        if (major == 5)
        {
            this->target = HLSL5;
        }
        else if (major == 4)
        {
            this->target = HLSL4;
        }
        else if (major == 3)
        {
            this->target = HLSL3;
        }
    }
    else if (type == Header::Wii)
    {
        if (major == 1)
        {
            this->target = WII;
        }
        else if (major == 2)
        {
            this->target = WIIU;
        }
    }
    else if (type == Header::PS)
    {
        if (major == 1)
        {
            this->target = PS3;
        }
        else if (major == 2)
        {
            this->target = PS4;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
Shader::Setup()
{
	// get line and row from function
	this->line = this->func.GetLine();
	this->row = this->func.GetPosition();
	this->file = this->func.GetFile();

	// inform all parameters which shader they belong to
	const unsigned numParams = this->func.GetNumParameters();
	unsigned i;
	for (i = 0; i < numParams; i++)
	{
		Parameter* param = this->func.GetParameter(i);
		param->SetShader(this);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
Shader::Generate( 
				 Generator& generator, 
				 const std::vector<Variable>& vars, 
				 const std::vector<Structure>& structures, 
				 const std::vector<Constant>& constants,
				 const std::vector<VarBlock>& blocks,
                 const std::vector<VarBuffer>& buffers,
                 const std::vector<Subroutine>& subroutines,
				 const std::vector<Function>& functions)
{
	// clear formatted code
	this->preamble.clear();

	// get header
	const Header& header = generator.GetHeader();

	if (header.GetType() == Header::GLSL)
	{
		std::string version = Format("#version %d%d%d\n", header.GetMajor(), header.GetMinor(), header.GetMinor() > 10 ? header.GetMinor() % 10 : 0);
		this->preamble.append(version);
	}

    this->preamble.append("#extension GL_ARB_bindless_texture : require\n");

	// this list holds a couple of defines which are inserted into the preamble of the code in order to be able to separate functions depending on shader type
	const std::string shaderDefines[] =
	{
		"#define VERTEX_SHADER\n\n",
		"#define FRAGMENT_SHADER\n\n",
		"#define GEOMETRY_SHADER\n\n",
		"#define HULL_SHADER\n\n",
		"#define DOMAIN_SHADER\n\n",
		"#define COMPUTE_SHADER\n\n"
	};
	this->preamble.append(shaderDefines[this->shaderType]);

    // add compile flags
    std::string tempFlags = this->compileFlags;
    if (tempFlags.length() > 0)
    {
        std::string token;
        size_t index = 0;
        while ((index = tempFlags.find("|")) != std::string::npos)
        {
            token = tempFlags.substr(0, index);
            tempFlags.erase(0, index + 1);
            this->preamble.append("#define " + token + "\n");
        }

        // fugly solution, but adds the last define
        this->preamble.append("#define " + tempFlags + "\n");
    }

	// undefine functions which GL will complain about when compiling for certain shader targets (likely they won't be used at all)
	if (this->shaderType != ProgramRow::PixelShader)
	{
		this->preamble.append("#define dFdx(val) val\n");
		this->preamble.append("#define dFdy(val) val\n");
		this->preamble.append("#define fwidth(val) val\n");
	}

	unsigned i;
	for (i = 0; i < structures.size(); i++)
	{
		const Structure& structure = structures[i];
		this->preamble.append(structure.Format(header));
	}

	for (i = 0; i < vars.size(); i++)
	{
		const Variable& var = vars[i];

		if (!var.IsSubroutine())
		{
			// variable is formatted by resolving the internal type to the target type
			this->preamble.append(var.Format(header));
		}		
	}

	for (i = 0; i < blocks.size(); i++)
	{
		const VarBlock& block = blocks[i];
		this->preamble.append(block.Format(header, i));
	}

    for (i = 0; i < buffers.size(); i++)
    {
        const VarBuffer& buffer = buffers[i];
        this->preamble.append(buffer.Format(header, i));
    }

	for (i = 0; i < constants.size(); i++)
	{
		const Constant& constant = constants[i];
		this->preamble.append(constant.Format(header));
	}

	for (i = 0; i < functions.size(); i++)
	{
		const Function& func = functions[i];
		this->preamble.append(func.GetCode());
        this->indexToFileMap[func.GetFileIndex()] = std::pair<std::string, std::string>(func.GetName(), func.GetFile());
	}

	for (i = 0; i < subroutines.size(); i++)
	{
		const Subroutine& subroutine = subroutines[i];
		this->preamble.append(subroutine.Format(header));
		this->indexToFileMap[subroutine.GetFileIndex()] = std::pair<std::string, std::string>(subroutine.GetName(), subroutine.GetFile());
	}

	for (i = 0; i < vars.size(); i++)
	{
		const Variable& var = vars[i];

		if (var.IsSubroutine())
		{
			// generate subroutine vars
			this->preamble.append(var.Format(header));
		}
	}

	// now generate target language specifics
	switch (this->target)
	{
	case GLSL1:
	case GLSL2:
	case GLSL3:
		this->GenerateGLSL3(generator);
		break;
	case GLSL4:
		this->GenerateGLSL4(generator);
		break;
	case HLSL3:
		this->GenerateHLSL3(generator);
		break;
	case HLSL4:
		this->GenerateHLSL4(generator);
		break;
	case HLSL5:
		this->GenerateHLSL5(generator);
		break;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Shader::GenerateGLSL4(Generator& generator)
{
	std::string code;

	// get header type
	const Header& header = generator.GetHeader();

	// write function attributes
	if (this->shaderType == ProgramRow::VertexShader)
	{
		// type checker should throw warning/error if we have an attribute
	}
	else if (this->shaderType == ProgramRow::PixelShader)
	{
		if (this->func.HasBoolFlag(FunctionAttribute::EarlyDepth))
		{
			code.append("layout(early_fragment_tests) in;\n");
		}
	}
	else if (this->shaderType == ProgramRow::HullShader)
	{
		bool hasOutputSize = this->func.HasIntFlag(FunctionAttribute::OutputVertices);
		if (!hasOutputSize)
		{
			std::string err = Format("Hull shader '%s' requires [outputvertices] to be defined, %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			generator.Error(err);
		}

		int outputSize = this->func.GetIntFlag(FunctionAttribute::OutputVertices);
		code.append("layout(vertices = " + Format("%d", outputSize) + ") out;\n");
	}
	else if (this->shaderType == ProgramRow::DomainShader)
	{
		bool hasVertexCount = this->func.HasIntFlag(FunctionAttribute::InputVertices);
		bool hasInputTopology = this->func.HasIntFlag(FunctionAttribute::Topology);
		if (!hasVertexCount)
		{
			std::string err = Format("Domain shader '%s' requires [inputvertices] to be defined, %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			generator.Error(err);
		}
		// input topology and spacing is not optional
		if (!hasInputTopology)
		{
			std::string err = Format("Domain shader '%s' requires [topology] to be defined, %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			generator.Error(err);
		}

		int vertexCount = this->func.GetIntFlag(FunctionAttribute::InputVertices);		
		int inputTopology = this->func.GetIntFlag(FunctionAttribute::Topology);
		bool hasSpacing = this->func.HasIntFlag(FunctionAttribute::PartitionMethod);
		bool hasWindingOrder = this->func.HasIntFlag(FunctionAttribute::WindingOrder);
		
		code.append("layout(");

		// write topology
		switch (inputTopology)
		{
		case 0:			// isolines
			code.append("triangles");
			break;
		case 1:			// triangles
			code.append("quads");
			break;
		case 2:			// quads
			code.append("isolines");
			break;
		case 3:			// force points
			code.append("point_mode");
			break;
		}

		if (hasSpacing)
		{
			// add comma for spacing method
			code.append(", ");
			int spacing = this->func.GetIntFlag(FunctionAttribute::PartitionMethod);

			switch (spacing)
			{
			case 0:
				code.append("equal_spacing");
				break;
			case 1:
				code.append("fractional_even_spacing");
				break;
			case 2:
				code.append("fractional_odd_spacing");
				break;
			case 3:
				{
					std::string message = Format("Tessellation Evaluation Shader '%s' does not define partitioning method 'pow' for GLSL4, %d:%d\n", this->name.c_str(), this->line, this->row);
					generator.Error(message);
					break;
				}
			}
		}

		if (hasWindingOrder)
		{
			// add comma for winding order
			code.append(", ");
			int winding = this->func.GetIntFlag(FunctionAttribute::WindingOrder);

			switch (winding)
			{
			case 0:
				code.append("cw");
				break;
			case 1:
				code.append("ccw");
				break;
			}
		}

		// write end of evaluation shader attributes
		code.append(") in;\n");
	}
	else if (this->shaderType == ProgramRow::GeometryShader)
	{
		bool hasInput = this->func.HasIntFlag(FunctionAttribute::InputPrimitive);
		bool hasOutput = this->func.HasIntFlag(FunctionAttribute::OutputPrimitive);
		bool hasMaxVerts = this->func.HasIntFlag(FunctionAttribute::MaxVertexCount);
		bool hasInstances = this->func.HasIntFlag(FunctionAttribute::Instances);

		if (!hasInput)
		{
			return;
		}
		if (!hasOutput)
		{
			return;
		}
		if (!hasMaxVerts)
		{
			return;
		}

		// write input primitive type
		{
			int type = this->func.GetIntFlag(FunctionAttribute::InputPrimitive);
			std::string inLayout;

			switch (type)
			{
			case 0:			// points
				inLayout.append("points");
				break;
			case 1:			// lines
				inLayout.append("lines");
				break;
			case 2:			// lines_adjacency
				inLayout.append("lines_adjacency");
				break;
			case 3:			// triangles
				inLayout.append("triangles");
				break;
			case 4:			// triangles_adjacency
				inLayout.append("triangles_adjacency");
				break;
			}

			// append instances if required
			if (hasInstances)
			{
				inLayout.append(AnyFX::Format(", invocations = %d", this->func.GetIntFlag(FunctionAttribute::Instances)));
			}
			code.append(AnyFX::Format("layout(%s) in;\n", inLayout.c_str()));
		}

		// write output primitive type
		{
			int type = this->func.GetIntFlag(FunctionAttribute::OutputPrimitive);
			int maxVerts = this->func.GetIntFlag(FunctionAttribute::MaxVertexCount);
			switch (type)
			{
			case 0:			// points
				code.append("layout(points, max_vertices = " + Format("%d", maxVerts) + ") out;\n");
				break;
			case 1:			// line_strip
				code.append("layout(line_strip, max_vertices = " + Format("%d", maxVerts) + ") out;\n");
				break;
			case 2:			// triangle_strip
				code.append("layout(triangle_strip, max_vertices = " + Format("%d", maxVerts) + ") out;\n");
				break;
			}
		}
	}
	else if (this->shaderType == ProgramRow::ComputeShader)
	{
		bool hasLocalX = this->func.HasIntFlag(FunctionAttribute::LocalSizeX);
		bool hasLocalY = this->func.HasIntFlag(FunctionAttribute::LocalSizeY);
		bool hasLocalZ = this->func.HasIntFlag(FunctionAttribute::LocalSizeZ);

		if (hasLocalX || hasLocalY || hasLocalZ)
		{
			code.append("layout(local_size_x = ");
			if (hasLocalX)
			{
				std::string number = AnyFX::Format("%d", this->func.GetIntFlag(FunctionAttribute::LocalSizeX));
				code.append(number);
			}
			else
			{
				code.append("1");
			}

			code.append(", ");
			code.append("local_size_y = ");

			if (hasLocalY)
			{	
				std::string number = AnyFX::Format("%d", this->func.GetIntFlag(FunctionAttribute::LocalSizeY));
				code.append(number);
			}
			else
			{
				code.append("1");
			}

			code.append(", ");
			code.append("local_size_z = ");

			if (hasLocalZ)
			{
				std::string number = AnyFX::Format("%d", this->func.GetIntFlag(FunctionAttribute::LocalSizeZ));
				code.append(number);
			}
			else
			{
				code.append("1");
			}
			code.append(") in;\n");
		}
	}

	unsigned input, output;
	input = output = 0;

	unsigned i;
	const unsigned numParams = this->func.GetNumParameters();
	for (i = 0; i < numParams; i++)
	{
		const Parameter* param = this->func.GetParameter(i);

		// format parameter and add it to the code
		code.append(param->Format(header, input, output));
	}

	// add function header
	std::string returnType = DataType::ToProfileType(this->func.GetReturnType(), header.GetType());
	code.append(returnType);
	code.append("\nmain()\n{\n");
	std::string line = Format("#line %d %d\n", this->func.GetCodeLine(), this->indexToFileMap.size() + 1);
	code.append(line);
	code.append(func.GetCode());
	code.append("\n}\n");

	// if we don't have subroutines, find and replace names of subroutines with generated functions
	if (header.GetFlags() & Header::NoSubroutines)
	{
		std::map<std::string, std::string>::const_iterator it;
		for (it = this->subroutineMappings.begin(); it != this->subroutineMappings.end(); it++)
		{
			const std::string& find = (*it).first;
			const std::string& replace = (*it).second;
			while (code.find(find) != std::string::npos)
			{
				size_t start = code.find(find);
				code.replace(start, find.length(), replace);
			}
		}
	}

	// set formatted code as the code we just generated
	this->formattedCode = code;
	
	// start compilation
	bool compilationSuccess = false;

	// this seems a bit weird, we attempt to compile when we perform type checking
	// however, we only perform a test compilation just to see if the formatted GLSL code is syntactically correct
	const GLenum shaderTable[] = 
	{
		GL_VERTEX_SHADER,
		GL_FRAGMENT_SHADER,
		GL_GEOMETRY_SHADER,			// only accepted in GLSL3+
		GL_TESS_CONTROL_SHADER,		// only accepted in GLSL4+
		GL_TESS_EVALUATION_SHADER,	// only accepted in GLSL4+
		GL_COMPUTE_SHADER
	};

	// create temporary shader object
	GLenum shader = glCreateShader(shaderTable[this->shaderType]);

	// create array of strings
	GLint* lengths = new GLint[2];
	const GLchar** sources = new const GLchar*[2];

	// the preamble part of the code should be the responsibility of AnyFX to ALWAYS get right
	// the rest of the code patches are up to the programmer
	sources[0] = this->preamble.c_str();
	lengths[0] = this->preamble.length();
		
	// add the shader source lastly, as such, everything before should be already defined
	sources[1] = this->formattedCode.c_str();
	lengths[1] = this->formattedCode.length();

	// put source in shader
	glShaderSource(shader, 2, sources, lengths);

	// now compile
	glCompileShader(shader);

	// if there was an error, get it
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	GLint errorSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorSize);

	if (errorSize > 0 && status != GL_TRUE)
	{
		// get error log
		GLchar* errorLog = new GLchar[errorSize];
		glGetShaderInfoLog(shader, errorSize, NULL, errorLog);

		// create string from error log
		std::string errorString(errorLog, errorSize);

		// now format errors and warnings to have correct line positions
		std::stringstream stream(errorString);

		// get vendor string
		std::string vendor = (const char*)glGetString(GL_VENDOR);

		// since different compilers handle error reporting differently, test for common strings and attempt to decode and output message accordingly
		if (vendor.find("ATI") != vendor.npos ||
			vendor.find("Intel") != vendor.npos)
		{
			this->GLSLProblemIntelATI(generator, stream);
		}
		else if (vendor.find("NVIDIA") != vendor.npos)
		{
			this->GLSLProblemNvidia(generator, stream);
		}
		else
		{
			// no known GPU vendor, just output raw string
			Emit(errorString.c_str());
		}
	}

	// delete shader, we don't want it hogging any more memory now do we!
	glDeleteShader(shader);

	// merge code
	this->formattedCode = this->preamble + this->formattedCode;
}

//------------------------------------------------------------------------------
/**
	Generates GLSL3 target language code
*/
void 
Shader::GenerateGLSL3(Generator& generator)
{
	std::string message = Format("GLSL3 code generator is not implemented yet!\n");
	generator.Error(message);
	// IMPLEMENT ME!
}

//------------------------------------------------------------------------------
/**
	Generates HLSL5 target language code
*/
void 
Shader::GenerateHLSL5(Generator& generator)
{
	std::string message = Format("HLSL5 code generator is not implemented yet!\n");
	generator.Error(message);

	// IMPLEMENT ME!
}

//------------------------------------------------------------------------------
/**
	Generates HLSL4 target language code
*/
void 
Shader::GenerateHLSL4(Generator& generator)
{
	std::string message = Format("HLSL4 code generator is not implemented yet!\n");
	generator.Error(message);

	// IMPLEMENT ME!
}

//------------------------------------------------------------------------------
/**
	Generates HLSL3 target language code
*/
void 
Shader::GenerateHLSL3(Generator& generator)
{
	std::string message = Format("HLSL3 code generator is not implemented yet!\n");
	generator.Error(message);

	// IMPLEMENT ME!
}

//------------------------------------------------------------------------------
/**
*/
void
Shader::Compile(BinWriter& writer)
{
	writer.WriteInt(this->shaderType);
	writer.WriteString(this->name);
	writer.WriteString(this->formattedCode);

    if (this->shaderType == ProgramRow::ComputeShader)
    {
        writer.WriteInt(max(1, this->func.GetIntFlag(FunctionAttribute::LocalSizeX)));
        writer.WriteInt(max(1, this->func.GetIntFlag(FunctionAttribute::LocalSizeY)));
        writer.WriteInt(max(1, this->func.GetIntFlag(FunctionAttribute::LocalSizeZ)));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Shader::TypeCheck(TypeChecker& typechecker)
{
	// type check function, this will make sure the function is properly formatted
	this->func.TypeCheck(typechecker);

	if (this->shaderType == ProgramRow::HullShader)
	{
		bool hasInputSize = this->func.HasIntFlag(FunctionAttribute::InputVertices);
		if (!hasInputSize)
		{
			std::string message = Format("Tessellation Control Shader: '%s' needs to define [inputvertices], %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Error(message);
		}

		bool hasOutputSize = this->func.HasIntFlag(FunctionAttribute::OutputVertices);
		if (!hasOutputSize)
		{
			std::string message = Format("Tessellation Control Shader: '%s' needs to define [outputvertices], %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Error(message);
		}
	}
	else if (this->shaderType == ProgramRow::DomainShader)
	{
		bool hasInputVertices = this->func.HasIntFlag(FunctionAttribute::InputVertices);
		bool hasInputTopology = this->func.HasIntFlag(FunctionAttribute::Topology);
		if (!hasInputVertices)
		{
			std::string message = Format("Tessellation Evaluation Shader: '%s' needs to define [inputvertices], %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Error(message);
		}		
		
		// input topology and spacing is not optional
		if (!hasInputTopology)
		{
			std::string message = Format("Tessellation Evaluation Shader '%s' needs to define [topology], %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Error(message);
		}
	}
	else if (this->shaderType == ProgramRow::GeometryShader)
	{
		bool hasInput = this->func.HasIntFlag(FunctionAttribute::InputPrimitive);
		bool hasOutput = this->func.HasIntFlag(FunctionAttribute::OutputPrimitive);
		bool hasMaxVerts = this->func.HasIntFlag(FunctionAttribute::MaxVertexCount);

		if (!hasInput)
		{
			std::string message = Format("Geometry Shader '%s' needs to define [inputprimitive], %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Error(message);
		}
		if (!hasOutput)
		{
			std::string message = Format("Geometry Shader '%s' needs to define [outputprimitive], %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Error(message);
		}
		if (!hasMaxVerts)
		{
			std::string message = Format("Geometry Shader '%s' needs to define [maxvertexcount], %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Error(message);
		}
	}
	else if (shaderType == ProgramRow::ComputeShader)
	{
		bool hasLocalX = this->func.HasIntFlag(FunctionAttribute::LocalSizeX);
		bool hasLocalY = this->func.HasIntFlag(FunctionAttribute::LocalSizeY);
		bool hasLocalZ = this->func.HasIntFlag(FunctionAttribute::LocalSizeZ);

		if (!(hasLocalX || hasLocalY || hasLocalZ))
		{
			std::string message = Format("Compute shader '%s' doesn't define any local size, %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.LinkError(message);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Shader::GLSLProblemIntelATI(Generator& generator, std::stringstream& stream)
{
	while (!stream.eof())
	{
		std::string line;
		std::getline(stream, line);

		if (line.length() == 0) continue;

		char* data = new char[line.size()+1];
		strcpy(data, line.c_str());

		char* errorMsg = strstr(data, "ERROR: ");
		char* warningMsg = strstr(data, "WARNING: ");

		// the error log can contain either warning or error
		// in some cases we may also have problem padding, but we throw that part away since we handle it ourselves
		if (errorMsg)
		{
			char* lineRow = errorMsg + sizeof("ERROR:");
			char* file = strtok(lineRow, ":");
			char* line = strtok(NULL, ":");
			if (line)
			{
				lineRow = strtok(NULL, "\n");
				int fileValue = atoi(file);

				// first file is the preamble
				if (fileValue == 0)
				{
					int lineValue = atoi(line);
                    std::string msg = Format("OpenGL error: %s at row %d in file %s.\n", lineRow, lineValue, this->func.GetFile().c_str());
					generator.Error(msg);
				}
                else if (this->indexToFileMap.find(fileValue) != this->indexToFileMap.end())
                {
                    int lineValue = atoi(line);
                    const std::pair<std::string, std::string>& func = this->indexToFileMap[fileValue];
                    std::string msg = Format("OpenGL warning in function '%s' at row %d:%s in file %s.\n", func.first.c_str(), lineValue, lineRow, func.second.c_str());
                    generator.Error(msg);
                }
				else
				{
					int lineValue = atoi(line);
					std::string msg = Format("OpenGL error: shader '%s' at row %d:%s in file %s.\n", this->name.c_str(), lineValue, lineRow, this->file.c_str());
					generator.Error(msg);
				}					
			}
		}
		else if (warningMsg)
		{
			char* lineRow = warningMsg + sizeof("WARNING:");
			char* file = strtok(lineRow, ":");
			char* line = strtok(NULL, ":");
			if (line)
			{
				lineRow = strtok(NULL, "\n");
				int fileValue = atoi(file);

				// first file is the preamble
				if (fileValue == 0)
				{
					int lineValue = atoi(line);
                    std::string msg = Format("OpenGL warning: %s at row %d in file %s.\n", lineRow, lineValue, this->func.GetFile().c_str());
					generator.Warning(msg);
				}
                else if (this->indexToFileMap.find(fileValue) != this->indexToFileMap.end())
                {
                    int lineValue = atoi(line);
                    const std::pair<std::string, std::string>& func = this->indexToFileMap[fileValue];
                    std::string msg = Format("OpenGL warning in function '%s' at row %d:%s in file %s.\n", func.first.c_str(), lineValue, lineRow, func.second.c_str());
                    generator.Warning(msg);
                }
				else
				{
					int lineValue = atoi(line);
					std::string msg = Format("OpenGL warning: shader '%s' at row %d:%s in file %s.\n", this->name.c_str(), lineValue, lineRow, this->file.c_str());
					generator.Warning(msg);
				}				
			}
		}
        else
        {
            generator.Error(line);
        }
        delete [] data;
	}		
}

//------------------------------------------------------------------------------
/**
*/
void
Shader::GLSLProblemNvidia(Generator& generator, std::stringstream& stream)
{
	while (!stream.eof())
	{
		std::string line;
		std::getline(stream, line);

		if (line.length() == 0) continue;

		char* data = new char[line.size()+1];
		strcpy(data, line.c_str());

		char* errorMsg = strstr(data, "error");
		char* warningMsg = strstr(data, "warning");		

		// we can either have an error or a warning, in case a row in the error report contains either
		// in some cases we may also have problem padding, but we throw that part away since we handle it ourselves
		if (errorMsg)
		{
			int fileNumber = atoi(strtok(data, "("));
			int lineNumber = atoi(strtok(NULL, ")"));

			char* lineRow = errorMsg + sizeof("error");
			lineRow = strtok(lineRow, "\n");
			if (fileNumber == 0)
			{
                std::string msg = Format("OpenGL error: %s at row %d in file %s.\n", lineRow, lineNumber, this->func.GetFile().c_str());
				generator.Error(msg);
			}
            else if (this->indexToFileMap.find(fileNumber) != this->indexToFileMap.end())
            {
                const std::pair<std::string, std::string>& func = this->indexToFileMap[fileNumber];
                std::string msg = Format("OpenGL error in function '%s' at row %d:%s in file %s.\n", func.first.c_str(), lineNumber, lineRow, func.second.c_str());
                generator.Error(msg);
            }
			else
			{
				std::string msg = Format("OpenGL error: shader '%s' at row %d, %s in file %s.\n", this->name.c_str(), lineNumber, lineRow, this->file.c_str());
				generator.Error(msg);
			}

		}
		else if (warningMsg)
		{
			int fileNumber = atoi(strtok(data, "("));
			int lineNumber = atoi(strtok(NULL, ")"));
			char* lineRow = warningMsg + sizeof("warning");
			lineRow = strtok(lineRow, "\n");
			if (fileNumber == 0)
			{
                std::string msg = Format("OpenGL warning: %s at row %d in file %s.\n", lineRow, lineNumber, this->func.GetFile().c_str());
				generator.Warning(msg);
			}
            else if (this->indexToFileMap.find(fileNumber) != this->indexToFileMap.end())
            {
                const std::pair<std::string, std::string>& func = this->indexToFileMap[fileNumber];
                std::string msg = Format("OpenGL warning in function '%s' at row %d:%s in file %s.\n", func.first.c_str(), lineNumber, lineRow, func.second.c_str());
                generator.Warning(msg);
            }            
			else
			{
				std::string msg = Format("OpenGL warning: shader '%s' at row %d, %s in file %s.\n", this->name.c_str(), lineNumber, lineRow, this->file.c_str());
				generator.Warning(msg);
			}	
		}
        else
        {
            generator.Error(line);
        }
        delete [] data;
	}
}

} // namespace AnyFX
