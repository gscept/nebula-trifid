#pragma once
//------------------------------------------------------------------------------
/**
    @class Particles::ParticleCurveWidget
    
    A particle curve widget describes the curve which resides within each envelope attribute.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
namespace Particles
{
class ParticleCurveWidget : public QGraphicsView
{
	Q_OBJECT
public:

	enum ConversionMode
	{
		FromPercent,
		ToPercent
	};

	/// constructor
	ParticleCurveWidget(QWidget* parent);
	/// destructor
	virtual ~ParticleCurveWidget();

	/// sets up widget
	void Setup(QPointF point0, QPointF point1, QPointF point2, QPointF point3);

	/// converts points to/from widget frame
	QPointF ConvertPoint(ConversionMode mode, QPointF point);

public slots:
	/// called whenever a point value has changed
	void SelectedPointValueChanged(QPointF point);

signals:
	/// emit when a point is selected
	void PointSelected(int point);
	/// emit when a point is updated
	void PointUpdated(QPointF point);

private:
	/// handles resize event
	void resizeEvent(QResizeEvent* e);
	/// overrides mouse press event
	void mousePressEvent(QMouseEvent* e);
	/// overrides mouse move event
	void mouseMoveEvent(QMouseEvent* e);
	/// overrides mouse release event
	void mouseReleaseEvent(QMouseEvent* e);

	/// updates connecting lines
	void UpdateLines();

	int selectedPoint;
	int activePoint;
	bool isDragging;
	QVector<QPointF> percentPoints;
	QVector<QPointF> dataPoints;
	QVector<QPointF> uiPoints;
	QVector<QGraphicsEllipseItem*> pointItems;
	QVector<QGraphicsLineItem*> lineItems;

	QGraphicsEllipseItem* dragItem;
}; 
} // namespace Particles
//------------------------------------------------------------------------------