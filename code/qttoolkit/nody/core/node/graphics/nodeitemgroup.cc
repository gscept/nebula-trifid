//------------------------------------------------------------------------------
//  nodeitemgroup.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "nodegraphics.h"
#include "nodeitemgroup.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>

namespace Nody
{
__ImplementClass(Nody::NodeItemGroup, 'NDIG', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
NodeItemGroup::NodeItemGroup()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
NodeItemGroup::~NodeItemGroup()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeItemGroup::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
	QGraphicsItemGroup::mousePressEvent(event);
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeItemGroup::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
	n_assert(this->node.isvalid());
	QGraphicsItemGroup::mouseMoveEvent(event);
    const QRectF& groupRect = this->boundingRect();
    // LOL, the scene returned from the scene()->sceneRect() is the same as the sceneBoundingRect()
    // however the sceneRect we get from our view is the fixed size scene rect...
    const QRectF& sceneRect = this->scene()->views()[0]->sceneRect();   
    float right = this->x() + groupRect.width();
    float bottom = this->y() + groupRect.height();
    float left = this->x();
    float top = this->y();

    // adjust position so that it never leaves the viewport
    if (left < sceneRect.left())
    {
        this->setPos(sceneRect.left(), this->y());
    }
    else if (right > sceneRect.right())
    {
        this->setPos(sceneRect.right() - groupRect.width(), this->y());
    }
    if (top < sceneRect.top())
    {
        this->setPos(this->x(), sceneRect.top());
    }
    else if (bottom > sceneRect.bottom())
    {
        this->setPos(this->x(), sceneRect.bottom() - groupRect.height());
    }
    
	const QPointF& pos = this->pos();
	this->node->OnMoved(Math::float2(pos.x(), pos.y()));
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeItemGroup::contextMenuEvent( QGraphicsSceneContextMenuEvent *event )
{
    QMenu* menu = this->node->OnRightClick();
    menu->exec(event->screenPos());
}

} // namespace Nody