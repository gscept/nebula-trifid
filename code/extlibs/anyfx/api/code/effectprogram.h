#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectProgram
    
    An EffectProgram provides an interface to which we can apply shaders and render states.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#ifdef AFX_API
#error "afxapi.h included before effectprogram.h"
#endif

#include "EASTL/string.h"
#include "EASTL/vector.h"
#include "annotable.h"
namespace AnyFX
{
class EffectShader;
class EffectRenderState;
class InternalEffectProgram;
class EffectProgram : public Annotable
{
public:
	/// constructor
	EffectProgram();
	/// destructor
	virtual ~EffectProgram();

	/// discard program
	void Discard();

	/// applies program shaders and render states
	void Apply();
	/// commits changes to shader variables, call this before rendering
	void Commit();
    /// performs pre-draw stuff
    void PreDraw();
    /// performs post-draw stuff
    void PostDraw();
	/// returns name of program
    const eastl::string& GetName() const;
	/// returns array to list of shaders, this is list is always a constant size of 6
	eastl::vector<EffectShader*> GetShaders() const;
	/// returns render state
	EffectRenderState* GetRenderState() const;

	/// returns true if the program requires patch-based rendering
	/// the OpenGL4 backend automatically sets the patch size
	const bool SupportsTessellation() const;

    /// returns the group sizes defined if the program uses a compute shader
    const unsigned* GetLocalSizes() const;

	/// returns true if linking of program was successful
	bool IsValid();
	/// returns linking error string
    const eastl::string& GetError() const;

private:
	friend class EffectProgramStreamLoader;
	friend class EffectVariableStreamLoader;
	friend class EffectVarblockStreamLoader;
	friend class EffectVarbufferStreamLoader;

	EffectRenderState* renderState;
	struct EffectShaderBlock
	{
		EffectShader* vs;		// vertex shader
		EffectShader* ps;		// pixel shader
		EffectShader* ds;		// domain shader
		EffectShader* hs;		// hull shader
		EffectShader* gs;		// geometry shader
		EffectShader* cs;		// compute shader

		bool operator==(const EffectShaderBlock& rhs)
		{
			return this->vs == rhs.vs && this->ps == rhs.ps && this->ds == rhs.ds && this->hs == rhs.hs && this->gs == rhs.gs && this->cs == rhs.cs;
		}
	} shaderBlock;

	int shaderMask[6];
	InternalEffectProgram* internalProgram;
}; 

} // namespace AnyFX
//------------------------------------------------------------------------------