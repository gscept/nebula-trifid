#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::NodeGraphicsView
    
    Implements a Nody graphics view, which should have a NodyGraphicsScene as its render target.
	You need one of these paired with a NodyGraphicsScene in order to encompass a zoom-in and out feature, 
	as well as having a trash icon.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QGraphicsView>
#include <QPropertyAnimation>
#include <QGraphicsPixmapItem>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include "nodegraphicsscene.h"

namespace Nody
{
class NodeGraphicsSceneView : public QGraphicsView
{
	Q_OBJECT
	Q_PROPERTY(qreal scale READ GetScale WRITE SetScale)
	Q_PROPERTY(QPoint center READ GetCenter WRITE SetCenter)
public:
	/// constructor
	NodeGraphicsSceneView(QWidget* parent);
	/// destructor
	virtual ~NodeGraphicsSceneView();

	/// handles wheel events
	void wheelEvent(QWheelEvent *event);
	/// handles a mouse event
	void mouseMoveEvent(QMouseEvent *event);
	/// handles key press
	void keyPressEvent(QKeyEvent *event);
	/// handles key release event
	void keyReleaseEvent(QKeyEvent *event);
    /// handles show event
    void showEvent(QShowEvent* event);

	/// overrides the set scene
	void SetScene(const Ptr<NodeGraphicsScene>& scene);
	/// sets the scroll speed for the zooming
	void SetScrollSpeed(qreal inSpeed);

	/// starts an animation for zooming
	void AnimateScale(qreal zoom, const QPoint& pos);
	/// sets the scale for the scene
	void SetScale(qreal scale);
	/// gets the current scale for the scene
	qreal GetScale();
	/// resets the scale
	void ResetScale();
    /// sets the size of the scene
    void SetSize(const QRectF& size);
	/// sets the center position of the scene
	void SetCenter(const QPoint& center);
	/// gets the center of the scene
	const QPoint& GetCenter();
    /// resets center to origin
    void ResetCenter();
	/// set the origin point, use this carefully
	void SetOrigin(const QPointF& origin);

	/// gets graphics scene
	const Ptr<NodeGraphicsScene>& GetScene() const;

	/// adjust size of scene
	void AdjustScene();

private slots:
	/// called whenever animation has finished
	void OnAnimationFinished();

private:
	Ptr<NodeGraphicsScene> graphicsScene;
	QPropertyAnimation* zoom;
	QPropertyAnimation* move;


    QPointF origin;
    QRectF size;
	QPoint center;
	qreal zoomScale;
	qreal scrollAmount;
	qreal scrollSpeed;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const QPoint& 
NodeGraphicsSceneView::GetCenter()
{
	return this->center;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
NodeGraphicsSceneView::SetScrollSpeed( qreal inSpeed )
{
	this->scrollSpeed = inSpeed;
}

//------------------------------------------------------------------------------
/**
*/
inline qreal 
Nody::NodeGraphicsSceneView::GetScale()
{
	return this->zoomScale;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<NodeGraphicsScene>& 
NodeGraphicsSceneView::GetScene() const
{
	return this->graphicsScene;
}

} // namespace Nody
//------------------------------------------------------------------------------