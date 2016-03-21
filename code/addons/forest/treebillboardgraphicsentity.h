#pragma once
//------------------------------------------------------------------------------
/**
    @class Forest::TreeBillboardGraphicsEntity

    A subclass of graphics entity to notify the tree billboard renderer
    about the Nebula 3 render loop.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "graphics/graphicsentity.h"

//------------------------------------------------------------------------------
namespace Forest
{

class TreeBillboardGraphicsEntity : public Graphics::GraphicsEntity
{
    __DeclareClass(TreeBillboardGraphicsEntity);
public:
    /// constructor
    TreeBillboardGraphicsEntity();
    /// destructor
    virtual ~TreeBillboardGraphicsEntity();

    /// called when attached to level
    virtual void OnActivate();
    /// called when removed from level
    virtual void OnDeactivate();
};

} // namespace Forest2
//------------------------------------------------------------------------------
