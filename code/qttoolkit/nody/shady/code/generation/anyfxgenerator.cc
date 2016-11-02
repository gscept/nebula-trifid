//------------------------------------------------------------------------------
//  anyfxgenerator.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "anyfxgenerator.h"
#include "scene/nodescene.h"
#include "node/node.h"
#include "link/link.h"
#include "variable/shadyvariable.h"
#include "timing/calendartime.h"
#include "variation/shadyvariation.h"
#include "variable/variableinstance.h"
#include "variation/shadysupervariation.h"
#include "node/shadynode.h"
#include "shadywindow.h"
#include "io/ioserver.h"
#include "anyfx/compiler/code/afxcompiler.h"
#include "nody_config.h"

#include <QApplication>

using namespace Nody;
namespace Shady
{
__ImplementClass(Shady::AnyFXGenerator, 'AFXG', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
AnyFXGenerator::AnyFXGenerator()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AnyFXGenerator::~AnyFXGenerator()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
AnyFXGenerator::GenerateToFile(const Ptr<NodeScene>& scene, const IO::URI& path)
{
    n_assert(scene.isvalid());

    // do actual work
    Util::String code = this->DoWork(scene);
    
    // write to file
    Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(path);
    stream->SetAccessMode(IO::Stream::WriteAccess);

    if (stream->Open())
    {
        // write to stream and close
        stream->Write(code.AsCharPtr(), code.Length());
        stream->Close();
    }
    else
    {
        SHADY_ERROR_FORMAT("Path '%s' is not valid", path.LocalPath().AsCharPtr());
		this->error = true;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AnyFXGenerator::GenerateToBuffer(const Ptr<NodeScene>& scene, Util::Blob& output)
{
    n_assert(scene.isvalid());

    // do actual work
    Util::String code = this->DoWork(scene);

    // set output
	if (!code.IsEmpty()) output.Set(code.AsCharPtr(), code.Length());
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
AnyFXGenerator::DoWork(const Ptr<Nody::NodeScene>& scene)
{
    // clear local resources
    this->generatedCode.Clear();
    this->vartypeInferenceMapping.Clear();
    this->variableInstanceNameMapping.Clear();
    this->paramBlock.Clear();
    this->error = false;

    // string which will contain final code
    Util::String finalCode;

    // get main node, this will be used to get the first level links which will be used to build each function
    const Ptr<Node>& start = scene->GetSuperNode();
    const Ptr<ShadySuperVariation>& superVariation = start->GetSuperVariation()->GetOriginalSuperVariation().downcast<ShadySuperVariation>();

	// start off by validating the graph
	this->ValidateGraph(start);
	if (this->error == true)
	{
		SHADY_ERROR_FORMAT("%s", this->errorMessage.AsCharPtr());
		return finalCode;
	}

    // write header
    Timing::CalendarTime date = Timing::CalendarTime::GetLocalTime();
    Util::String intro;
    intro.Append("//------------------------------------------------------------------------------\n");
    intro.Append("// Shady generated shader\n");
    intro.Append("// Generated on " + Util::String::FromInt(date.GetDay()) + "/" + Util::String::FromInt(date.GetMonth()) + "/" + Util::String::FromInt(date.GetYear()) + "\n");
    intro.Append("//------------------------------------------------------------------------------\n\n");

    // define default program, we should be able to modify this to select either default, geometry, tessellation or geometry-tessellation
    intro.Append("#define DEFAULT_PROGRAM (1)\n");
    intro.Append("\n");
	Util::String variationDefineString = superVariation->GetDefines();
	Util::Array<Util::String> variationDefines = variationDefineString.Tokenize(";");
	IndexT i;
	for (i = 0; i < variationDefines.Size(); i++)
	{
		if (this->defines.FindIndex(variationDefines[i]) == InvalidIndex)
		{
			this->defines.Append(variationDefines[i]);
		}
	}

    // this code fragment will be generated per link
    Util::String linkCode;

    // lets go through all links in this node, they can ONLY be inputs, so we can assume each link will cause a function to be generated
    const Util::Array<Ptr<Link>>& links = start->GetLinks();
    for (i = 0; i < links.Size(); i++)
    {
        const Ptr<Link>& link = links[i];
        const Ptr<VariableInstance>& toVar = link->GetToVariable();
        const Ptr<VariableInstance>& fromVar = link->GetFromVariable();
        const Ptr<ShadyVariable>& origTo = toVar->GetOriginalVariable().downcast<ShadyVariable>();
        const Ptr<ShadyVariable>& origFrom = fromVar->GetOriginalVariable().downcast<ShadyVariable>();

        // generate code for variable
        Util::String input;

        // reset counters
        this->localVarCounter = 0;

        // add static links and variables to usage list
        this->usedLinks.Append(link);
        this->usedVariables.Append(toVar);
        this->usedVariables.Append(fromVar);

        // mark link as visiting, and breadth first traverse this subnetwork
        if (this->visualize) link->GetGraphics()->Visit();
        this->BreadthFirst(link->GetFromVariable()->GetNode());
        if (this->visualize) link->GetGraphics()->Unvisit();

        // clear visited nodes
        this->revisitedNodes.Clear();
        this->visitedNodes.Clear();
        this->usedVariables.Clear();
        this->usedLinks.Clear();

        // format the type of input being passed, note that this is assumed by the template to be the types used to pass data between shaders
        Util::String paramType;
        switch (origTo->GetResult())
        {
        case ShadyVariable::VertexResult:
            paramType = "VertexShaderParameters";
            break;
        case ShadyVariable::HullResult:
            paramType = "HullShaderParameters";
            break;
        case ShadyVariable::DomainResult:
            paramType = "DomainShaderParameters";
            break;
        case ShadyVariable::GeometryResult:
            paramType = "GeometryShaderParameters";
            break;
        case ShadyVariable::PixelResult:
            paramType = "PixelShaderParameters";
            break;
        }

        Util::String returnVar = this->lastGeneratedName;
        Nody::VarType inferredType = fromVar->GetInferredType();

        // convert to output if possible
        if (origTo->GetType() != inferredType)
        {
            if (this->CanExplicitlyConvert(inferredType, origTo->GetType()))
            {
                returnVar.Format("%s(%s)", Nody::VarType::ToString(inferredType).AsCharPtr(), returnVar.AsCharPtr());
            }
            else
            {
                this->error = true;
                this->visualize = false;
                fromVar->GetNode()->GetGraphics()->Error();
                SHADY_ERROR_FORMAT("Cannot explicitly convert from '%s' to '%s'", 
                    Nody::VarType::ToString(inferredType).AsCharPtr(), 
                    Nody::VarType::ToString(origTo->GetType()).AsCharPtr());
            }
        }

        // the last variable will be the one we should return
        this->generatedCode = this->generatedCode + "\n    return " + returnVar + ";\n";

        // format function
        input.Format("%s Get%s(%s params)\n{\n%s}\n\n", 
            VarType::ToString(origTo->GetType()).AsCharPtr(), 
            origTo->GetName().AsCharPtr(), 
            paramType.AsCharPtr(), 
            this->generatedCode.AsCharPtr());

		// append defines to code
		Util::String linkDefineString = origTo->GetDefines();
		Util::Array<Util::String> linkDefines = linkDefineString.Tokenize(";");
		IndexT j;
		for (j = 0; j < linkDefines.Size(); j++)
		{
			Util::String def = Util::String::Sprintf("#define %s\n", linkDefines[j].AsCharPtr());
			linkCode.Append(def);
		}

		// append function to full code
        linkCode.Append(input);

        // clear code
        this->generatedCode.Clear();
    }    

    // get template path
    const IO::URI& templatePath = superVariation->GetTemplate(GLSL);
    Ptr<IO::Stream> templateStream = IO::IoServer::Instance()->CreateStream(templatePath);
    Util::String temp;    
    if (templateStream->Open())
    {
        void* data = templateStream->Map();
        SizeT size = templateStream->GetSize();
        temp.AppendRange((const char*)data, size);
    }
    else
    {
        SHADY_ERROR_FORMAT("Template file %s not found!", templatePath.LocalPath().AsCharPtr());
        this->error = true;
        return "// Improper shader code";
    }

    // get header path
    const IO::URI& headerPath = superVariation->GetHeader(GLSL);
    Ptr<IO::Stream> headerStream = IO::IoServer::Instance()->CreateStream(headerPath);
    Util::String head;
    if (headerStream->Open())
    {
        void* data = headerStream->Map();
        SizeT size = headerStream->GetSize();
        head.AppendRange((const char*)data, size);
    }
    else
    {
        SHADY_ERROR_FORMAT("Header file %s not found!", headerPath.LocalPath().AsCharPtr());
        this->error = true;
        return "// Improper shader code";
    }
    
    // output defines, these must come at the very beginning
    for (i = 0; i < this->defines.Size(); i++)
    {
        Util::String define;
        define.Format("#define %s\n", this->defines[i].AsCharPtr());
        finalCode.Append(define);
    }

    // write start of code, this includes the super variation header and the introduction header seen above
    finalCode.Append(head);
    finalCode.Append(intro);

    // setup static parameter list
    Util::Array<Util::String> paramNames;

    // go through parameters and format
    finalCode.Append("// -- Start of parameter declaration section\n");
    for (i = 0; i < this->parameters.Size(); i++)
    {
        const Ptr<VariableInstance>& param = this->parameters[i];
        const Ptr<Variable>& origVar = param->GetOriginalVariable();
        const Ptr<Node>& node = param->GetNode();

        // get logic name of variable
        Util::String varName = node->GetValue(origVar->GetName() + "Name").GetString();            
        
        // add parameter, but only if it has not yet been declared
        // double declaring a static variable or setting some parameter name twice will cause an error
        if (paramNames.FindIndex(varName) == InvalidIndex)
        {
            Util::Variant value = node->GetValue(origVar->GetName());
            Util::String suffix;            
            suffix.Format(" = %s", this->ConvertFromVariant(value, origVar->GetType()).AsCharPtr());
            finalCode.Append(VarType::ToString(origVar->GetType()));
            finalCode.Append(" ");
            finalCode.Append(varName);
            if (origVar->IsAllowingInit()) finalCode.Append(suffix);
            finalCode.Append(";\n");
            paramNames.Append(varName);
        }
        else
        {
            // if variable is not static, otherwise this is fine, since the name is not user-modified
            if (origVar->GetFlags() & ~ShadyVariable::Static)
            {
                this->error = true;
                this->visualize = false;
                param->GetNode()->GetGraphics()->Error();
                SHADY_ERROR_FORMAT("Parameter %s is already declared", varName.AsCharPtr());
            }
        }
    }
    finalCode.Append("// -- End of parameter declaration section\n\n");

    // compose code, include parameters, then generated code, and finally the template
    finalCode.Append(linkCode);
    finalCode.Append(temp);

    return finalCode;
}

//------------------------------------------------------------------------------
/**
    This is where you put your validation.
    This is where you compile your code in order to make sure that it works.
*/
void
AnyFXGenerator::Validate(const Ptr<Nody::NodeScene>& scene, const Util::String& language)
{
	// begin compilation
	AnyFXBeginCompile();

	IO::IoServer* ioServer = IO::IoServer::Instance();
	IO::URI path("int:anyfx/shader.fx");
	IO::URI dest("int:anyfx/shaderbin.fx");
	std::vector<std::string> defines;
	std::vector<std::string> flags;
	Util::String define;
	define.Format("-DGLSL");
	defines.push_back(define.AsCharPtr());

	// get main node, this will be used to get the first level links which will be used to build each function
	const Ptr<Node>& start = scene->GetSuperNode();
	const Ptr<ShadySuperVariation>& superVariation = start->GetSuperVariation()->GetOriginalSuperVariation().downcast<ShadySuperVariation>();

	// get includes
	Util::Array<IO::URI> includes = superVariation->GetIncludes(language);
	IndexT i;
	for (i = 0; i < includes.Size(); i++)
	{
		define.Format("-I%s/", includes[i].LocalPath().AsCharPtr());
		defines.push_back(define.AsCharPtr());
	}

	// set flags
	flags.push_back("/NOSUB");		// deactivate subroutine usage
	flags.push_back("/GBLOCK");		// put all shader variables outside of explicit buffers in one global block

	// get target language
	Util::String target = superVariation->GetTarget();

	AnyFXErrorBlob* error;
	AnyFXCompile(path.LocalPath().AsCharPtr(), dest.LocalPath().AsCharPtr(), target.AsCharPtr(), "Khronos", defines, flags, &error);
	if (error)
	{
		Util::String buf;
		buf.Set(error->buffer, error->size);
		SHADY_ERROR_FORMAT("Compilation error: %s", buf.AsCharPtr());
		delete error;
		this->error = true;
	}

	ioServer->DeleteFile(path);
	ioServer->DeleteFile(dest);
	ioServer->DeleteDirectory("int:anyfx");

	// end compilation
	AnyFXEndCompile();
}

//------------------------------------------------------------------------------
/**
*/
void
AnyFXGenerator::VisitNode(const Ptr<Nody::Node>& node)
{
    n_assert(node.isvalid());

    // only visit node once, tag node as visited if this is the first time
    if (this->visitedNodes.FindIndex(node) == InvalidIndex) this->visitedNodes.Append(node);
    else                                                    return;

    // unmark any eventual error on the node
    node->GetGraphics()->Unerror();

    if (this->visualize)
    {
        node->GetGraphics()->Visit();
		n_qtwait(this->delay);
    }   

    // if node has unused inputs, mark node as errornous
    const Util::Array<Ptr<VariableInstance>>& inputs = node->GetVariation()->GetInputs();
    IndexT i;
    for (i = 0; i < inputs.Size(); i++)
    {
        const Ptr<VariableInstance>& input = inputs[i];
        if (!input->GetInLink().isvalid())
        {
            this->error = true;
            this->visualize = false;
            node->GetGraphics()->Error();
            SHADY_ERROR_FORMAT("Node '%s' has an unused input '%s'", node->GetVariation()->GetOriginalVariation()->GetName().AsCharPtr(), input->GetOriginalVariable()->GetName().AsCharPtr());
        }
        else
        {
            // add defines
            const Ptr<ShadyVariable>& var = input->GetOriginalVariable().downcast<ShadyVariable>();
            const Util::String& defineString = var->GetDefines();
			Util::Array<Util::String> defines = defineString.Tokenize(";");
			IndexT j;
			for (j = 0; j < defines.Size(); j++)
			{
				if (this->defines.FindIndex(defines[j]) == InvalidIndex)
				{
					this->defines.Append(defines[j]);
				}
			}
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AnyFXGenerator::VisitLink(const Ptr<Nody::Link>& link)
{
    n_assert(link.isvalid());
    if (this->visualize)
    {
        link->GetGraphics()->Visit();
		n_qtwait(this->delay);
    }    

    // add both variables as used
    this->usedVariables.Append(link->GetFromVariable());
    this->usedVariables.Append(link->GetToVariable());
    this->usedLinks.Append(link);
}

//------------------------------------------------------------------------------
/**
    Main source for code generation in AnyFX.
    When all links have been treated, we generate output names, get per-output sources and attempt implicit type conversions if possible.
*/
void
AnyFXGenerator::RevisitNode(const Ptr<Nody::Node>& node)
{
    n_assert(node.isvalid());

    // only visit node once, tag node as visited if this is the first time
    if (this->revisitedNodes.FindIndex(node) == InvalidIndex) this->revisitedNodes.Append(node);
    else                                                      return;

    if (this->visualize)
    {
        node->GetGraphics()->Unvisit();
		n_qtwait(this->delay);
    }

    // solve type by checking for inheritance
    Ptr<ShadyNode> shadyNode = node.downcast<ShadyNode>();
    Nody::VarType inferredType = shadyNode->ResolveWorkingType();

    Util::String nodeSource;

    // write outputs
    const Util::Array<Ptr<VariableInstance>>& outputs = node->GetVariation()->GetOutputs();
    IndexT i;
    for (i = 0; i < outputs.Size(); i++)
    {
        const Ptr<VariableInstance>& output = outputs[i];
        if (this->usedVariables.FindIndex(output) == InvalidIndex) continue;    // ignore this variable if it's not active in this scope

        // create variable name
        const Ptr<Variable>& origVar = output->GetOriginalVariable();

        // set inferred type if output allows it
        if (origVar->IsAllowingInference()) output->SetInferredType(inferredType);

        const uint flags = output->GetOriginalVariable()->GetFlags();
        Util::String varName;
        Util::String replacement;
        if (flags & ShadyVariable::Parameter)
        {
            // get variable name
            varName = node->GetValue(origVar->GetName() + "Name").GetString();

            // add parameter to mappings
            if (this->parameters.FindIndex(output) == InvalidIndex) this->parameters.Append(output);
        }
        else
        {
            varName = "Local" + Util::String::FromInt(this->localVarCounter++);
            Util::String prefix;
            Util::String suffix;
            if (flags & ShadyVariable::Constant)
            {
                prefix = "const ";
                Util::Variant value = node->GetValue(origVar->GetName());
                suffix.Format(" = %s", this->ConvertFromVariant(value, output->GetInferredType()).AsCharPtr());                
            }
            this->generatedCode.Append("    ");
            this->generatedCode.Append(prefix);
            this->generatedCode.Append(VarType::ToString(output->GetInferredType()).AsCharPtr());
            this->generatedCode.Append(" ");
            this->generatedCode.Append(varName);
            this->generatedCode.Append(suffix);
            this->generatedCode.Append(";\n");
        }

        const Ptr<ShadyVariable>& shadyVar = origVar.downcast<ShadyVariable>();
        Util::String outputSrc = shadyVar->GetSource(GLSL);
        if (!outputSrc.IsEmpty())
        {
            outputSrc.SubstituteString("[output]", varName);
            nodeSource.Append("    " + outputSrc + "\n");
        }        

        // add defines
        const Ptr<ShadyVariable>& var = output->GetOriginalVariable().downcast<ShadyVariable>();
        const Util::String& defineString = var->GetDefines();
		Util::Array<Util::String> defines = defineString.Tokenize(";");
		IndexT j;
		for (j = 0; j < defines.Size(); j++)
		{
			if (this->defines.FindIndex(defines[j]) == InvalidIndex)
			{
				this->defines.Append(defines[j]);
			}
		}
        
        this->variableInstanceNameMapping.Add(output, varName);
        this->lastGeneratedName = varName;
    }

    // write hidden
    const Util::Array<Ptr<VariableInstance>>& hiddens = node->GetVariation()->GetHiddens();
    for (i = 0; i < hiddens.Size(); i++)
    {
        const Ptr<VariableInstance>& hidden = hiddens[i];
        const Ptr<Variable>& origVar = hidden->GetOriginalVariable();

        // the custom name will be the name set by the user, the logic name will be the name in the source code
        Util::String customName = node->GetValue(origVar->GetName() + "Name").GetString();
        Util::String logicName = origVar->GetName();
        
        // add parameter to mappings
        if (this->parameters.FindIndex(hidden) == InvalidIndex) this->parameters.Append(hidden);

        // replace in code
        nodeSource.SubstituteString(logicName, customName);
    }

    // get links
    const Util::Array<Ptr<Link>>& links = node->GetLinks();
    for (i = 0; i < links.Size(); i++)
    {
        const Ptr<Link>& link = links[i];
        if (this->usedLinks.FindIndex(link) == InvalidIndex) continue; // ignore links unused for this ultimate output
        const Ptr<VariableInstance>& to = link->GetToVariable();
        const Ptr<VariableInstance>& from = link->GetFromVariable();

        // if type doesn't match the nodes inferred type, do an explicit conversion
        const Ptr<Variable>& origFrom = from->GetOriginalVariable();
        const Ptr<Variable>& origTo = to->GetOriginalVariable();

        const Ptr<ShadyVariable>& shadyVar = origFrom.downcast<ShadyVariable>();
        Nody::VarType fromType = from->GetInferredType();
        Nody::VarType toType = to->GetInferredType();
        Util::String varCode = shadyVar->GetSource(GLSL);

        // if its an incoming connection...
        if (to->GetNode() == node)
        {
            // handle outgoing connection
            // we compare the incoming value type with that which is inferred by the node, meaning the 'most dominant attribute'
            // this converts for example a float to float3 if the highest value is a float3 vector
            Util::String replacePattern = to->GetOriginalVariable()->GetName();
            Util::String replacement = this->variableInstanceNameMapping[from];
            if (origTo->IsAllowingInference())
            {
                if (fromType != inferredType)
                {
                    if (this->CanExplicitlyConvert(fromType, inferredType))
                    {
                        // convert to dominant type
                        replacement.Format("%s(%s)", Nody::VarType::ToString(inferredType).AsCharPtr(), replacement.AsCharPtr());
                    }
                    else if (!origTo->GetType().CanMap(fromType))
                    {
                        // we cannot explicitly convert
                        node->GetGraphics()->Error();
                        SHADY_ERROR_FORMAT("Cannot explicitly convert from %s to %s", Nody::VarType::ToString(fromType).AsCharPtr(), Nody::VarType::ToString(inferredType).AsCharPtr());
                        this->error = true;
                        this->visualize = false;
                        return;
                    }                
                }
            }
            else
            {
                if (fromType != toType)
                {
                    if (this->CanExplicitlyConvert(fromType, toType))
                    {
                        // convert to dominant type
                        replacement.Format("%s(%s)", Nody::VarType::ToString(toType).AsCharPtr(), replacement.AsCharPtr());
                    }
                    else if (!origTo->GetType().CanMap(toType))
                    {
                        // we cannot explicitly convert
                        node->GetGraphics()->Error();
                        SHADY_ERROR_FORMAT("Cannot explicitly convert from %s to %s", Nody::VarType::ToString(fromType).AsCharPtr(), Nody::VarType::ToString(toType).AsCharPtr());
                        this->error = true;
                        this->visualize = false;
                        return;
                    } 
                }
            }

            // find more primitive type and do an explicit type conversion
            nodeSource.SubstituteString(replacePattern, replacement);
        }
        else
        {  
            // in this case, we simply solve if the to-value is mappable to the type inferred by the from-value
            // meaning we simply check if the comparison is allowed
            if (!origTo->GetType().CanMap(fromType))
            {
                node->GetGraphics()->Error();
                SHADY_ERROR_FORMAT("Cannot explicitly convert from %s to %s", Nody::VarType::ToString(fromType).AsCharPtr(), Nody::VarType::ToString(origTo->GetType()).AsCharPtr());
                this->error = true;
                this->visualize = false;
                return;
            }
        }            
    }

    // add collected node source to  generated code
    if (!nodeSource.IsEmpty())
    {
        this->generatedCode += nodeSource;
    }    
}

//------------------------------------------------------------------------------
/**
*/
void
AnyFXGenerator::RevisitLink(const Ptr<Nody::Link>& link)
{
    n_assert(link.isvalid());
    if (this->visualize)
    {
        link->GetGraphics()->Unvisit();
		n_qtwait(this->delay);
    }    
}

//------------------------------------------------------------------------------
/**
*/
Util::String
AnyFXGenerator::ConvertFromVariant(const Util::Variant& value, const Nody::VarType& type)
{
    Util::String retval;
    switch (type.GetType())
    {
    case VarType::Float:
        retval.Format("%ff", value.GetFloat());
        break;
    case VarType::Float2:
        {
            const Util::Array<float>& vec = value.GetFloatArray();
            retval.Format("vec2(%ff,%ff)", vec[0], vec[1]);
            break;
        }        
    case VarType::Float3:
        {
            const Util::Array<float>& vec = value.GetFloatArray();
            retval.Format("vec3(%ff,%ff,%ff)", vec[0], vec[1], vec[2]);
            break;
        }        
    case VarType::Float4:
        {
            const Util::Array<float>& vec = value.GetFloatArray();
            retval.Format("vec4(%ff,%ff,%ff,%ff)", vec[0], vec[1], vec[2], vec[3]);
            break;
        }        
    case VarType::Int:
        retval.Format("%d", value.GetInt());
        break;
    case VarType::Int2:
        {
            const Util::Array<int>& vec = value.GetIntArray();
            retval.Format("ivec2(%d,%d)", vec[0], vec[1]);
            break;
        }        
    case VarType::Int3:
        {
            const Util::Array<int>& vec = value.GetIntArray();
            retval.Format("ivec3(%d,%d,%d)", vec[0], vec[1], vec[2]);
            break;
        }
    case VarType::Int4:
        {
            const Util::Array<int>& vec = value.GetIntArray();
            retval.Format("ivec4(%d,%d,%d,%d)", vec[0], vec[1], vec[2], vec[3]);
            break;
        }
    case VarType::Bool:
        retval = value.GetBool() ? "true" : "false";
        break;
    case VarType::Bool2:
        {
            const Util::Array<bool>& vec = value.GetBoolArray();
            retval.Format("bvec4(%s,%s)", vec[0] ? "true" : "false", vec[1] ? "true" : "false");
            break;
        }
    case VarType::Bool3:
        {
            const Util::Array<bool>& vec = value.GetBoolArray();
            retval.Format("bvec3(%d,%d,%d,%d)", vec[0] ? "true" : "false", vec[1] ? "true" : "false", vec[2] ? "true" : "false");
            break;
        }        
    case VarType::Bool4:
        {
            const Util::Array<bool>& vec = value.GetBoolArray();
            retval.Format("bvec4(%d,%d,%d,%d)", vec[0] ? "true" : "false", vec[1] ? "true" : "false", vec[2] ? "true" : "false", vec[3] ? "true" : "false");
            break;
        }
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Tests if lhs can be converted into rhs by using explicit casting, for example:

    float lhs;
    float3 rhs;
    float3 = float3(lhs) + rhs;
*/
bool
AnyFXGenerator::CanExplicitlyConvert(const Nody::VarType& lhs, const Nody::VarType& rhs)
{
    if (lhs.GetType() == rhs.GetType()) return true;
    else
    {
        uint lhsSize = Nody::VarType::VectorSize(lhs);
        uint rhsSize = Nody::VarType::VectorSize(rhs);
        if (lhsSize < rhsSize)
        {
            if (lhsSize == 1)   return true;
        }
        else if (lhsSize == rhsSize)
        {
            return true;
        }
    }
    return false;
}
} // namespace Shady
