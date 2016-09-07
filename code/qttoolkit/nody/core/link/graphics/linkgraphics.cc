//------------------------------------------------------------------------------
//  linkgraphics.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "link/link.h"
#include "linkgraphics.h"
#include "scene/graphics/nodegraphicsscene.h"
#include "variable/variableinstance.h"
#include "nody_config.h"
#include <QApplication>

#define LINK_DATAFLOW_ELLIPSE_RADIUS 15.0f
#define LINK_DATAFLOW_ELLIPSE_HALF_SIZE LINK_DATAFLOW_ELLIPSE_RADIUS/2.0f

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

	// show data flow ellipses
	this->ellipseItems[0]->show();
	this->ellipseItems[1]->show();
	this->ellipseItems[2]->show();
	this->ellipseItems[3]->show();

	// reset time
	this->dataFlowTime = 0.0f;

	// update dots a single time, then start rendering timer
	this->RenderDataFlow();
	connect(&this->dataFlowTimer, SIGNAL(timeout()), this, SLOT(RenderDataFlow()));
	this->dataFlowTimer.start();

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

	// hide data flow ellipses
	this->ellipseItems[0]->hide();
	this->ellipseItems[1]->hide();
	this->ellipseItems[2]->hide();
	this->ellipseItems[3]->hide();

	this->dataFlowTimer.stop();
	disconnect(&this->dataFlowTimer, SIGNAL(timeout()), this, SLOT(RenderDataFlow()));

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
LinkGraphics::SetVisible(bool b)
{
	n_assert(0 != this->linePathItem);
	this->linePathItem->setVisible(b);
}

//------------------------------------------------------------------------------
/**
*/
void
LinkGraphics::AddToScene(const Ptr<NodeGraphicsScene>& scene)
{
	n_assert(0 != this->linePathItem);

	// add graphics to scene
	scene->addItem(this->linePathItem);
	QRadialGradient gradient(QPointF(LINK_DATAFLOW_ELLIPSE_HALF_SIZE, LINK_DATAFLOW_ELLIPSE_HALF_SIZE), LINK_DATAFLOW_ELLIPSE_HALF_SIZE);
	gradient.setColorAt(0, qRgba(142, 255, 102, 255));
	gradient.setColorAt(0.5f, qRgba(102, 215, 62, 255));
	gradient.setColorAt(1, Qt::transparent);
	this->ellipseItems[0] = scene->addEllipse(0, 0, LINK_DATAFLOW_ELLIPSE_RADIUS, LINK_DATAFLOW_ELLIPSE_RADIUS, QPen(Qt::transparent), QBrush(gradient));
	this->ellipseItems[0]->setZValue(DATAFLOWLAYER);
	this->ellipseItems[1] = scene->addEllipse(0, 0, LINK_DATAFLOW_ELLIPSE_RADIUS, LINK_DATAFLOW_ELLIPSE_RADIUS, QPen(Qt::transparent), QBrush(gradient));
	this->ellipseItems[1]->setZValue(DATAFLOWLAYER);
	this->ellipseItems[2] = scene->addEllipse(0, 0, LINK_DATAFLOW_ELLIPSE_RADIUS, LINK_DATAFLOW_ELLIPSE_RADIUS, QPen(Qt::transparent), QBrush(gradient));
	this->ellipseItems[2]->setZValue(DATAFLOWLAYER);
	this->ellipseItems[3] = scene->addEllipse(0, 0, LINK_DATAFLOW_ELLIPSE_RADIUS, LINK_DATAFLOW_ELLIPSE_RADIUS, QPen(Qt::transparent), QBrush(gradient));
	this->ellipseItems[3]->setZValue(DATAFLOWLAYER);

	// hide data flow ellipses
	this->ellipseItems[0]->hide();
	this->ellipseItems[1]->hide();
	this->ellipseItems[2]->hide();
	this->ellipseItems[3]->hide();
}

//------------------------------------------------------------------------------
/**
*/
void
LinkGraphics::RemoveFromScene(const Ptr<NodeGraphicsScene>& scene)
{
	n_assert(0 != this->linePathItem);
	scene->removeItem(this->linePathItem);
	scene->removeItem(this->ellipseItems[0]);
	scene->removeItem(this->ellipseItems[1]);
	scene->removeItem(this->ellipseItems[2]);
	scene->removeItem(this->ellipseItems[3]);
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

//------------------------------------------------------------------------------
/**
*/
void
LinkGraphics::RenderDataFlow()
{
	float t1 = this->dataFlowTime;
	float t2 = fmod(this->dataFlowTime + 0.2f, 1.0f);
	float t3 = fmod(this->dataFlowTime + 0.4f, 1.0f);
	float t4 = fmod(this->dataFlowTime + 0.6f, 1.0f);
	QPointF p1 = this->linePath.pointAtPercent(t1) - QPointF(LINK_DATAFLOW_ELLIPSE_HALF_SIZE, LINK_DATAFLOW_ELLIPSE_HALF_SIZE);
	QPointF p2 = this->linePath.pointAtPercent(t2) - QPointF(LINK_DATAFLOW_ELLIPSE_HALF_SIZE, LINK_DATAFLOW_ELLIPSE_HALF_SIZE);
	QPointF p3 = this->linePath.pointAtPercent(t3) - QPointF(LINK_DATAFLOW_ELLIPSE_HALF_SIZE, LINK_DATAFLOW_ELLIPSE_HALF_SIZE);
	QPointF p4 = this->linePath.pointAtPercent(t4) - QPointF(LINK_DATAFLOW_ELLIPSE_HALF_SIZE, LINK_DATAFLOW_ELLIPSE_HALF_SIZE);
	this->ellipseItems[0]->setPos(p1);
	this->ellipseItems[1]->setPos(p2);
	this->ellipseItems[2]->setPos(p3);
	this->ellipseItems[3]->setPos(p4);
	this->dataFlowTime = fmod(this->dataFlowTime + 0.01f, 1.0f);
}

} // namespace Nody
