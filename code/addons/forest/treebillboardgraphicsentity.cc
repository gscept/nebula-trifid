//------------------------------------------------------------------------------
//  treebillboardgraphicsentity.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "forest/treebillboardgraphicsentity.h"
#include "forest/treebillboardrenderer.h"
#include "graphics/graphicsentitytype.h"

namespace Forest
{
__ImplementClass(Forest::TreeBillboardGraphicsEntity, 'TBGE', Graphics::GraphicsEntity);

using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
TreeBillboardGraphicsEntity::TreeBillboardGraphicsEntity()
{
    this->SetType(GraphicsEntityType::Tree);
}

//------------------------------------------------------------------------------
/**
*/
TreeBillboardGraphicsEntity::~TreeBillboardGraphicsEntity()
{
	// empty
}

//------------------------------------------------------------------------------
/**
    Need to override the Graphics entity's activation.
*/
void
TreeBillboardGraphicsEntity::OnActivate()
{
	n_assert(!this->IsActive());
	GraphicsEntity::OnActivate();
	this->SetAlwaysVisible(true);    
}

//------------------------------------------------------------------------------
/**
*/
void
TreeBillboardGraphicsEntity::OnDeactivate()
{
    GraphicsEntity::OnDeactivate();
}


} // namespace Forest
