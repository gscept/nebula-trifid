//------------------------------------------------------------------------------
//  nodegraphicspaletteview.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "nodegraphicspaletteview.h"
#include <QtOpenGL/QGLWidget>

namespace Nody
{

//------------------------------------------------------------------------------
/**
*/
NodeGraphicsPaletteView::NodeGraphicsPaletteView(QWidget* parent) :
	QGraphicsView(parent)
{
	// setup view
	this->setBackgroundBrush(QBrush(QColor(51,51,51, 255)));
	this->setMouseTracking(true);

	// setup GL viewport
	QGLFormat format;
	format.setSamples(8);
	format.setSampleBuffers(true);
	format.setDoubleBuffer(true);
	this->setViewport(new QGLWidget(format));
	this->setRenderHint(QPainter::HighQualityAntialiasing);

	// create graphics scene
	this->graphicsPalette = NodeGraphicsPalette::Create();
	this->setScene(this->graphicsPalette);
}

//------------------------------------------------------------------------------
/**
*/
NodeGraphicsPaletteView::~NodeGraphicsPaletteView()
{
	// empty
}

} // namespace Nody