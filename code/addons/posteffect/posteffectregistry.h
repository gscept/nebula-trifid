#pragma once
//------------------------------------------------------------------------------
/**
    @class PostEffect::PostEffectRegistry
  
	Contains different presets of post effect settings that can be applied to
	a PostEffectEntity.
	Different presets are read from the static database
           
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/

#include "core/singleton.h"
#include "posteffect/posteffectentity.h"
#include "util/hashtable.h"

//------------------------------------------------------------------------------
namespace PostEffect
{
class PostEffectRegistry : public Core::RefCounted
{
	__DeclareClass(PostEffectRegistry);
	__DeclareInterfaceSingleton(PostEffectRegistry);
public:
    
    /// constructor
	PostEffectRegistry();
    /// destructor
	virtual ~PostEffectRegistry();

    /// open the posteffectregistry and load presets
    virtual void OnActivate();
    /// close the posteffectregistry
    virtual void OnDeactivate();

	/// is preset available
	const bool HasPreset(const Util::String & preset) const;
    
	/// add or update preset
	void SetPreset(const Util::String & name, const PostEffectEntity::ParamSet & params);

	/// apply
	void ApplySettings(const Util::String & preset, const Ptr<PostEffect::PostEffectEntity> & entity);	
	/// clear registry
	void Clear();

private:
  
	Util::HashTable<Util::String, PostEffect::PostEffectEntity::ParamSet> paramRegistry;
    
};


//------------------------------------------------------------------------------
/**
*/
inline const bool
PostEffectRegistry::HasPreset(const Util::String & preset) const
{
	return this->paramRegistry.Contains(preset);
}

} // namespace PostEffect
//------------------------------------------------------------------------------
