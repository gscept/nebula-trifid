//------------------------------------------------------------------------------
//  particlecurvewidget.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "particlecurvewidget.h"
#include <QPainter>
#include <QLineF>
#include <QMouseEvent>
#include <QtOpenGL/QGLWidget>

#define POINTSIZE 12
#define LINEWIDTH 2
#define POINTCOUNT 4

#define HALFSIZE QPointF(POINTSIZE/2, POINTSIZE/2)
QColor baseFillColor = qRgb(255, 153, 51);
QColor highlightedColor = qRgb(235, 135, 45);
QColor lineColor = qRgb(255, 255, 255);
namespace Particles
{

//------------------------------------------------------------------------------
/**
*/
ParticleCurveWidget::ParticleCurveWidget(QWidget* parent) : 
	QGraphicsView(parent),
	dragItem(0)
{
	this->activePoint = -1;
	this->selectedPoint = 0;
	this->isDragging = false;

	this->setRenderHint(QPainter::Antialiasing);

	// create scene
	QGraphicsScene* scene = new QGraphicsScene;

	this->setScene(scene);
	this->setSceneRect(this->rect());

	QRectF rect = this->rect();
	this->percentPoints.clear();

	this->update();
	const QPalette& palette = this->palette();
	//const QPalette& palette = ((QWidget*)(parent->parent()))->palette();
	baseFillColor = palette.button().color();
	highlightedColor = palette.light().color();

	// create pens for ellipses
	QPen stroke(lineColor);
	stroke.setWidth(LINEWIDTH);
	QBrush fill(baseFillColor);

	int i;
	for (i = 0; i < POINTCOUNT; i++)
	{
		this->percentPoints << QPointF(i / (float)(POINTCOUNT-1), 0);
		this->dataPoints << this->ConvertPoint(FromPercent, this->percentPoints[i]);
		this->uiPoints << this->mapToScene(this->dataPoints[i].toPoint());

		QGraphicsEllipseItem* item = this->scene()->addEllipse(QRectF(0, 0, POINTSIZE, POINTSIZE), stroke, fill);
		item->setZValue(1);
		item->setPos(this->uiPoints[i] - HALFSIZE);
		this->pointItems << item;
	}

	// create lines
	for (int i = 0; i < POINTCOUNT-1; i++)
	{
		// get points
		QPointF pos1 = this->pointItems[i]->pos() + HALFSIZE;
		QPointF pos2 = this->pointItems[i+1]->pos() + HALFSIZE;
		QGraphicsLineItem* item = this->scene()->addLine(QLineF(pos1, pos2), stroke);
		item->setZValue(0);
		this->lineItems << item;
	}

}

//------------------------------------------------------------------------------
/**
*/
ParticleCurveWidget::~ParticleCurveWidget()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleCurveWidget::Setup( QPointF point0, QPointF point1, QPointF point2, QPointF point3 )
{
	// calculate percental points
	this->percentPoints.clear();
	this->percentPoints << point0 
						<< point1
						<< point2 
						<< point3;

	// setup actual data
	this->dataPoints.clear();
	this->dataPoints << this->ConvertPoint(FromPercent, point0)
					 << this->ConvertPoint(FromPercent, point1)
					 << this->ConvertPoint(FromPercent, point2)
					 << this->ConvertPoint(FromPercent, point3);

	// setup ui points
	this->uiPoints.clear();
	this->uiPoints << this->mapToScene(dataPoints[0].toPoint()) 
				   << this->mapToScene(dataPoints[1].toPoint()) 
				   << this->mapToScene(dataPoints[2].toPoint())
				   << this->mapToScene(dataPoints[3].toPoint());

	// calculate positions of items
	int i;
	for (i = 0; i < 4; i++)
	{
		this->pointItems[i]->setPos(this->uiPoints[i] - HALFSIZE);
	}

	// finally update lines
	this->UpdateLines();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleCurveWidget::mousePressEvent( QMouseEvent* e )
{
	this->activePoint = -1;

	// recolor old point
	if (this->selectedPoint != -1)
	{
		// create pen
		QPen pen(lineColor);
		pen.setWidth(LINEWIDTH);

		// set color
		this->pointItems[this->selectedPoint]->setPen(pen);
	}

	// find item
	QGraphicsItem* item = this->scene()->itemAt(this->mapToScene(e->pos()));

	// test if it's a point
	QGraphicsEllipseItem* point = dynamic_cast<QGraphicsEllipseItem*>(item);

	// if we have a point
	if (point)
	{
		// get index of point
		int pointIndex = this->pointItems.indexOf(point);

		// create pen
		QPen pen(highlightedColor);
		pen.setWidth(LINEWIDTH);

		// color point differently
		point->setPen(pen);

		// set point index, point item and notify that we are dragging
		this->activePoint = this->selectedPoint = pointIndex;
		this->dragItem = point;
		this->isDragging = true;

		// emit signal
		emit this->PointSelected(this->activePoint);
	}
	else
	{
		this->isDragging = false;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleCurveWidget::mouseMoveEvent( QMouseEvent* e )
{
	if (this->isDragging)
	{
		// get position
		QPointF pos = e->pos();
		
		// handle first point and last point specially
		if (this->activePoint == 0)
		{
			pos.setX(0);
		}
		else if (this->activePoint == POINTCOUNT-1)
		{
			pos.setX(this->width());
		}
		else
		{
			// get next point
			QPointF nextPos = this->dataPoints[this->activePoint+1];
			QPointF prevPos = this->dataPoints[this->activePoint-1];

			// clamp X
			if (pos.x() < prevPos.x() - 0.01f)
			{
				pos.setX(prevPos.x() - 0.01f);
			}
			else if (pos.x() > nextPos.x() + 0.01f)
			{
				pos.setX(nextPos.x() + 0.01f);
			}
		}

		// clamp Y
		if (pos.y() > this->height())
		{
			pos.setY(this->height());
		}
		else if (pos.y() < 0)
		{
			pos.setY(0);
		}

		// convert to percent
		this->percentPoints[this->activePoint] = this->ConvertPoint(ToPercent, pos);
		this->dataPoints[this->activePoint] = this->ConvertPoint(FromPercent, this->percentPoints[this->activePoint]);
		this->uiPoints[this->activePoint] = this->mapToScene(this->dataPoints[this->activePoint].toPoint());

		// update position of graphics
		this->pointItems[this->activePoint]->setPos(this->uiPoints[this->activePoint] - HALFSIZE);

		// update lines
		this->UpdateLines();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleCurveWidget::resizeEvent( QResizeEvent* e )
{
	// resize window
	QGraphicsView::resizeEvent(e);

	// then move points again
	int i;
	for (i = 0; i < POINTCOUNT; i++)
	{
		this->dataPoints[i] = this->ConvertPoint(FromPercent, this->percentPoints[i]);
		this->uiPoints[i] = this->mapToScene(this->dataPoints[i].toPoint());
		this->pointItems[i]->setPos(this->uiPoints[i] - HALFSIZE);
	}		

	// update lines
	this->UpdateLines();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleCurveWidget::mouseReleaseEvent( QMouseEvent* e )
{
	// emit signal if a point has been dragged
	if (this->dragItem)
	{
		emit this->PointUpdated(this->percentPoints[this->activePoint]);
	}
	this->isDragging = false;
	this->activePoint = -1;
	this->dragItem = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleCurveWidget::SelectedPointValueChanged( QPointF point )
{
	this->percentPoints[this->selectedPoint] = point;
	this->dataPoints[this->selectedPoint] = ConvertPoint(FromPercent, this->percentPoints[this->selectedPoint]);
	this->uiPoints[this->selectedPoint] = this->mapToScene(this->dataPoints[this->selectedPoint].toPoint());

	// get point and move
	this->pointItems[this->selectedPoint]->setPos(this->uiPoints[this->selectedPoint] - HALFSIZE);

	// also update lines
	this->UpdateLines();
}

//------------------------------------------------------------------------------
/**
	In the actual widget, we simply treat our points as a percentual representation of the width of the widget.
	So here we convert between percentages and their actual position...
*/
QPointF 
ParticleCurveWidget::ConvertPoint( ConversionMode mode, QPointF point )
{

	int height = this->height();
	int width = this->width();
	switch (mode)
	{
		case FromPercent:
			{
				float x = point.x();
				float y = 1 - point.y();
				x *= width;
				y *= height;
				return QPointF(x, y);
			}
			break;
		case ToPercent:
			{
				float x = point.x();
				float y = point.y();
				x /= width;
				y /= height;
				y = 1 - y;
				return QPointF(x, y);
			}
			break;
	}

	// bullshit MSVC 
	return QPointF();
}

//------------------------------------------------------------------------------
/**
*/
void 
ParticleCurveWidget::UpdateLines()
{
	int i;
	for (i = 0; i < POINTCOUNT-1; i++)
	{
		// get points
		QPointF pos1 = this->pointItems[i]->pos() + HALFSIZE;
		QPointF pos2 = this->pointItems[i+1]->pos() + HALFSIZE;
		QGraphicsLineItem* item = this->lineItems[i];
		item->setLine(QLineF(pos1, pos2));
		item->setZValue(0);		
	}
}

} // namespace Particles