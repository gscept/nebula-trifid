//------------------------------------------------------------------------------
//  previewwidget.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "viewerwidget.h"
#include "leveleditor2app.h"


namespace LevelEditor2
{

//------------------------------------------------------------------------------
/**
*/
ViewerWidget::ViewerWidget(QWidget* parent) :
	QtNebulaWidget(parent)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ViewerWidget::~ViewerWidget()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ViewerWidget::enterEvent ( QEvent * event )
{
	QWidget::enterEvent(event);
}

//------------------------------------------------------------------------------
/**
*/
void 
ViewerWidget::leaveEvent ( QEvent * event )
{
	QWidget::leaveEvent(event);

}

} // namespace LevelEditor2