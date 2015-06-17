//------------------------------------------------------------------------------
//  surfaceconstant.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "surfaceconstant.h"
#include "coregraphics/shaderinstance.h"
#include "resources/resourcemanager.h"
#include "surfaceconstantinstance.h"

using namespace Util;
namespace Materials
{
__ImplementClass(Materials::SurfaceConstant, 'SUCO', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
SurfaceConstant::SurfaceConstant() :
    system(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
SurfaceConstant::~SurfaceConstant()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Ptr<SurfaceConstantInstance>
SurfaceConstant::CreateInstance()
{
    Ptr<SurfaceConstantInstance> newInst = SurfaceConstantInstance::Create();
    newInst->Setup(this);
    return newInst;
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceConstant::Setup(const StringAtom& name, const Array<Ptr<CoreGraphics::ShaderInstance>>& shaders)
{
    this->name = name;

    IndexT shaderIndex;
    for (shaderIndex = 0; shaderIndex < shaders.Size(); shaderIndex++)
    {
        // get shader and variable (the variable must exist in the shader!)
        const Ptr<CoreGraphics::ShaderInstance>& shader = shaders[shaderIndex];
        const Ptr<CoreGraphics::ShaderVariable>& var = shader->GetVariableByName(this->name);
        this->variablesByShader.Add(shader, var);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceConstant::Discard()
{
    this->variablesByShader.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceConstant::Apply(const Ptr<CoreGraphics::ShaderInstance>& shader)
{
    // hmm, maybe shader instances deserves the zero-indexed treatment?
    const Ptr<CoreGraphics::ShaderVariable>& var = this->variablesByShader[shader];

    // setup value in shader
    switch (this->value.GetType())
    {
    case Variant::Int:
        var->SetInt(this->value.GetInt());
        break;
    case Variant::IntArray:
    {
        Array<int> array = this->value.GetIntArray();
        var->SetIntArray((const int*)&array, array.Size());
        break;
    }
    case Variant::Float:
        var->SetFloat(this->value.GetFloat());
        break;
    case Variant::FloatArray:
    {
       Array<float> array = this->value.GetFloatArray();
       var->SetFloatArray((const float*)&array, array.Size());
       break;
    }
    case Variant::Float2:
        var->SetFloat2(this->value.GetFloat2());
        break;
    case Variant::Float2Array:
    {
        Array<Math::float2> array = this->value.GetFloat2Array();
        var->SetFloat2Array((const Math::float2*)&array, array.Size());
        break;
    }
    case Variant::Float4:
        var->SetFloat4(this->value.GetFloat4());
        break;
    case Variant::Float4Array:
    {
        Array<Math::float4> array = this->value.GetFloat4Array();
        var->SetFloat4Array((const Math::float4*)&array, array.Size());
        break;
    }
    case Variant::Matrix44:
        var->SetMatrix(this->value.GetMatrix44());
        break;
    case Variant::Matrix44Array:
    {
        Array<Math::matrix44> array = this->value.GetMatrix44Array();
        var->SetMatrixArray((const Math::matrix44*)&array, array.Size());
        break;
    }
    case Variant::Bool:
        var->SetBool(this->value.GetBool());
        break;
    case Variant::BoolArray:
    {
        Array<bool> array = this->value.GetBoolArray();
        var->SetBoolArray((const bool*)&array, array.Size());
        break;
    }
    case Variant::Object:
        // @note: implicit Ptr<> creation!
        if (this->value.GetObject() != 0)
        {
            CoreGraphics::Texture* tex = (CoreGraphics::Texture*)this->value.GetObject();
            var->SetTexture(tex);
        }
        else
        {
            var->SetTexture(NULL);
        }
        break;
    default:
        n_error("SurfaceMaterial::Setup(): invalid data type for parameter!");
        break;
    }
}


} // namespace Materials