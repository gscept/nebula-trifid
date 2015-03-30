#pragma once
//------------------------------------------------------------------------------
/**
    @class OpenGL4::OGL4ShaderVariation

    Under OpenGL4, a shader variation is represented by an AnyFX effect 
    program which must be annotated by a FeatureMask string.

    (C) 2013 Gustav Sterbrant
*/
#include "afxapi.h"
#include "coregraphics/base/shadervariationbase.h"

//------------------------------------------------------------------------------
namespace OpenGL4
{
class OGL4ShaderVariation : public Base::ShaderVariationBase
{
    __DeclareClass(OGL4ShaderVariation);
public:
    /// constructor
    OGL4ShaderVariation();
    /// destructor
    virtual ~OGL4ShaderVariation();

	/// applies program
	void Apply();
	/// performs a variable commit to the current program
	void Commit();
    /// calls shading system pre draw callback
    void PreDraw();
    /// calls shading subsystem post draw callback
    void PostDraw();
    /// override current render state to use wireframe
    void SetWireframe(bool b);

	/// returns true if shader variation needs to use patches
	const bool UsePatches() const;

	/// get program
	AnyFX::EffectProgram* GetProgram() const;
private:
    friend class OGL4ShaderInstance;

	/// setup from AnyFX program
	void Setup(AnyFX::EffectProgram* program);

	bool usePatches;
	AnyFX::EffectProgram* program;

};

//------------------------------------------------------------------------------
/**
*/
inline AnyFX::EffectProgram*
OGL4ShaderVariation::GetProgram() const
{
	return this->program;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
OGL4ShaderVariation::UsePatches() const
{
	return this->usePatches;
}

} // namespace Direct3D9
//------------------------------------------------------------------------------
