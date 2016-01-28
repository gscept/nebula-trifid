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

#define GL4_MULTIBIND 1
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

	/// commit shader state
	void Commit();

	/// setup from other OpenGL program
	void SetupSlave(InternalEffectProgram* other);

    /// sets up subroutine mappings
    void SetupSubroutines();
    /// helper function for subroutine setup
    void SetupSubroutineHelper(GLenum shaderType, GLsizei& numBindings, GLuint** bindingArray, const eastl::map<eastl::string, InternalEffectSubroutine*>& bindings);

	/// loading is done, time to setup range binds
	void LoadingDone();

	/// destroy shader objects
	void DestroyShaders();

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

#if GL4_MULTIBIND
	// multibind arrays
	GLuint varblockBindsCount;
	GLuint varbufferBindsCount;

	GLuint* textureBinds;
	GLint textureBindsCount;

	GLuint* imageBinds;
	GLint imageBindsCount;

	eastl::vector<GLSL4EffectVariable*> glsl4Variables;
	eastl::vector<GLSL4EffectVarblock*> glsl4Varblocks;
	eastl::vector<GLSL4EffectVarbuffer*> glsl4Varbuffers;

	bool varblocksDirty;
	bool varbuffersDirty;
	bool texturesDirty;
	bool imagesDirty;

	/// update multi binding buffer, but only flags as dirty if any change has happened
	static void SetVarblockBinding(unsigned index, int handle, unsigned offset, unsigned range);
	/// same but for varbuffers
	static void SetVarbufferBinding(unsigned index, int handle, unsigned offset, unsigned range);

	struct varblockBindings
	{
		GLuint buffer[512];
		GLintptr offset[512];
		GLintptr length[512];
		bool dirty;

		varblockBindings()
		{
			memset(buffer, 0, 512 * sizeof(int));
			memset(offset, 0, 512 * sizeof(int));
			memset(length, 1, 512 * sizeof(int));
			dirty = true;
		}

	} static globalVarblockBindings;

	struct varbufferBindings
	{
		GLuint buffer[512];
		GLintptr offset[512];
		GLintptr length[512];
		bool dirty;

		varbufferBindings()
		{
			memset(buffer, 0, 512 * sizeof(int));
			memset(offset, 0, 512 * sizeof(int));
			memset(length, 1, 512 * sizeof(int));
			dirty = true;
		}

	} static globalVarbufferBindings;
#endif
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



#if GL4_MULTIBIND
//------------------------------------------------------------------------------
/**
*/
inline void
GLSL4EffectProgram::SetVarblockBinding(unsigned index, int handle, unsigned offset, unsigned range)
{
	if (GLSL4EffectProgram::globalVarblockBindings.buffer[index] != handle ||
		GLSL4EffectProgram::globalVarblockBindings.offset[index] != offset ||
		GLSL4EffectProgram::globalVarblockBindings.length[index] != range)
	{
		GLSL4EffectProgram::globalVarblockBindings.buffer[index] = handle;
		GLSL4EffectProgram::globalVarblockBindings.offset[index] = offset;
		GLSL4EffectProgram::globalVarblockBindings.length[index] = range;
		GLSL4EffectProgram::globalVarblockBindings.dirty = true;
	}

}

//------------------------------------------------------------------------------
/**
*/
inline void
GLSL4EffectProgram::SetVarbufferBinding(unsigned index, int handle, unsigned offset, unsigned range)
{
	if (GLSL4EffectProgram::globalVarbufferBindings.buffer[index] != handle ||
		GLSL4EffectProgram::globalVarbufferBindings.offset[index] != offset ||
		GLSL4EffectProgram::globalVarbufferBindings.length[index] != range)
	{
		GLSL4EffectProgram::globalVarbufferBindings.buffer[index] = handle;
		GLSL4EffectProgram::globalVarbufferBindings.offset[index] = offset;
		GLSL4EffectProgram::globalVarbufferBindings.length[index] = range;
		GLSL4EffectProgram::globalVarbufferBindings.dirty = true;
	}
}
#endif

} // namespace AnyFX
//------------------------------------------------------------------------------