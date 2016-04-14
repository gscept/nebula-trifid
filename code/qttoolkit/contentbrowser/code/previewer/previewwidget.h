#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::PreviewWidget
    
    Inherhits QtNebulaWidget and overrides input handling
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "qtfeature/qtnebulawidget.h"
#include "core/debug.h"
#include "util/string.h"

namespace ContentBrowser
{
class ContentBrowserApp;
class PreviewWidget : public QtFeature::QtNebulaWidget
{
public:
	/// constructor
	PreviewWidget(QWidget* parent);
	/// destructor
	virtual ~PreviewWidget();

	/// sets the nebula application to which callbacks from the gui shall be sent
	void SetCallbackApplication(ContentBrowserApp* app);

	/// overrides drag enter event
	void dragEnterEvent(QDragEnterEvent* e);
	/// overrides drop event
	void dropEvent(QDropEvent* e);

private:
	/// called if a texture gets dropped in the widget
	void OnTextureDropped(const Util::String& category, const Util::String& name, const Math::float2 pos);

	ContentBrowserApp* app;
}; 


//------------------------------------------------------------------------------
/**
*/
inline void 
PreviewWidget::SetCallbackApplication( ContentBrowserApp* app )
{
	n_assert(0 != app);
	this->app = app;
}

} // namespace ContentBrowser
//------------------------------------------------------------------------------