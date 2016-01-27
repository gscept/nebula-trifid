#pragma once
//------------------------------------------------------------------------------
/**
	@class Layers::Layer
	
	The layer describes a single layer, whether it should auto load when the level laods, and if it's visible. 
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "game/entity.h"
namespace Layers
{
class Layer : public Core::RefCounted
{
	__DeclareClass(Layer);
public:
	/// constructor
	Layer();
	/// destructor
	virtual ~Layer();

	/// set name of layer
	void SetName(const Util::String& name);
	/// get name of layer
	const Util::String& GetName() const;
	/// set layer visibility
	void SetVisible(bool b);
	/// get layer visibility
	const bool GetVisible() const;
	/// set layer autoload
	void SetAutoLoad(bool b);
	/// get layer autoload
	const bool GetAutoLoad() const;
	/// set locked
	void SetLocked(bool b);
	/// get layer autoload
	const bool GetLocked() const;

	/// add entity to layer
	void Add(const Ptr<Game::Entity>& entity);
	/// remove entity from layer
	void Remove(const Ptr<Game::Entity>& entity);

	/// move all entities from one layer to anoter
	void MoveToLayer(const Ptr<Layer>& otherLayer);

private:
	friend class LayerHandler;
	Util::String name;
	bool visible;
	bool autoLoad;
	bool locked;
	Util::Array<Ptr<Game::Entity>> entities;
};


//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
Layer::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
Layer::GetVisible() const
{
	return this->visible;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Layer::SetAutoLoad(bool b)
{
	this->autoLoad = b;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
Layer::GetAutoLoad() const
{
	return this->autoLoad;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
Layer::GetLocked() const
{
	return this->locked;
}

} // namespace Layers