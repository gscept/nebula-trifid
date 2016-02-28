#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::ViewerWidget
    
    Inherhits QtNebulaWidget and overrides input handling
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include "qtfeature/qtnebulawidget.h"
#include "core/debug.h"

//------------------------------------------------------------------------------
namespace LevelEditor2
{
class LevelEditor2App;

class ViewerWidget : public QtFeature::QtNebulaWidget
{
public:
	/// constructor
	ViewerWidget(QWidget* parent);
	/// destructor
	virtual ~ViewerWidget();

	/// sets the nebula application to which callbacks from the gui shall be sent
	void SetCallbackApplication(LevelEditor2App* app);

protected:
	virtual void enterEvent ( QEvent * event );
	virtual void leaveEvent ( QEvent * event );

private:
	LevelEditor2App* app;
}; 


//------------------------------------------------------------------------------
/**
*/
inline void 
ViewerWidget::SetCallbackApplication( LevelEditor2App* app )
{
	n_assert(0 != app);
	this->app = app;
}

} // namespace LevelEditor2
//------------------------------------------------------------------------------