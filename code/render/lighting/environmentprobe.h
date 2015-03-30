#pragma once
//------------------------------------------------------------------------------
/**
	@class Lighting::EnvironmentProbe
	
	An environment probe contains information about the environment and irradiance map being used in an area.

	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "resources/managedtexture.h"
namespace Lighting
{
class EnvironmentProbe : public Core::RefCounted
{
	__DeclareClass(EnvironmentProbe);
public:
	/// constructor
	EnvironmentProbe();
	/// destructor
	virtual ~EnvironmentProbe();

	/// assign reflection map resource, returns true if a texture was loaded
	bool AssignReflectionMap(const Resources::ResourceId& refl);
	/// assign irradiance map resource, returns true if a texture was loaded
	bool AssignIrradianceMap(const Resources::ResourceId& irr);

	/// discard probe, unloads textures
	void Discard();

	/// get reflection map
	const Ptr<Resources::ManagedTexture>& GetReflectionMap() const;
	/// get irradiance map
	const Ptr<Resources::ManagedTexture>& GetIrradianceMap() const;

	/// the default environment probe which is automatically assigned to all model entitys upon startup
	static Ptr<EnvironmentProbe> DefaultEnvironmentProbe;

private:
	
	Ptr<Resources::ManagedTexture> reflectionMap;
	Ptr<Resources::ManagedTexture> irradianceMap;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Resources::ManagedTexture>&
EnvironmentProbe::GetReflectionMap() const
{
	return this->reflectionMap;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Resources::ManagedTexture>&
EnvironmentProbe::GetIrradianceMap() const
{
	return this->irradianceMap;
}

} // namespace Lighting