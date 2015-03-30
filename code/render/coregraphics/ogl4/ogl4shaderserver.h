#pragma once
//------------------------------------------------------------------------------
/**
    @class Direct3D9::OGL4ShaderServer
    
    OGL4 implementation of ShaderServer.
    
    (C) 2007 Radon Labs GmbH
*/
#include "coregraphics/base/shaderserverbase.h"
#include "coregraphics/ogl4/ogl4shader.h"
#include "effectfactory.h"

//------------------------------------------------------------------------------
namespace OpenGL4
{
class OGL4ShaderServer : public Base::ShaderServerBase
{
    __DeclareClass(OGL4ShaderServer);
    __DeclareSingleton(OGL4ShaderServer);
public:
    /// constructor
    OGL4ShaderServer();
    /// destructor
    virtual ~OGL4ShaderServer();
    
    /// open the shader server
    bool Open();
    /// close the shader server
    void Close();


	/// get the shared shader
	Ptr<CoreGraphics::ShaderInstance> GetSharedShader();
	/// reloads a shader
	void ReloadShader(Ptr<CoreGraphics::Shader> shader);
	/// explicitly loads a shader by resource id
	void LoadShader(const Resources::ResourceId& shdName);

private:
	AnyFX::EffectFactory* factory;
};

} // namespace Direct3D9
//------------------------------------------------------------------------------
    