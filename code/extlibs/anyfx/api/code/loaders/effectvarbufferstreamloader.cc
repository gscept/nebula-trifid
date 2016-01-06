//------------------------------------------------------------------------------
//  effectvarbufferstreamloader.cc
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "effectvarbufferstreamloader.h"
#include "effect.h"
#include "effectvarbuffer.h"
#include "effectprogram.h"
#include "internal/internaleffectvarbuffer.h"
#include "internal/internaleffectprogram.h"
#include "internal/glsl4/glsl4effectvarbuffer.h"
#include "effectannotationstreamloader.h"
#include "effectvariablestreamloader.h"

namespace AnyFX
{

eastl::map<eastl::string, InternalEffectVarbuffer*> EffectVarbufferStreamLoader::sharedBuffers;
//------------------------------------------------------------------------------
/**
*/
EffectVarbufferStreamLoader::EffectVarbufferStreamLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EffectVarbufferStreamLoader::~EffectVarbufferStreamLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EffectVarbuffer*
EffectVarbufferStreamLoader::Load( BinReader* reader, Effect* effect )
{
    InternalEffectVarbuffer* internalVarbuffer = 0;

    if (effect->GetType() == Effect::GLSL)
    {
        if (effect->GetMajor() == 4) internalVarbuffer = new GLSL4EffectVarbuffer;
    }
	EffectVarbuffer* varbuffer = new EffectVarbuffer;

	// start loading
	eastl::string name = reader->ReadString().c_str();
	bool shared = reader->ReadBool();

	// load annotations
	bool hasAnnotation = reader->ReadBool();
	if (hasAnnotation)
	{
		EffectAnnotationStreamLoader loader;
		loader.Load(reader, varbuffer);
	}

	// load size of buffer
	unsigned size = reader->ReadUInt();

	// load internal buffer
	internalVarbuffer->name = name;
	internalVarbuffer->size = size;
	internalVarbuffer->isShared = shared;

	size_t numPrograms = effect->GetNumPrograms();
	EffectProgram** programs = effect->GetPrograms();
	eastl::vector<InternalEffectProgram*> internalPrograms;
	internalPrograms.reserve(numPrograms);

	unsigned i;
	for (i = 0; i < numPrograms; i++)
	{
        if (programs[i]->IsValid()) internalPrograms.push_back(programs[i]->internalProgram);
	}

	// setup shared varbuffers
	if (internalVarbuffer->isShared)
	{
		if (sharedBuffers.find(internalVarbuffer->GetName()) != sharedBuffers.end())
		{
			// get shared block
			InternalEffectVarbuffer* sharedBuffer = sharedBuffers[internalVarbuffer->GetName()];

			// bump reference counter of internal varblock
			sharedBuffer->Retain();

			// add programs to block
			internalVarbuffer->SetupSlave(internalPrograms, sharedBuffer);
		}
		else
		{
			// set varblock in shared list
			sharedBuffers[internalVarbuffer->GetName()] = internalVarbuffer;

			// setup varblock
			internalVarbuffer->Setup(internalPrograms);
		}
	}
	else
	{
		// setup varblock
		internalVarbuffer->Setup(internalPrograms);
	}

	varbuffer->internalVarbuffer = internalVarbuffer;
    return varbuffer;
}
} // namespace AnyFX