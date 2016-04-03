#pragma once
//------------------------------------------------------------------------------
/**
	@class LevelEditor2::LightProbe
	
	This class captures the scene around it and generates a cube map reflection.

	It can be linked with elements which should be hidden prior to rendering, which results in
	objects being removed from the environment capture.

	This class also supports rendering out an irradiance map, which goes together
	with the reflection map to create a fully offline diffuse and specular global illumination.
	
	(C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/rendertargetcube.h"
#include "graphics/modelentity.h"
#include "game/entity.h"
class QPlainTextEdit;
namespace LevelEditor2
{
class EnvironmentProbeCapturer : public Core::RefCounted
{
	__DeclareClass(EnvironmentProbeCapturer);
public:
	/// constructor
	EnvironmentProbeCapturer();
	/// destructor
	virtual ~EnvironmentProbeCapturer();

	/// set game entity which this probe is contained within
	void SetEntity(const Ptr<Game::Entity>& entity);
	/// we need to discard this when we're done with it because it has a reference back to the owning entity
	void Discard();

	/// sets visibility of entity to which this capturer is attached
	void SetVisible(bool b);

	/// set render target cube
	const Ptr<CoreGraphics::RenderTargetCube>& GetRenderTargetCube() const;
	/// adds en entity to be hidden when rendering
	void AddHideLink(const Ptr<Graphics::GraphicsEntity>& entity);
	/// removes an entity to be hidden when rendering
	void RemoveHideLink(const Ptr<Graphics::GraphicsEntity>& entity);

	/// set name
	void SetName(const Util::String& name);
	/// get name
	const Util::String& GetName() const;
	/// set position of the light probe
	void SetPosition(const Math::float4& pos);
	/// set capture bounding box of the light probe
	void SetCaptureZone(const Math::bbox& box);
	/// set resolution
	void SetResolution(SizeT width, SizeT height);
	/// set if we should calculate irradiance
	void SetRenderIrradiance(bool b);
	/// set if we should calculate reflections
	void SetRenderReflections(bool b);
	/// set if we should save a depth cube
	void SetGenerateDepthCube(bool b);
	/// set if we should generate mipmaps for reflections
	void SetGenerateMipmaps(bool b);

	/// set output folder and name
	void SetOutput(const Util::String& folder, const Util::String& name);
	/// get name of generated reflection map
	const Util::String& GetReflectionMapName() const;
	/// get name of generated irradiance map
	const Util::String& GetIrradianceMapName() const;

	/// handle stuff to do before rendering
	void BeforeRender();
	/// renders environment into cube map
	void Render(QPlainTextEdit* progressBar);
	/// handle stuff to do after rendering
	void AfterRender();

private:	
	Util::String name;
	bool calculateReflections;
	bool calculateIrradiance;
	bool captureDepth;
	bool generateMipmaps;
	SizeT resolutionX, resolutionY;
	
	Util::Array<Ptr<Graphics::GraphicsEntity>> hideLinks;
	Math::float4 position;
	Math::bbox captureZone;

	Util::String outputFolder;
	Util::String outputName;
	Ptr<CoreGraphics::RenderTargetCube> probeMap;
	Util::String reflectionMapName;
	Util::String irradianceMapName;
	Util::String depthMapName;

	Ptr<Game::Entity> entity;
};

//------------------------------------------------------------------------------
/**
*/
inline void
EnvironmentProbeCapturer::SetEntity(const Ptr<Game::Entity>& entity)
{
	this->entity = entity;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::RenderTargetCube>&
EnvironmentProbeCapturer::GetRenderTargetCube() const
{
	return this->probeMap;
}

//------------------------------------------------------------------------------
/**
*/
inline void
EnvironmentProbeCapturer::SetPosition(const Math::float4& pos)
{
	this->position = pos;
}

//------------------------------------------------------------------------------
/**
*/
inline void
EnvironmentProbeCapturer::SetCaptureZone(const Math::bbox& box)
{
	this->captureZone = box;
}

//------------------------------------------------------------------------------
/**
*/
inline void
EnvironmentProbeCapturer::SetResolution(SizeT width, SizeT height)
{
	this->resolutionX = width;
	this->resolutionY = height;
}

//------------------------------------------------------------------------------
/**
*/
inline void
EnvironmentProbeCapturer::SetName(const Util::String& name)
{
	this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
EnvironmentProbeCapturer::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline void
EnvironmentProbeCapturer::SetRenderIrradiance(bool b)
{
	this->calculateIrradiance = b;
}

//------------------------------------------------------------------------------
/**
*/
inline void
EnvironmentProbeCapturer::SetRenderReflections(bool b)
{
	this->calculateReflections = b;
}

//------------------------------------------------------------------------------
/**
*/
inline void
EnvironmentProbeCapturer::SetGenerateDepthCube(bool b)
{
	this->captureDepth = b;
}

//------------------------------------------------------------------------------
/**
*/
inline void
EnvironmentProbeCapturer::SetGenerateMipmaps(bool b)
{
	this->generateMipmaps = b;
}

//------------------------------------------------------------------------------
/**
*/
inline void
EnvironmentProbeCapturer::SetOutput(const Util::String& folder, const Util::String& name)
{
	this->outputFolder = folder;
	this->outputName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
EnvironmentProbeCapturer::GetReflectionMapName() const
{
	return this->reflectionMapName;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
EnvironmentProbeCapturer::GetIrradianceMapName() const
{
	return this->irradianceMapName;
}

} // namespace LevelEditor2