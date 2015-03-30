//------------------------------------------------------------------------------
//  subroutine.cc
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "subroutine.h"
#include "datatype.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
Subroutine::Subroutine() :
    fileIndex(-1)
{
    this->symbolType = Symbol::SubroutineType;
}

//------------------------------------------------------------------------------
/**
*/
Subroutine::~Subroutine()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
Subroutine::TypeCheck( TypeChecker& typechecker )
{
    // add variable, if failed we must have a redefinition
    if (!typechecker.AddSymbol(this)) return;

    if (this->subroutineType == Implementation)
    {
        if (this->signature.length() == 0)
        {
            std::string message = AnyFX::Format("Subroutine signature must not be empty, %s\n", this->ErrorSuffix().c_str());
            typechecker.Error(message);
        }

        // get subroutine signature
        Subroutine* signatureRoutine = (Subroutine*)typechecker.GetSymbol(this->signature);
        if (signatureRoutine->GetType() != Symbol::SubroutineType)
        {
            std::string message = AnyFX::Format("Symbol '%s' must be a subroutine to be used as a signature, %s\n", this->signature.c_str(), this->ErrorSuffix().c_str());
            typechecker.Error(message);

            // actually a fatal error, we cannot continue from here
            return;
        }
        if (signatureRoutine->GetSubroutineType() != Signature)
        {
            std::string message = AnyFX::Format("Prototype signature must not be the name of a formatted subroutine, %s\n", this->ErrorSuffix().c_str());
            typechecker.Error(message);
        }

        // make sure our return type matches that of the signature
        if (this->func.GetReturnType() != signatureRoutine->GetReturnType())
        {
            std::string message = AnyFX::Format("Subroutine implementation doesn't match the return type from signature, %s\n", this->ErrorSuffix().c_str());
            typechecker.Error(message);
        }

        if (signatureRoutine->GetNumParameters() != this->func.GetNumParameters())
        {
            std::string message = AnyFX::Format("Subroutine implementation and prototype does not have a matching amount of parameters, got %d expected %d, %s\n", this->func.GetNumParameters(), signatureRoutine->GetNumParameters(), this->ErrorSuffix().c_str());
            typechecker.Error(message);
        }
        else
        {  
            unsigned numParams = signatureRoutine->GetNumParameters();
            unsigned i;
            for (i = 0; i < numParams; i++)
            {
                const Parameter* param = signatureRoutine->GetParameter(i);
                if (param->GetDataType() != this->func.GetParameter(i)->GetDataType())
                {
                    std::string message = AnyFX::Format("Subroutine implementation doesn't match prototype signature, argument %d is of type '%s', expected '%s', %s\n", i, DataType::ToString(this->func.GetParameter(i)->GetDataType()).c_str(), DataType::ToString(param->GetDataType()).c_str(), this->ErrorSuffix().c_str());
                    typechecker.Error(message);
                }
                else
                {
                    const std::string& code = this->func.GetCode();
                    if (code.find(this->func.GetParameter(i)->GetName()) != std::string::npos)
                    {
                        signatureRoutine->ParameterUsed(i);
                    }
                }
            }
        }
    }
    else
    {
        if (this->returnType.GetType() == DataType::UserType)
        {
            if (!typechecker.HasSymbol(this->returnType.GetName()))
            {
                std::string message = AnyFX::Format("Subroutine prototype '%s' has an undefined return type '%s', %s\n", this->GetName().c_str(), this->returnType.GetName().c_str(), this->ErrorSuffix().c_str());
                typechecker.Error(message);
            }            
        }

        // setup parameter usage list
        this->optimizationList.resize(this->parameters.size(), false);
    }

    // type check parameters
    unsigned i;
    for (i = 0; i < this->parameters.size(); i++)
    {
        this->parameters[i].TypeCheck(typechecker);		
    }

	// type check function
	for (i = 0; i < this->func.parameters.size(); i++)
	{
		this->func.parameters[i].TypeCheck(typechecker);
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
Subroutine::Compile( BinWriter& writer )
{
    writer.WriteString(this->name);
    writer.WriteInt(this->subroutineType);
}

//------------------------------------------------------------------------------
/**
*/
void 
Subroutine::CheckForOptimization( TypeChecker& typeChecker )
{
    if (this->subroutineType == Subroutine::Signature)
    {
        unsigned i;
        for (i = 0; i < this->optimizationList.size(); i++)
        {
            if (optimizationList[i] == false)
            {
                std::string message = AnyFX::Format("Parameter '%s' is not used by any subroutine implementation, this may cause compilation errors, %s\n", this->parameters[i].GetName().c_str(), this->ErrorSuffix().c_str());
                typeChecker.Warning(message);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
std::string 
Subroutine::Format( const Header& header ) const
{
    std::string formattedCode;

    if (header.GetType() == Header::GLSL)
    {
        if (this->subroutineType == Subroutine::Signature)
        {
            formattedCode.append("subroutine ");
            formattedCode.append(DataType::ToProfileType(this->returnType, header.GetType()));
            formattedCode.append(" ");
            formattedCode.append(this->name);
            formattedCode.append("(");

            unsigned i;
            for (i = 0; i < this->parameters.size(); i++)
            {
                const Parameter& param = this->parameters[i];
                if (param.GetIO() == Parameter::Input) formattedCode.append("in ");
                else if (param.GetIO() == Parameter::Output) formattedCode.append("out ");
				else if (param.GetIO() == Parameter::InputOutput) formattedCode.append("inout ");
                formattedCode.append(DataType::ToProfileType(param.GetDataType(), header.GetType()));
                formattedCode.append(" ");
                formattedCode.append(param.GetName());

                if (i < this->parameters.size()-1)
                {
                    formattedCode.append(", ");
                }
            }

            formattedCode.append(");\n");
        }
        else if (this->subroutineType == Subroutine::Implementation)
        {
            formattedCode.append(AnyFX::Format("#line %d %d\n", this->GetLine(), this->fileIndex));
            formattedCode.append("subroutine");
            formattedCode.append(" (");
            formattedCode.append(this->signature);
            formattedCode.append(") ");
            formattedCode.append(this->func.GetCode());
        }
    }

    return formattedCode;
}

//------------------------------------------------------------------------------
/**
*/
void 
Subroutine::UpdateCode( const Header& header, unsigned fileIndex )
{
    // format function code with file index as input
    this->fileIndex = fileIndex;
    this->func.Restore(header, fileIndex);
}

//------------------------------------------------------------------------------
/**
*/
void 
Subroutine::ParameterUsed( unsigned i )
{
    if (i < this->optimizationList.size())
    {
        this->optimizationList[i] = true;
    }
}
} // namespace AnyFX