//------------------------------------------------------------------------------
//  shadervariableinstancebase.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/base/shadervariableinstancebase.h"
#include "coregraphics/shadervariableinstance.h"
#include "coregraphics/shadervariable.h"
#include "resources/resourcemanager.h"

namespace Base
{
__ImplementClass(Base::ShaderVariableInstanceBase, 'SVIB', Core::RefCounted);

using namespace CoreGraphics;
using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
ShaderVariableInstanceBase::ShaderVariableInstanceBase() :
	shaderVariable(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ShaderVariableInstanceBase::~ShaderVariableInstanceBase()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderVariableInstanceBase::Setup(const Ptr<ShaderVariable>& var)
{
	this->Prepare(var->GetType());
	this->shaderVariable = var;    
}

//------------------------------------------------------------------------------
/**
*/
void 
ShaderVariableInstanceBase::Setup( const Base::ShaderVariableBase::Type& type )
{
	this->Prepare(type);
}

//------------------------------------------------------------------------------
/**
*/
void 
ShaderVariableInstanceBase::Discard()
{
	this->shaderVariable->DiscardInstance((ShaderVariableInstance*)this);
}

//------------------------------------------------------------------------------
/**
*/
void 
ShaderVariableInstanceBase::Cleanup()
{
	n_assert(this->shaderVariable.isvalid());
	this->shaderVariable = 0;

	// clear value
	this->value.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderVariableInstanceBase::Prepare(ShaderVariable::Type type)
{
    n_assert(!this->shaderVariable.isvalid());
    switch (type)
    {
        case ShaderVariable::IntType:
            this->value.SetType(Variant::Int);
            break;
        
        case ShaderVariable::FloatType:
            this->value.SetType(Variant::Float);
            break;

        case ShaderVariable::VectorType:
            this->value.SetType(Variant::Float4);
            break;

        case ShaderVariable::MatrixType:
            this->value.SetType(Variant::Matrix44);
            break;

        case ShaderVariable::BoolType:
            this->value.SetType(Variant::Bool);
            break;

        case ShaderVariable::TextureType:
            this->value.SetType(Variant::Object);
            break;

        default:
            n_error("ShaderVariableInstanceBase::Prepare(): Invalid ShaderVariable::Type in switch!\n");
            break;
    }
}

//------------------------------------------------------------------------------
/**
    @todo: hmm, the dynamic type switch is sort of lame...
*/
void
ShaderVariableInstanceBase::Apply()
{
    n_assert(this->shaderVariable.isvalid());
    switch (this->value.GetType())
    {
        case Variant::Int:
            this->shaderVariable->SetInt(this->value.GetInt());
            break;
        case Variant::Float:
            this->shaderVariable->SetFloat(this->value.GetFloat());
            break;
        case Variant::Float4:
            this->shaderVariable->SetFloat4(this->value.GetFloat4());
            break;
        case Variant::Matrix44:
            this->shaderVariable->SetMatrix(this->value.GetMatrix44());
            break;
        case Variant::Bool:
            this->shaderVariable->SetBool(this->value.GetBool());
            break;
        case Variant::Object:
            // @note: implicit Ptr<> creation!
            if (this->value.GetObject() != 0)
            {
                this->shaderVariable->SetTexture((Texture*)this->value.GetObject());
            }
			else
			{
				// attempt to load texture if at variable instance binding, the texture wasn't available
				const Ptr<Resources::ResourceManager>& resManager = Resources::ResourceManager::Instance();
				if (resManager->HasResource(this->deferredTexture))
				{
					const Ptr<Resources::Resource>& resource = resManager->LookupResource(this->deferredTexture);
					this->value.SetObject(resource.upcast<Core::RefCounted>());
				}
			}
            break;

        default:
            n_error("ShaderVariableInstance::Apply(): invalid data type!");
            break;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ShaderVariableInstanceBase::ApplyTo( const Ptr<CoreGraphics::ShaderVariable>& var )
{
	n_assert(var.isvalid());
	switch (this->value.GetType())
	{
	case Variant::Int:
		var->SetInt(this->value.GetInt());
		break;
	case Variant::Float:
		var->SetFloat(this->value.GetFloat());
		break;
	case Variant::Float4:
		var->SetFloat4(this->value.GetFloat4());
		break;
	case Variant::Matrix44:
		var->SetMatrix(this->value.GetMatrix44());
		break;
	case Variant::Bool:
		var->SetBool(this->value.GetBool());
		break;
	case Variant::Object:
		// @note: implicit Ptr<> creation!
		if (this->value.GetObject() != 0)
		{
			var->SetTexture((Texture*)this->value.GetObject());
		}
		break;
	default:
		n_error("ShaderVariable::Apply(): invalid data type for scalar!");
		break;
	}
}

} // namespace Base

