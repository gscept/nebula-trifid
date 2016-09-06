//------------------------------------------------------------------------------
//  nodegraphics.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "nodepixmapitem.h"
#include "nodeitemgroup.h"
#include "nodegraphics.h"
#include "scene/graphics/nodegraphicsscene.h"
#include "node/node.h"
#include "link/link.h"
#include "nody_config.h"

using namespace Util;
namespace Nody
{
__ImplementClass(Nody::NodeGraphics, 'NDGX', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
NodeGraphics::NodeGraphics() : 
	node(0),
    group(0),
    focus(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
NodeGraphics::~NodeGraphics()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphics::Visit()
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphics::Unvisit()
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphics::Error()
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphics::Unerror()
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphics::OnMoved( const Math::float2& pos )
{
	this->position = pos;

	// get links and update their positions
	const Array<Ptr<Link> >& links = this->node->GetLinks();

	IndexT i;
	for (i = 0; i < links.Size(); i++)
	{
		// get pointer to graphics
		const Ptr<LinkGraphics>& linkGraphics = links[i]->GetGraphics();

		// perform callback to notify our node has moved
		linkGraphics->OnNodeMoved();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphics::SetPosition( const Math::float2& pos )
{
    this->position = pos;
    this->group->setPos(pos.x(), pos.y());
}

//------------------------------------------------------------------------------
/**
	This function must be overridden.
	This is because each subclass of NodeGraphics has its own set of graphical items.
*/
void 
NodeGraphics::AddToScene( const Ptr<NodeGraphicsScene>& scene )
{
    // get view of scene
    QGraphicsView* view = scene->views()[0];

    // finally add group to scene
    scene->addItem(this->group);
    this->group->setPos(view->mapToScene(view->viewport()->rect().center() - (this->group->boundingRect().bottomRight()*0.5f).toPoint()));
}

//------------------------------------------------------------------------------
/**
	This is function must also be overridden for the same reason as the one above.
*/
void 
NodeGraphics::RemoveFromScene( const Ptr<NodeGraphicsScene>& scene )
{
    n_assert(0 != this->group->scene());

    // remove item group from scene
    scene->removeItem(this->group);
}

//------------------------------------------------------------------------------
/**
	Must be overridden since each NodeGraphics implementation has its own set of graphical items.
*/
void 
NodeGraphics::Generate()
{
    n_assert(!this->group.isvalid());

    // create group
    this->group = NodeItemGroup::Create();
    this->group->node = this;
    if (this->node->GetFlags() & Node::NoMove)		this->group->setFlag(QGraphicsItem::ItemIsMovable, false);
    else                                            this->group->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->group->setHandlesChildEvents(false);
    this->group->setZValue(NODEDEFAULTLAYER);
    this->group->setData(NEBULAUSERPOINTERLOCATION, (qint32)this);
}

//------------------------------------------------------------------------------
/**
	Override this if you want to remove the graphical items from the scene, but be sure to call parent class also.
*/
void 
NodeGraphics::Destroy()
{
	this->node = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphics::OnSimulate(const Util::Variant& value, const Nody::VarType& type, SimResult& result)
{
    // implement in sublcass
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphics::OnSimulate(const SimResult& value)
{
    // implement in sublcass
}

//------------------------------------------------------------------------------
/**
*/
QMenu*
NodeGraphics::OnRightClick() const
{
    // empty, implement in subclass
    return NULL;
}

//------------------------------------------------------------------------------
/**
*/
QPointF 
NodeGraphics::FromFloat2( const Math::float2& pos )
{
	QPointF point(pos.x(), pos.y());
	return point;
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphics::SetFocus(bool b, bool resort)
{
	if (resort)
	{
		if (b)	this->group->setZValue(NODEFOCUSEDLAYER);
		else	this->group->setZValue(NODEDEFAULTLAYER);
	}	
	this->focus = b;
}


} // namespace Nody
