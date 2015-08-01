#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::GLSL4EffectProgram
    
    GLSL4 backend for EffectProgram, implements InternalEffectProgram.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "internal/internaleffectprogram.h"
#include "GL/glew.h"
namespace AnyFX
{
class GLSL4EffectProgram : public InternalEffectProgram
{
public:
	/// constructor
	GLSL4EffectProgram();
	/// destructor
	virtual ~GLSL4EffectProgram();

private:
	friend class GLSL4EffectVariable;
	friend class GLSL4EffectVarblock;
	friend class GLSL4EffectVarbuffer;

	/// applies program
	void Apply();
    /// applies subroutines
    void ApplySubroutines();
	/// links program
	bool Link();

	/// setup from other OpenGL program
	void SetupSlave(InternalEffectProgram* other);

    /// sets up subroutine mappings
    void SetupSubroutines();
    /// helper function for subroutine setup
    void SetupSubroutineHelper(GLenum shaderType, GLsizei& numBindings, GLuint** bindingArray, const eastl::map<eastl::string, InternalEffectSubroutine*>& bindings);

	GLuint programHandle;

    GLuint* vsSubroutineBindings;
    GLsizei numVsSubroutines;
    GLuint* hsSubroutineBindings;
    GLsizei numHsSubroutines;
    GLuint* dsSubroutineBindings;
    GLsizei numDsSubroutines;
    GLuint* gsSubroutineBindings;
    GLsizei numGsSubroutines;
    GLuint* psSubroutineBindings;
    GLsizei numPsSubroutines;
    GLuint* csSubroutineBindings;
    GLsizei numCsSubroutines;
}; 

struct GLSL4GlobalProgramState
{
    static unsigned program;
    static unsigned patchSize;
    static unsigned* vsSubroutines;
    static unsigned* hsSubroutines;
    static unsigned* dsSubroutines;
    static unsigned* gsSubroutines;
    static unsigned* psSubroutines;
    static unsigned* csSubroutines;    
};

} // namespace AnyFX
//------------------------------------------------------------------------------