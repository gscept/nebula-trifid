//------------------------------------------------------------------------------
//  nodegraphicsview.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "nodegraphicssceneview.h"
#include "nody_config.h"
#include "io/ioserver.h"
#include "io/stream.h"
#include "variation/variation.h"
#include <QPlastiqueStyle>
#include <QtOpenGL/QGLWidget>
#include <QApplication>
#include <QAction>

#define ZOOMDURATION 300
#define MOVEDURATION 700

//#define __SHADY_OPENGL_VIEWPORT__

using namespace Math;
using namespace IO;
namespace Nody
{

//------------------------------------------------------------------------------
/**
*/
NodeGraphicsSceneView::NodeGraphicsSceneView(QWidget* parent) : 
	QGraphicsView(parent),
	scrollSpeed(1.01f), 
	scrollAmount(1.0f)
{
	this->setFocusPolicy(Qt::StrongFocus);

	// setup view
	this->setBackgroundBrush(QBrush(QColor(51, 51, 51, 255)));
	this->zoomScale = 1;
	this->setMouseTracking(true);

#ifdef __SHADY_OPENGL_VIEWPORT__
	// setup GL viewport
	QGLFormat format = QGLFormat(QGL::SampleBuffers);
	format.setSamples(4);
	format.setSampleBuffers(true);
	format.setDoubleBuffer(true);
	QGLWidget* widget = new QGLWidget(format);
	//widget->setAutoFillBackground(true);
	this->setViewport(widget);	
#else
	this->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
#endif

	// create graphics scene
	this->graphicsScene = NodeGraphicsScene::Create();
	this->SetScene(this->graphicsScene);
    QRectF scene = this->sceneRect();
    scene.setX(-2500);
    scene.setY(-2500);
    scene.setWidth(5000);
    scene.setHeight(5000);
    this->setSceneRect(scene);    
	//this->AdjustScene();

	// setup graphics scene
	this->graphicsScene->Setup();

	// create signal for delete
	QAction* deleteAction = new QAction(this);
	deleteAction->setShortcut(Qt::Key_Delete);
	connect(deleteAction, SIGNAL(triggered()), this->graphicsScene, SLOT(DeleteCurrent()));
	this->addAction(deleteAction);

	// create zoom animation properties
	this->zoom = new QPropertyAnimation(this, "scale");
	this->move = new QPropertyAnimation(this, "center");
	zoom->setDuration(ZOOMDURATION);
	move->setDuration(MOVEDURATION);

	connect(this->zoom, SIGNAL(finished()), this, SLOT(OnAnimationFinished()));
	connect(this->move, SIGNAL(finished()), this, SLOT(OnAnimationFinished()));
}

//------------------------------------------------------------------------------
/**
*/
NodeGraphicsSceneView::~NodeGraphicsSceneView()
{
	// discard scene
	this->graphicsScene->Discard();
	this->graphicsScene = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsSceneView::wheelEvent(QWheelEvent *event)
{
	QPointF scenePos = this->mapToScene(event->pos());
	QGraphicsItem* item = this->scene()->itemAt(scenePos);
	scalar currentScroll = 1.0;
	scalar prevScale = this->scrollAmount;
	if (event->delta() > 0)
	{
		this->scrollAmount = scrollSpeed;
	}
	else
	{
		this->scrollAmount = 1 / scrollSpeed;
	}

	event->accept();		
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsSceneView::mouseMoveEvent(QMouseEvent *event)
{
	QGraphicsView::mouseMoveEvent(event);
	this->center = this->mapToScene(this->rect().center()).toPoint();
	this->AdjustScene();
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsSceneView::keyPressEvent(QKeyEvent *event)
{
	QGraphicsView::keyPressEvent(event);
	if (event->modifiers() == Qt::ShiftModifier)
	{
		this->setDragMode(QGraphicsView::ScrollHandDrag);
	}
    else if (event->key() == Qt::Key_Space)
    {
        this->centerOn(this->origin);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsSceneView::keyReleaseEvent(QKeyEvent *event)
{
	QGraphicsView::keyReleaseEvent(event);
	this->setDragMode(QGraphicsView::RubberBandDrag);
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsSceneView::showEvent(QShowEvent* event)
{
    QGraphicsView::showEvent(event);
    this->origin = this->viewport()->rect().center();
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsSceneView::SetScale(qreal scale)
{
	NodeGraphicsScene* nodeScene = dynamic_cast<NodeGraphicsScene*>(this->scene());
	if (nodeScene)
	{
		nodeScene->OnScale(scale);
	}
	this->center *=  1 / this->zoomScale;
	this->zoomScale = scale;
	this->center *= this->zoomScale;
	this->SetCenter(this->center);
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphicsSceneView::ResetScale()
{
	this->SetScale(1);
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsSceneView::SetSize(const QRectF& size)
{
    this->size = size;
    this->setSceneRect(size);
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsSceneView::SetCenter(const QPoint& center)
{
	this->center = center;
	this->centerOn(center);
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphicsSceneView::ResetCenter()
{
    this->centerOn(this->origin);
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsSceneView::SetOrigin(const QPointF& origin)
{
	this->origin = origin;
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphicsSceneView::AdjustScene()
{
	QRectF rect = this->viewport()->rect();
	rect = rect.united(this->scene()->itemsBoundingRect());
    rect.setWidth(rect.width() * 1.2f);
    rect.setHeight(rect.height() * 1.2f);
	//this->setSceneRect(rect);
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsSceneView::SetScene(const Ptr<NodeGraphicsScene>& scene)
{
	QGraphicsView::setScene(scene);
	this->graphicsScene = scene;
}

//------------------------------------------------------------------------------
/**
*/
void
NodeGraphicsSceneView::AnimateScale(qreal zoom, const QPoint& pos)
{
	if (zoom != this->zoomScale)
	{
		if (this->zoom->state() == QAbstractAnimation::Running)
		{
			this->zoom->stop();
		}
		this->zoom->setStartValue(this->zoomScale);
		this->zoom->setEndValue(zoom);
		this->zoom->setEasingCurve(QEasingCurve::OutQuart);
		this->zoom->setDuration(ZOOMDURATION);
		this->zoom->start(QAbstractAnimation::KeepWhenStopped);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphicsSceneView::OnAnimationFinished()
{
	// empty
}

} // namespace Nody
