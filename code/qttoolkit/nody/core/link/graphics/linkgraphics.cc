//------------------------------------------------------------------------------
//  linkgraphics.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "link/link.h"
#include "linkgraphics.h"
#include "scene/graphics/nodegraphicsscene.h"
#include "variable/variableinstance.h"
#include "config.h"
#include <QApplication>

namespace Nody
{
__ImplementClass(Nody::LinkGraphics, 'LIGX', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
LinkGraphics::LinkGraphics() :
	linePathItem(0),
    curveType(Cubic),
	isLocked(false)
{
	this->standardPen = QPen(Qt::black);
	this->standardPen.setWidthF(1.6f);
	this->lockedPen = QPen(qRgb(210, 105, 30));
	this->lockedPen.setWidthF(1.6f);
}

//------------------------------------------------------------------------------
/**
*/
LinkGraphics::~LinkGraphics()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
LinkGraphics::Visit()
{
    QPen pen(QBrush(Qt::green), 2.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    this->linePathItem->setPen(pen);

    // also visit variables
    this->link->GetToVariable()->GetGraphics()->Visit();
    this->link->GetFromVariable()->GetGraphics()->Visit();    
    QApplication::processEvents();
}

//------------------------------------------------------------------------------
/**
*/
void 
LinkGraphics::Unvisit()
{
    this->linePathItem->setPen(this->lockedPen);

    // also unvisit variables
    this->link->GetToVariable()->GetGraphics()->Unvisit();
    this->link->GetFromVariable()->GetGraphics()->Unvisit();
    QApplication::processEvents();
}

//------------------------------------------------------------------------------
/**
*/
void 
LinkGraphics::Lock()
{
	n_assert(!this->isLocked);
	this->isLocked = true;
	this->linePathItem->setPen(this->lockedPen);
    
}

//------------------------------------------------------------------------------
/**
*/
void 
LinkGraphics::Unlock()
{
	n_assert(this->isLocked);
	this->isLocked = false;
	this->linePathItem->setPen(this->standardPen);
}

//------------------------------------------------------------------------------
/**
*/
void 
LinkGraphics::SetVisible( bool b )
{
	n_assert(0 != this->linePathItem);
	this->linePathItem->setVisible(b);
}

//------------------------------------------------------------------------------
/**
*/
void 
LinkGraphics::AddToScene( const Ptr<NodeGraphicsScene>& scene )
{
	n_assert(0 != this->linePathItem);

	// add graphics to scene
	scene->addItem(this->linePathItem);
}

//------------------------------------------------------------------------------
/**
*/
void 
LinkGraphics::RemoveFromScene( const Ptr<NodeGraphicsScene>& scene )
{
	n_assert(0 != this->linePathItem);
	scene->removeItem(this->linePathItem);
}

//------------------------------------------------------------------------------
/**
*/
void 
LinkGraphics::Generate()
{
	n_assert(0 == this->linePathItem);

	// create path item
	this->linePathItem = new AntialiasedLineGraphics;
	QPen pen;
	this->linePathItem->setPen(this->standardPen);
	this->linePathItem->setZValue(LINKLAYER);
}

//------------------------------------------------------------------------------
/**
*/
void 
LinkGraphics::Destroy()
{
	n_assert(0 != this->linePathItem);

	// delete path item
	delete this->linePathItem;
}

//------------------------------------------------------------------------------
/**
*/
void 
LinkGraphics::Update()
{
	n_assert(0 != this->linePathItem);
	QPointF dist = this->toAnchor - this->fromAnchor;
	this->linePath = QPainterPath(this->fromAnchor);
	qreal length = qMin(dist.manhattanLength() * 0.5f, 100.0);
    switch (this->curveType)
    {
    case Linear:
        this->linePath.lineTo(this->toAnchor);
        break;
    case Quadratic:
        this->linePath.quadTo(this->fromAnchor + QPointF(length, 0), this->toAnchor);
        break;
    case Cubic:
        this->linePath.cubicTo(this->fromAnchor + QPointF(length, 0), this->toAnchor + QPointF(-length, 0), this->toAnchor);
        break;
    }
	this->linePathItem->setPath(this->linePath);
}

//------------------------------------------------------------------------------
/**
*/
void 
LinkGraphics::OnNodeMoved()
{
	n_assert(this->link.isvalid());
	
	// get variables
	const Ptr<VariableInstance>& fromVarInst = this->link->GetFromVariable();
	const Ptr<VariableInstance>& toVarInst = this->link->GetToVariable();

	n_assert(fromVarInst.isvalid());
	n_assert(toVarInst.isvalid());

	// set anchors and update
	this->SetAnchorFrom(fromVarInst->GetGraphics()->GetCenter());
	this->SetAnchorTo(toVarInst->GetGraphics()->GetCenter());
	this->Update();
}

} // namespace Nody