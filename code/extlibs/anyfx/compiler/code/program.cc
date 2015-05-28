//------------------------------------------------------------------------------
//  program.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "program.h"
#include "typechecker.h"
#include "subroutine.h"
namespace AnyFX
{


//------------------------------------------------------------------------------
/**
*/
Program::Program() :
	patchSize(0),
	hasAnnotation(false)
{
	this->symbolType = Symbol::ProgramType;
	this->slotNames.resize(ProgramRow::NumProgramRows);
	this->slotNames[ProgramRow::VertexShader] = "";
	this->slotNames[ProgramRow::PixelShader] = "";
	this->slotNames[ProgramRow::HullShader] = "";
	this->slotNames[ProgramRow::DomainShader] = "";
	this->slotNames[ProgramRow::GeometryShader] = "";
	this->slotNames[ProgramRow::ComputeShader] = "";
	this->slotNames[ProgramRow::RenderState] = "placeholder";

	this->slotMask[ProgramRow::VertexShader] = false;
	this->slotMask[ProgramRow::PixelShader] = false;
	this->slotMask[ProgramRow::HullShader] = false;
	this->slotMask[ProgramRow::DomainShader] = false;
	this->slotMask[ProgramRow::GeometryShader] = false;
	this->slotMask[ProgramRow::ComputeShader] = false;
	this->slotMask[ProgramRow::RenderState] = true;
}

//------------------------------------------------------------------------------
/**
*/
Program::~Program()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
Program::ConsumeRow( const ProgramRow& row )
{
    if (row.GetFlag() == "VertexShader")		{ this->slotNames[ProgramRow::VertexShader] = row.GetString();      this->slotMask[ProgramRow::VertexShader] = true;    this->slotSubroutineMappings[ProgramRow::VertexShader] = row.GetSubroutineMappings(); }
	else if (row.GetFlag() == "PixelShader")	{ this->slotNames[ProgramRow::PixelShader] = row.GetString();       this->slotMask[ProgramRow::PixelShader] = true;     this->slotSubroutineMappings[ProgramRow::PixelShader] = row.GetSubroutineMappings(); }
	else if (row.GetFlag() == "GeometryShader")	{ this->slotNames[ProgramRow::GeometryShader] = row.GetString();    this->slotMask[ProgramRow::GeometryShader] = true;  this->slotSubroutineMappings[ProgramRow::GeometryShader] = row.GetSubroutineMappings(); }
	else if (row.GetFlag() == "HullShader")		{ this->slotNames[ProgramRow::HullShader] = row.GetString();        this->slotMask[ProgramRow::HullShader] = true;      this->slotSubroutineMappings[ProgramRow::HullShader] = row.GetSubroutineMappings(); }
	else if (row.GetFlag() == "DomainShader")	{ this->slotNames[ProgramRow::DomainShader] = row.GetString();      this->slotMask[ProgramRow::DomainShader] = true;    this->slotSubroutineMappings[ProgramRow::DomainShader] = row.GetSubroutineMappings(); }
	else if (row.GetFlag() == "ComputeShader")	{ this->slotNames[ProgramRow::ComputeShader] = row.GetString();     this->slotMask[ProgramRow::ComputeShader] = true;   this->slotSubroutineMappings[ProgramRow::ComputeShader] = row.GetSubroutineMappings(); }
	else if (row.GetFlag() == "RenderState")	{ this->slotNames[ProgramRow::RenderState] = row.GetString();       this->slotMask[ProgramRow::RenderState] = true; }
    else if (row.GetFlag() == "CompileFlags")   { this->compileFlags = row.GetString(); }
	else this->invalidFlags.push_back(row.GetFlag());
}

//------------------------------------------------------------------------------
/**
*/
void 
Program::TypeCheck( TypeChecker& typechecker )
{
	// add program, if failed we must have a redefinition
	if (!typechecker.AddSymbol(this)) return;

	// type check annotation
	if (this->hasAnnotation)
	{
		this->annotation.TypeCheck(typechecker);
	}

	// report all if any unwanted options
	unsigned i;
	for (i = 0; i < this->invalidFlags.size(); i++)
	{
		std::string msg = Format("Invalid program flag '%s', %s\n", this->invalidFlags[i].c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(msg);
	}

    // make sure that subroutine bindings are valid, traverse all possible program rows besides the render state
    for (i = 0; i < ProgramRow::NumProgramRows-1; i++)
    {
        // get mappings
        const std::map<std::string, std::string>& mappings = this->slotSubroutineMappings[i];

        // now simply check if all symbols exist!
        std::map<std::string, std::string>::const_iterator it;
        for (it = mappings.begin(); it != mappings.end(); it++)
        {
            if (!typechecker.HasSymbol((*it).first))
            {
                std::string msg = AnyFX::Format("Subroutine interface '%s' is not defined, %s\n", (*it).first.c_str(), this->ErrorSuffix().c_str());
                typechecker.Error(msg);
            }
            else
            {
                Symbol* sym = typechecker.GetSymbol((*it).first);
                if (sym->GetType() == Symbol::VariableType)
                {
                    Variable* sub = (Variable*)sym;
                    if (!sub->IsSubroutine())
                    {
                        std::string msg = AnyFX::Format("Symbol '%s' doesn't denote a subroutine prototype, %s\n", (*it).first.c_str(), this->ErrorSuffix().c_str());
                        typechecker.Error(msg);
                    }
                }
                else
                {
                    std::string msg = AnyFX::Format("Symbol '%s' is not of subroutine type, %s\n", (*it).first.c_str(), this->ErrorSuffix().c_str());
                    typechecker.Error(msg);
                }
            }

            if (!typechecker.HasSymbol((*it).second))
            {
                std::string msg = AnyFX::Format("Subroutine implementation '%s' is not defined, %s\n", (*it).second.c_str(), this->ErrorSuffix().c_str());
                typechecker.Error(msg);
            }
            else
            {
                Symbol* sym = typechecker.GetSymbol((*it).second);
                if (sym->GetType() == Symbol::SubroutineType)
                {
                    Subroutine* sub = (Subroutine*)sym;
                    if (sub->GetSubroutineType() != Subroutine::Implementation)
                    {
                        std::string msg = AnyFX::Format("Subroutine symbol '%s' must be declared as subroutine implementation, %s\n", (*it).second.c_str(), this->ErrorSuffix().c_str());
                        typechecker.Error(msg);
                    }
                }
                else
                {
                    std::string msg = AnyFX::Format("Symbol '%s' is not of subroutine type, %s\n", (*it).second.c_str(), this->ErrorSuffix().c_str());
                    typechecker.Error(msg);
                }
            }
        }
    }

    // update shader names with compile flags
    for (int i = 0; i < ProgramRow::NumProgramRows - 1; i++)
    {
        //this->slotNames[i] += "_" + this->compileFlags;
    }
	
	// get shaders
	Function* vs = typechecker.HasSymbol(this->slotNames[ProgramRow::VertexShader])		? dynamic_cast<Function*>(typechecker.GetSymbol(this->slotNames[ProgramRow::VertexShader]))		: NULL;
	Function* ps = typechecker.HasSymbol(this->slotNames[ProgramRow::PixelShader])		? dynamic_cast<Function*>(typechecker.GetSymbol(this->slotNames[ProgramRow::PixelShader]))		: NULL;
	Function* hs = typechecker.HasSymbol(this->slotNames[ProgramRow::HullShader])		? dynamic_cast<Function*>(typechecker.GetSymbol(this->slotNames[ProgramRow::HullShader]))		: NULL;
	Function* ds = typechecker.HasSymbol(this->slotNames[ProgramRow::DomainShader])		? dynamic_cast<Function*>(typechecker.GetSymbol(this->slotNames[ProgramRow::DomainShader]))		: NULL;
	Function* gs = typechecker.HasSymbol(this->slotNames[ProgramRow::GeometryShader])	? dynamic_cast<Function*>(typechecker.GetSymbol(this->slotNames[ProgramRow::GeometryShader]))	: NULL;
	Function* cs = typechecker.HasSymbol(this->slotNames[ProgramRow::ComputeShader])	? dynamic_cast<Function*>(typechecker.GetSymbol(this->slotNames[ProgramRow::ComputeShader]))	: NULL;

	RenderState* renderState = typechecker.HasSymbol(this->slotNames[ProgramRow::RenderState]) ? dynamic_cast<RenderState*>(typechecker.GetSymbol(this->slotNames[ProgramRow::RenderState]))	: NULL;
	if (!renderState)
	{
		std::string message = AnyFX::Format("RenderState '%s' undefined, %s\n", this->slotNames[ProgramRow::RenderState].c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}

	if (vs && !vs->IsShader())
	{
		std::string message = AnyFX::Format("Function '%s' is not marked as a shader, %s\n", vs->GetName().c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}
	if (ps && !ps->IsShader())
	{
		std::string message = AnyFX::Format("Function '%s' is not marked as a shader, %s\n", ps->GetName().c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}
	if (hs && !hs->IsShader())
	{
		std::string message = AnyFX::Format("Function '%s' is not marked as a shader, %s\n", hs->GetName().c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}
	if (ds && !ds->IsShader())
	{
		std::string message = AnyFX::Format("Function '%s' is not marked as a shader, %s\n", ds->GetName().c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}
	if (gs && !gs->IsShader())
	{
		std::string message = AnyFX::Format("Function '%s' is not marked as a shader, %s\n", gs->GetName().c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}
	if (cs && !cs->IsShader())
	{
		std::string message = AnyFX::Format("Function '%s' is not marked as a shader, %s\n", cs->GetName().c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}

	// first type check to see our functions truly do exist
	if (slotMask[ProgramRow::VertexShader] && vs == 0)
	{
		std::string message = Format("Vertex shader with name '%s' is not defined, %s", this->slotNames[ProgramRow::VertexShader].c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}

	if (slotMask[ProgramRow::PixelShader] && ps == 0)
	{
		std::string message = Format("Pixel shader with name '%s' is not defined, %s", this->slotNames[ProgramRow::PixelShader].c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}

	if (slotMask[ProgramRow::HullShader] && hs == 0)
	{
		std::string message = Format("Hull/Control shader with name '%s' is not defined, %s", this->slotNames[ProgramRow::HullShader].c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);	
	}

	if (slotMask[ProgramRow::DomainShader] && ds == 0)
	{
		std::string message = Format("Domain/Evaluation shader with name '%s' is not defined, %s", this->slotNames[ProgramRow::DomainShader].c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}

	if (slotMask[ProgramRow::GeometryShader] && gs == 0)
	{
		std::string message = Format("Geometry shader with name '%s' is not defined, %s", this->slotNames[ProgramRow::GeometryShader].c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}

	if (slotMask[ProgramRow::ComputeShader] && cs == 0)
	{
		std::string message = Format("Compute shader with name '%s' is not defined, %s", this->slotNames[ProgramRow::ComputeShader].c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);		
	}

	if (hs)
	{
		// get patch size from attributes
		this->patchSize = hs->GetIntFlag(FunctionAttribute::InputVertices);
	}

	if (vs && hs)
	{
		std::vector<Parameter*> outputs = vs->GetOutputParameters();
		std::vector<Parameter*> inputs = hs->GetInputParameters();

		// our outputs must be bigger than our inputs
		if (outputs.size() < inputs.size())
		{
			std::string message = Format("Input-to-Output size mismatch between Vertex '%s' (%d) and Hull/Control '%s' (%d) shaders defined in program '%s', %s\n", vs->GetName().c_str(), outputs.size(), hs->GetName().c_str(), inputs.size(), this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.LinkError(message);		
		}

		// compare signatures
		unsigned i;
		for (i = 0; i < outputs.size() && i < inputs.size(); i++)
		{
			Parameter* output = outputs[i];
			Parameter* input = inputs[i];
			if (!input->Compatible(output))
			{
				std::string outputType = DataType::ToString(output->GetDataType());
				std::string inputType = DataType::ToString(input->GetDataType());
				std::string message = Format("Cannot implicitly convert from type '%s' (%s) to type '%s' (%s) in linkage between Vertex (%s) and Hull/Control (%s) shaders defined in program '%s', %s\n", outputType.c_str(), output->GetName().c_str(), inputType.c_str(), input->GetName().c_str(), vs->GetName().c_str(), hs->GetName().c_str(), this->name.c_str(), this->ErrorSuffix().c_str());
				typechecker.LinkError(message);
			}
		}
	}

	if (hs && ds)
	{
		std::vector<Parameter*> outputs = hs->GetOutputParameters();
		std::vector<Parameter*> inputs = ds->GetInputParameters();

		unsigned outputVertices = hs->GetIntFlag(FunctionAttribute::OutputVertices);
		unsigned inputVertices = ds->GetIntFlag(FunctionAttribute::InputVertices);

		if (outputVertices != inputVertices)
		{
			std::string message = Format("Input-to-Output vertices between Hull/Control '%s' (%d) and Domain/Evaluation '%s' (%d) shaders must be equal in program '%s', %s\n", hs->GetName().c_str(), outputVertices, ds->GetName().c_str(), inputVertices, this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.LinkError(message);
		}

		// our outputs must be bigger than our inputs
		if (outputs.size() < inputs.size())
		{
			std::string message = Format("Input-to-Output size mismatch between Hull/Control '%s' (%d) and Domain/Evaluation '%s' (%d) shaders in program '%s', %s\n", hs->GetName().c_str(), outputs.size(), ds->GetName().c_str(), inputs.size(), this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.LinkError(message);
		}

		// compare signatures
		unsigned i;
		for (i = 0; i < outputs.size() && i < inputs.size(); i++)
		{
			Parameter* output = outputs[i];
			Parameter* input = inputs[i];
			if (!input->Compatible(output))
			{
				std::string outputType = DataType::ToString(output->GetDataType());
				std::string inputType = DataType::ToString(input->GetDataType());
				std::string message = Format("Cannot implicitly convert from type '%s' (%s) to type '%s' (%s) in linkage between Hull/Control (%s) and Domain/Evaluation (%s) shaders in program '%s', %s\n", outputType.c_str(), output->GetName().c_str(), inputType.c_str(), input->GetName().c_str(), hs->GetName().c_str(), ds->GetName().c_str(), this->name.c_str(), this->ErrorSuffix().c_str());
				typechecker.LinkError(message);
			}
		}
	}
	else if (hs)
	{
		// we cannot have a hull shader without a domain shader
		std::string message = Format("Definition of Hull/Control shader without a Domain/Evaluation shader is not allowed in program '%s', %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
		typechecker.LinkError(message);
	}
	else if (ds)
	{
		// just like above, but reversed
		std::string message = Format("Definition of Domain/Evaluation shader without a Hull/Control shader is not allowed in program '%s', %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
		typechecker.LinkError(message);
	}

	if (ds && gs)
	{
		std::vector<Parameter*> outputs = ds->GetOutputParameters();
		std::vector<Parameter*> inputs = gs->GetInputParameters();

		// our outputs must be bigger than our inputs
		if (outputs.size() < inputs.size())
		{
			std::string message = Format("Input-to-Output size mismatch between Domain/Evaluation '%s' (%d) and Geometry '%s' (%d) shaders in program '%s', %s\n", ds->GetName().c_str(), outputs.size(), gs->GetName().c_str(), inputs.size(), this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.LinkError(message);
		}

		// compare signatures
		unsigned i;
		for (i = 0; i < outputs.size() && i < inputs.size(); i++)
		{
			Parameter* output = outputs[i];
			Parameter* input = inputs[i];
			if (!input->Compatible(output))
			{
				std::string outputType = DataType::ToString(output->GetDataType());
				std::string inputType = DataType::ToString(input->GetDataType());
				std::string message = Format("Cannot implicitly convert from type '%s' (%s) to type '%s' (%s) in linkage between Domain/Evaluation (%s) and Geometry (%s) shaders in program '%s', %s\n", outputType.c_str(), output->GetName().c_str(), inputType.c_str(), input->GetName().c_str(), ds->GetName().c_str(), gs->GetName().c_str(), this->name.c_str(), this->ErrorSuffix().c_str());
				typechecker.LinkError(message);
			}
		}
	}
	else if (vs && gs)
	{
		std::vector<Parameter*> outputs = vs->GetOutputParameters();
		std::vector<Parameter*> inputs = gs->GetInputParameters();

		// our outputs must be bigger than our inputs
		if (outputs.size() < inputs.size())
		{
			std::string message = Format("Input-to-Output size mismatch between Vertex '%s' (%d) and Geometry '%s' (%d) shaders in program '%s', %s\n", vs->GetName().c_str(), outputs.size(), gs->GetName().c_str(), inputs.size(), this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.LinkError(message);
		}

		// compare signatures
		unsigned i;
		for (i = 0; i < outputs.size() && i < inputs.size(); i++)
		{
			Parameter* output = outputs[i];
			Parameter* input = inputs[i];
			if (!input->Compatible(output))
			{
				std::string outputType = DataType::ToString(output->GetDataType());
				std::string inputType = DataType::ToString(input->GetDataType());
				std::string message = Format("Cannot implicitly convert from type '%s' (%s) to type '%s' (%s) in linkage between Vertex (%s) and Geometry (%s) shaders in program '%s', %s\n", outputType.c_str(), output->GetName().c_str(), inputType.c_str(), input->GetName().c_str(), vs->GetName().c_str(), gs->GetName().c_str(), this->name.c_str(), this->ErrorSuffix().c_str());
				typechecker.LinkError(message);
			}
		}
	}

	if (gs && ps)
	{
		std::vector<Parameter*> outputs = gs->GetOutputParameters();
		std::vector<Parameter*> inputs = ps->GetInputParameters();

		// our outputs must be bigger than our inputs
		if (outputs.size() < inputs.size())
		{
			std::string message = Format("Input-to-Output size mismatch between Geometry '%s' (%d) and Pixel/Fragment '%s' (%d) shaders in program '%s', %s\n", gs->GetName().c_str(), outputs.size(), ps->GetName().c_str(), inputs.size(), this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.LinkError(message);
		}

		// compare signatures
		unsigned i;
		for (i = 0; i < outputs.size() && i < inputs.size(); i++)
		{
			Parameter* output = outputs[i];
			Parameter* input = inputs[i];
			if (!input->Compatible(output))
			{
				std::string outputType = DataType::ToString(output->GetDataType());
				std::string inputType = DataType::ToString(input->GetDataType());
				std::string message = Format("Cannot implicitly convert from type '%s' (%s) to type '%s' (%s) in linkage between Geometry (%s) and Pixel/Fragment (%s) shaders in program '%s', %s\n", outputType.c_str(), output->GetName().c_str(), inputType.c_str(), input->GetName().c_str(), gs->GetName().c_str(), ps->GetName().c_str(), this->name.c_str(), this->ErrorSuffix().c_str());
				typechecker.LinkError(message);
			}
		}
	}
	else if (ds && ps)
	{
		std::vector<Parameter*> outputs = ds->GetOutputParameters();
		std::vector<Parameter*> inputs = ps->GetInputParameters();

		// our outputs must be bigger than our inputs
		if (outputs.size() < inputs.size())
		{
			std::string message = Format("Input-to-Output size mismatch between Domain/Evaluation '%s' (%d) and Pixel/Fragment '%s' (%d) shaders in program '%s', %s\n", ds->GetName().c_str(), outputs.size(), ps->GetName().c_str(), inputs.size(), this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.LinkError(message);
		}

		// compare signatures
		unsigned i;
		for (i = 0; i < outputs.size() && i < inputs.size(); i++)
		{
			Parameter* output = outputs[i];
			Parameter* input = inputs[i];
			if (!input->Compatible(output))
			{
				std::string outputType = DataType::ToString(output->GetDataType());
				std::string inputType = DataType::ToString(input->GetDataType());
				std::string message = Format("Cannot implicitly convert from type '%s' (%s) to type '%s' (%s) in linkage between Domain/Evaluation (%s) and Pixel/Fragment (%s) shaders in program '%s', %s\n", outputType.c_str(), output->GetName().c_str(), inputType.c_str(), input->GetName().c_str(), ds->GetName().c_str(), ps->GetName().c_str(), this->name.c_str(), this->ErrorSuffix().c_str());
				typechecker.LinkError(message);
			}
		}
	}
	else if (vs && ps)
	{
		std::vector<Parameter*> outputs = vs->GetOutputParameters();
		std::vector<Parameter*> inputs = ps->GetInputParameters();

		// our outputs must be bigger than our inputs
		if (outputs.size() < inputs.size())
		{
			std::string message = Format("Input-to-Output size mismatch between Vertex '%s' (%d) and Pixel/Fragment '%s' (%d) shaders in program '%s', %s\n", vs->GetName().c_str(), outputs.size(), ps->GetName().c_str(), inputs.size(), this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.LinkError(message);
		}

		// compare signatures
		unsigned i;
		for (i = 0; i < outputs.size() && i < inputs.size(); i++)
		{
			Parameter* output = outputs[i];
			Parameter* input = inputs[i];
			if (!input->Compatible(output))
			{
				std::string outputType = DataType::ToString(output->GetDataType());
				std::string inputType = DataType::ToString(input->GetDataType());
				std::string message = Format("Cannot implicitly convert from type '%s' (%s) to type '%s' (%s) in linkage between Vertex (%s) and Pixel/Fragment (%s) shaders in program '%s', %s\n", outputType.c_str(), output->GetName().c_str(), inputType.c_str(), input->GetName().c_str(), vs->GetName().c_str(), ps->GetName().c_str(), this->name.c_str(), this->ErrorSuffix().c_str());
				typechecker.LinkError(message);
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
Program::Compile( BinWriter& writer )
{
	writer.WriteString(this->name);

	// write annotation if it's used
	writer.WriteBool(this->hasAnnotation);
	if (this->hasAnnotation)
	{
		this->annotation.Compile(writer);
	}	

	// write tessellation boolean and tessellation patch size
	writer.WriteBool(this->slotMask[ProgramRow::HullShader]);
	writer.WriteInt(this->patchSize);

    // write geometry shader boolean which tells us we can use transform feedbacks
    writer.WriteBool(this->slotMask[ProgramRow::GeometryShader]);

    // create iterator to iterate over subroutine mappings
    std::map<std::string, std::string>::const_iterator it;

	// write shader programs
	writer.WriteInt('VERT');
	writer.WriteString(this->slotNames[ProgramRow::VertexShader]);
    writer.WriteUInt(this->slotSubroutineMappings[ProgramRow::VertexShader].size());
    for (it = this->slotSubroutineMappings[ProgramRow::VertexShader].begin(); it != this->slotSubroutineMappings[ProgramRow::VertexShader].end(); it++)
    {
        writer.WriteString((*it).first);
        writer.WriteString((*it).second);
    }

	writer.WriteInt('PIXL');
	writer.WriteString(this->slotNames[ProgramRow::PixelShader]);
    writer.WriteUInt(this->slotSubroutineMappings[ProgramRow::PixelShader].size());
    for (it = this->slotSubroutineMappings[ProgramRow::PixelShader].begin(); it != this->slotSubroutineMappings[ProgramRow::PixelShader].end(); it++)
    {
        writer.WriteString((*it).first);
        writer.WriteString((*it).second);
    }

	writer.WriteInt('HULL');
	writer.WriteString(this->slotNames[ProgramRow::HullShader]);
    writer.WriteUInt(this->slotSubroutineMappings[ProgramRow::HullShader].size());
    for (it = this->slotSubroutineMappings[ProgramRow::HullShader].begin(); it != this->slotSubroutineMappings[ProgramRow::HullShader].end(); it++)
    {
        writer.WriteString((*it).first);
        writer.WriteString((*it).second);
    }

	writer.WriteInt('DOMA');
	writer.WriteString(this->slotNames[ProgramRow::DomainShader]);
    writer.WriteUInt(this->slotSubroutineMappings[ProgramRow::DomainShader].size());
    for (it = this->slotSubroutineMappings[ProgramRow::DomainShader].begin(); it != this->slotSubroutineMappings[ProgramRow::DomainShader].end(); it++)
    {
        writer.WriteString((*it).first);
        writer.WriteString((*it).second);
    }

	writer.WriteInt('GEOM');
	writer.WriteString(this->slotNames[ProgramRow::GeometryShader]);
    writer.WriteUInt(this->slotSubroutineMappings[ProgramRow::GeometryShader].size());
    for (it = this->slotSubroutineMappings[ProgramRow::GeometryShader].begin(); it != this->slotSubroutineMappings[ProgramRow::GeometryShader].end(); it++)
    {
        writer.WriteString((*it).first);
        writer.WriteString((*it).second);
    }

	writer.WriteInt('COMP');
	writer.WriteString(this->slotNames[ProgramRow::ComputeShader]);
    writer.WriteUInt(this->slotSubroutineMappings[ProgramRow::ComputeShader].size());
    for (it = this->slotSubroutineMappings[ProgramRow::ComputeShader].begin(); it != this->slotSubroutineMappings[ProgramRow::ComputeShader].end(); it++)
    {
        writer.WriteString((*it).first);
        writer.WriteString((*it).second);
    }

	writer.WriteInt('RSTA');
	writer.WriteString(this->slotNames[ProgramRow::RenderState]);
}

//------------------------------------------------------------------------------
/**
*/
void
Program::BuildShaders( const Header& header, const std::vector<Function>& functions, std::map<std::string, Shader*>& shaders )
{
	unsigned i;
	for (i = 0; i < ProgramRow::NumProgramRows; i++)
	{
		// first check if shader slot is used
		if (this->slotMask[i])
		{
			// get string
			const std::string& functionName = this->slotNames[i];
		
			unsigned j;
			for (j = 0; j < functions.size(); j++)
			{
				const Function& func = functions[j];

				// ok, we have a matching function
				if (func.GetName() == functionName && func.IsShader())
				{
                    // create string which is the function name merged with its compile flags
                    std::string functionNameWithDefines = functionName;// +"_" + this->compileFlags;

					// if the shader has not been created yet, create it
                    if (shaders.find(functionNameWithDefines) == shaders.end())
					{
						Shader* shader = new Shader;
						shader->SetFunction(func);
						shader->SetType(i);
                        shader->SetName(functionNameWithDefines);
						shader->SetHeader(header);
                        shader->SetCompileFlags(this->compileFlags);
                        shaders[functionNameWithDefines] = shader;
					}
				}
			}

		}
	}
}
} // namespace AnyFX
