//------------------------------------------------------------------------------
//  shadertest.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadertest.h"
#include "coregraphics/shaderinstance.h"

namespace Test
{
__ImplementClass(Test::ShaderTest, 'shdt', Test::CoreGraphicsTest);

using namespace CoreGraphics;
using namespace Resources;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
void
ShaderTest::Run()
{
    if (this->SetupRuntime())
    {
        ShaderServer* shdServer = ShaderServer::Instance();
        ShaderFeature::Mask defaultFeatureMask = shdServer->FeatureStringToMask("Default");
        ShaderFeature::Mask depthFeatureMask = shdServer->FeatureStringToMask("DepthOnly");
        ShaderVariable::Name varName = "ModelViewProjection";
        ShaderVariable::Semantic varSemantic = "MODELVIEWPROJECTION";

        // create a new shader instance
        Ptr<ShaderInstance> shaderInst = shdServer->CreateShaderInstance(ResourceId("shd:test"));
        this->Verify(shaderInst->IsValid());
        this->Verify(shaderInst->HasVariableByName(varName));
        this->Verify(shaderInst->HasVariableBySemantic(varSemantic));
        this->Verify(shaderInst->HasVariation(defaultFeatureMask));
        this->Verify(shaderInst->HasVariation(depthFeatureMask));
        this->Verify(shaderInst->GetNumVariables() == 1);
        this->Verify(shaderInst->GetNumVariations() == 2);        
        const Ptr<ShaderVariable>& mvp0 = shaderInst->GetVariableByName(varName);
        const Ptr<ShaderVariable>& mvp1 = shaderInst->GetVariableBySemantic(varSemantic);
        this->Verify(mvp0 == mvp1);
        this->Verify(mvp0->GetType() == ShaderVariable::MatrixType);
        this->Verify(mvp0->GetName() == varName);
        this->Verify(mvp0->GetSemantic() == varSemantic);
        /*this->Verify(mvp0->GetNumArrayElements() == 0);*/
        /*this->Verify(!mvp0->IsArray());*/
        matrix44 m;
        m.setrow3(float4(1.0f, 2.0f, 3.0f, 1.0f));
        mvp0->SetMatrix(m);
                
        const Ptr<ShaderVariation>& defaultVariation = shaderInst->GetVariationByFeatureMask(defaultFeatureMask);
        const Ptr<ShaderVariation>& depthVariation = shaderInst->GetVariationByFeatureMask(depthFeatureMask);
        this->Verify(defaultVariation->GetName() == "Default");
        this->Verify(defaultVariation->GetFeatureMask() == defaultFeatureMask);
        this->Verify(defaultVariation->GetNumPasses() == 1);
        this->Verify(depthVariation->GetName() == "DepthOnly");
        this->Verify(depthVariation->GetFeatureMask() == depthFeatureMask);
        this->Verify(depthVariation->GetNumPasses() == 1);

        // cleanup
        shaderInst->Discard();
        this->ShutdownRuntime();
    }
}

} // namespace Test