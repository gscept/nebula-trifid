//------------------------------------------------------------------------------
//  shadynode.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadynode.h"
#include "node/graphics/shadynodegraphics.h"
#include "variation/shadyvariation.h"
#include "variable/shadyvariable.h"
#include "link/link.h"
#include "project/project.h"
#include "variable/variableinstance.h"

namespace Shady
{
__ImplementClass(Shady::ShadyNode, 'SHND', Nody::Node);

uint ShadyNode::GlobalParamCount = 0;
//------------------------------------------------------------------------------
/**
*/
ShadyNode::ShadyNode()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ShadyNode::~ShadyNode()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyNode::Setup(const Ptr<Nody::Variation>& variation)
{
    Nody::Node::Setup(variation);
    
    // go through outputs and generate values
    const Util::Array<Ptr<Nody::VariableInstance>>& outputs = this->variation->GetOutputs();
    IndexT i;
    for (i = 0; i < outputs.Size(); i++)
    {
        const Ptr<Nody::Variable>& output = outputs[i]->GetOriginalVariable();
        const uint flags = output->GetFlags();
        if (flags & ShadyVariable::Constant || flags & ShadyVariable::Parameter)
        {
            // value name is the name of the output
            Util::String valueName = output->GetName();
            Util::Variant value;
            this->SetupDefaultValue(output->GetType(), value);  
            
            // add value
            this->values.Add(valueName, value);
            this->valueTypes.Add(valueName, output->GetType());

            // also add an auto-generated name
            Util::String paramName;
            if (output->GetFlags() & ShadyVariable::Static) paramName = valueName;
			else                                            paramName = Nody::Project::RequestParameterName(outputs[i], "Param");
            Util::Variant generatedName;
            generatedName.SetString(paramName);

            // add a value name if this variable is a parameter
            if (output->GetFlags() & ShadyVariable::Parameter)
            {
                this->values.Add(valueName + "Name", generatedName);
                this->valueTypes.Add(valueName + "Name", Nody::VarType());
				this->valueFlags.Add(valueName + "Name", output->GetFlags());
            }
        }
    }

    // do the same for hidden parameters
    const Util::Array<Ptr<Nody::VariableInstance>>& hiddens = this->variation->GetHiddens();
    for (i = 0; i < hiddens.Size(); i++)
    {
        const Ptr<Nody::Variable>& hidden = hiddens[i]->GetOriginalVariable();

        // value name is the name of the output
        Util::String valueName = hidden->GetName();
        Util::Variant value;
        this->SetupDefaultValue(hidden->GetType(), value);  

        // also add an auto-generated name
        Util::String paramName;
        if (hidden->GetFlags() & ShadyVariable::Static) paramName = valueName;
		else                                            paramName = Nody::Project::RequestParameterName(hiddens[i], "Param");
        Util::Variant generatedName;
        generatedName.SetString(paramName);

        // add value and name
        this->values.Add(valueName, value);
        this->values.Add(valueName + "Name", generatedName);
        this->valueTypes.Add(valueName, hidden->GetType());
        this->valueTypes.Add(valueName + "Name", Nody::VarType());
		this->valueFlags.Add(valueName + "Name", hidden->GetFlags());
    }

    const Util::String& sim = this->variation->GetOriginalVariation()->GetSimulationValue();
    if (!sim.IsEmpty())
    {
        // copy simulation value
        this->simulationValue = Util::KeyValuePair<Util::Variant, Nody::VarType>(this->values[sim], this->valueTypes[sim]);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyNode::Setup(const Ptr<Nody::SuperVariation>& superVariation)
{
    Nody::Node::Setup(superVariation);
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyNode::Discard()
{
    Node::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyNode::GenerateGraphics()
{
	n_assert(!this->graphics.isvalid());
	Ptr<ShadyNodeGraphics> shadyGraphics = ShadyNodeGraphics::Create();
	shadyGraphics->node = this;
	this->graphics = shadyGraphics.upcast<Nody::NodeGraphics>();	
	this->graphics->Generate();

    // run base class
    Node::GenerateGraphics();
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyNode::DestroyGraphics()
{
	n_assert(this->graphics.isvalid());
	this->graphics->Destroy();
	this->graphics = 0;
}

//------------------------------------------------------------------------------
/**
*/
Nody::VarType 
ShadyNode::ResolveWorkingType()
{
    const Util::Array<Ptr<Nody::VariableInstance>>& inputs = this->variation->GetInputs();
    Nody::VarType workingType = this->variation->GetOutputs()[0]->GetOriginalVariable()->GetType();

    // solve working types by comparing all inputs
    IndexT i;
    for (i = 0; i < inputs.Size(); i++)
    {
        const Ptr<Nody::VariableInstance>& input = inputs[i];

        // ignore input if we have no inlink
        if (!input->GetInLink().isvalid()) continue;
        
        // get type on the other end of the link
        const Nody::VarType& type = input->GetInLink()->GetFromVariable()->GetInferredType();
        uint size1 = Nody::VarType::VectorSize(type);
        uint size2 = Nody::VarType::VectorSize(workingType);
        if (size1 > size2)
        {
            workingType = type;
        }
        else if (type != workingType)
        {
            Nody::VarType::ValueType comp1 = Nody::VarType::ComponentType(type);
            Nody::VarType::ValueType comp2 = Nody::VarType::ComponentType(workingType);

            // the enums are ordered in 'complexity', float - int - bool
            if (comp1 < comp2)
            {
                workingType = type;
            }
        }
    }

    return workingType;
}

//------------------------------------------------------------------------------
/**
*/
void
ShadyNode::SetupDefaultValue(const Nody::VarType& type, Util::Variant& value)
{
    switch (type.GetType())
    {
    case Nody::VarType::Float:
        value.SetFloat(0.0f);
        break;
    case Nody::VarType::Float2:
        {
            Util::Array<float> vec(2, 0, 0);
            value.SetFloatArray(vec);
            break;
        }
        break;
    case Nody::VarType::Float3:
        {
            Util::Array<float> vec(3, 0, 0);
            value.SetFloatArray(vec);
            break;
        }
    case Nody::VarType::Float4:
        {
            Util::Array<float> vec(4, 0, 0);
            value.SetFloatArray(vec);
            break;
        }
    case Nody::VarType::Int:
        value.SetInt(0);
        break;
    case Nody::VarType::Int2:
        {
            Util::Array<int> vec(2, 0, 0);
            value.SetIntArray(vec);
            break;
        }
    case Nody::VarType::Int3:
        {
            Util::Array<int> vec(3, 0, 0);
            value.SetIntArray(vec);
            break;
        }
    case Nody::VarType::Int4:
        {
            Util::Array<int> vec(4, 0, 0);
            value.SetIntArray(vec);
            break;
        }           
    case Nody::VarType::Bool:
        value.SetBool(false);
        break;
    case Nody::VarType::Sampler2D:
    case Nody::VarType::Sampler2DArray:
    case Nody::VarType::Sampler3D:
    case Nody::VarType::Sampler3DArray:
    case Nody::VarType::SamplerCube:
    case Nody::VarType::SamplerCubeArray:
        value.SetString("tex:system/black.dds");
        break;
    }
}

} // namespace Shady