//------------------------------------------------------------------------------
//  connectorgraphicsitem.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "nody_config.h"
#include "variableinstancegraphics.h"
#include "variable/variableinstance.h"
#include "scene/graphics/nodegraphicsscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

namespace Nody
{
__ImplementClass(VariableInstanceGraphics, 'VARG', Core::RefCounted);
//------------------------------------------------------------------------------
/**
*/
VariableInstanceGraphics::VariableInstanceGraphics() :
	connectorItem(0),
	isLocked(false)
{
	this->highlightBrush = QBrush(qRgb(210,105,30));
	this->standardBrush = QBrush(Qt::white);
	this->lockedBrush = QBrush(qRgb(210, 105, 30));
}

//------------------------------------------------------------------------------
/**
*/
VariableInstanceGraphics::~VariableInstanceGraphics()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::Generate()
{
	n_assert(0 == this->connectorItem);
	this->connectorItem = new QGraphicsPolygonItem;
	this->connectorItem->setAcceptHoverEvents(true);
	this->connectorItem->setPen(QPen(QBrush(Qt::black), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
	this->connectorItem->setBrush(QBrush(Qt::white));
	this->connectorItem->setData(NEBULAUSERPOINTERLOCATION, (qint32)this);
	this->connectorItem->setZValue(NODELAYERFOREGROUND);
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::Destroy()
{
	n_assert(0 != this->connectorItem);
	delete this->connectorItem;
	this->varInst = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::AddToGroup( QGraphicsItemGroup* group )
{
	n_assert(0 != group);
	group->addToGroup(this->connectorItem);
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::RemoveFromGroup( QGraphicsItemGroup* group )
{
	n_assert(0 != group);
	group->removeFromGroup(this->connectorItem);
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::AddToScene( const Ptr<NodeGraphicsScene>& scene )
{
	n_assert(scene.isvalid());
	scene->addItem(this->connectorItem);
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::RemoveFromScene( const Ptr<NodeGraphicsScene>& scene )
{
	n_assert(scene.isvalid());
	scene->removeItem(this->connectorItem);
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::SetPosition( const Math::float2& pos )
{
	n_assert(0 != this->connectorItem);
	this->pos = pos;
	this->connectorItem->setPos(pos.x(), pos.y());
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::SetSize( const Math::float2& size )
{
	n_assert(0 != this->connectorItem);
	this->size = size;
	this->connectorItem->setPolygon(QPolygonF(QRectF(0, 0, size.x(), size.y())));
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::Lock()
{
	n_assert(!this->isLocked);

	// also remember to allow the item to be moved once again
	this->connectorItem->parentItem()->setFlag(QGraphicsItem::ItemIsMovable, true);

	this->connectorItem->setBrush(this->lockedBrush);
	this->isLocked = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::Unlock()
{
	n_assert(this->isLocked);
	this->connectorItem->setBrush(this->standardBrush);
	this->isLocked = false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
VariableInstanceGraphics::IsLocked() const
{
	return this->isLocked;
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::Highlight()
{
	n_assert(0 != this->connectorItem);

	// disable dragging while highlighted, also make sure nothing is moving
	this->connectorItem->parentItem()->setFlag(QGraphicsItem::ItemIsMovable, false);
	if (!this->isLocked) this->connectorItem->setBrush(this->highlightBrush);
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::UnHighlight()
{
	n_assert(0 != this->connectorItem);

	// reset to previous state
	this->connectorItem->parentItem()->setFlag(QGraphicsItem::ItemIsMovable, true);
	if (!this->isLocked) this->connectorItem->setBrush(this->standardBrush);
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::Visit()
{
    QBrush brush(Qt::green);
    this->connectorItem->setBrush(brush);
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::Unvisit()
{
    this->connectorItem->setBrush(this->lockedBrush);
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstanceGraphics::SetVariableInstance( const Ptr<VariableInstance>& var )
{
	n_assert(var.isvalid());
	this->varInst = var;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<VariableInstance>& 
VariableInstanceGraphics::GetVariableInstance() const
{
	return this->varInst;
}

} // namespace Nody
