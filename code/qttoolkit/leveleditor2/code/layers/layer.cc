//------------------------------------------------------------------------------
//  layer.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "layer.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "messaging/staticmessagehandler.h"

namespace Layers
{
__ImplementClass(Layers::Layer, 'LAYE', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Layer::Layer() :
	visible(true),
	autoLoad(true),
	locked(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Layer::~Layer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
Layer::SetName(const Util::String& name)
{
	IndexT i;
	for (i = 0; i < this->entities.Size(); i++)
	{
		this->entities[i]->SetString(Attr::_Layers, name);
	}
	this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
void
Layer::SetVisible(bool b)
{
	IndexT i;
	for (i = 0; i < this->entities.Size(); i++)
	{
		Ptr<GraphicsFeature::SetGraphicsVisible> msg = GraphicsFeature::SetGraphicsVisible::Create();
		msg->SetVisible(b);
		__SendSync(this->entities[i], msg);
	}
	this->visible = b;
}

//------------------------------------------------------------------------------
/**
*/
void
Layer::SetLocked(bool b)
{
	this->locked = b;
}

//------------------------------------------------------------------------------
/**
*/
void
Layer::Add(const Ptr<Game::Entity>& entity)
{
	n_assert(this->entities.BinarySearchIndex(entity) == InvalidIndex);

	// add to list and keep it sorted
	entity->SetString(Attr::_Layers, this->name);
	this->entities.Append(entity);
	this->entities.Sort();

	// set visible if we moved it into this layer
	Ptr<GraphicsFeature::SetGraphicsVisible> msg = GraphicsFeature::SetGraphicsVisible::Create();
	msg->SetVisible(this->visible);
	__SendSync(entity, msg);
}

//------------------------------------------------------------------------------
/**
*/
void
Layer::Remove(const Ptr<Game::Entity>& entity)
{
	IndexT index = this->entities.BinarySearchIndex(entity);
	n_assert(index != InvalidIndex);

	// erase index, we shouldn't need to sort the array now
	this->entities.EraseIndex(index);
}

//------------------------------------------------------------------------------
/**
*/
void
Layer::MoveToLayer(const Ptr<Layer>& otherLayer)
{
	n_assert(otherLayer.isvalid());
	otherLayer->entities.AppendArray(this->entities);
	otherLayer->SetVisible(otherLayer->visible);
	otherLayer->SetAutoLoad(otherLayer->autoLoad);
	otherLayer->SetName(otherLayer->name);
	otherLayer->entities.Sort();
}

} // namespace Layers