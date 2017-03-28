#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::NebulaWidget
    
    Creates a Nebula renderer widget using a Qt Widget
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QObject>
#ifdef __USE_QT4
#include <QtGui/QFrame>
#else
#include <QtWidgets/QFrame>
#endif
#include <QResizeEvent>
#include "graphics/view.h"

namespace QtFeature
{
class QtNebulaWidget : public QFrame
{
    Q_OBJECT
public:
    /// constructor
    QtNebulaWidget(QWidget* parent);
	/// constructor
	QtNebulaWidget();
    /// destructor
    virtual ~QtNebulaWidget();

    /// overrides paintEngine to return null
    virtual QPaintEngine* paintEngine() const;

    /// resizes window
    void resizeEvent(QResizeEvent* e);

    /// override key pressed
    void keyPressEvent(QKeyEvent *event);
    /// override key released
    void keyReleaseEvent(QKeyEvent* event);

	/// override focus in event
	void focusInEvent(QFocusEvent* event);
	/// override focus out event
	void focusOutEvent(QFocusEvent* event);

	/// set view so we can toggle it whenever 
	void SetView(const Ptr<Graphics::View>& view);

	/// set window id in Nebula to be used by this widget
	void SetWindowId(const int wid);
    /// generates window data structure
    void GenerateWindowData();
    /// returns window data as a void*, delete when usage is no longer needed
    void* GetWindowData() const;
    /// returns size of window data
    int GetWindowDataSize() const;

private:
	
	Ptr<Graphics::View> view;
    void* windowData;
    int windowDataSize;
	int windowId;
}; 

//------------------------------------------------------------------------------
/**
	return NULL pointer, which means we will handle our own rendering
*/
inline QPaintEngine* 
QtNebulaWidget::paintEngine() const
{
    return NULL;
}

//------------------------------------------------------------------------------
/**
*/
inline void
QtNebulaWidget::SetView(const Ptr<Graphics::View>& view)
{
	this->view = view;
	this->windowId = view->GetWindowId();
}

//------------------------------------------------------------------------------
/**
*/
inline void
QtNebulaWidget::SetWindowId(const int wid)
{
	this->windowId = wid;
}

//------------------------------------------------------------------------------
/**
*/
inline void*
QtNebulaWidget::GetWindowData() const 
{
    return this->windowData;
}

//------------------------------------------------------------------------------
/**
*/
inline int
QtNebulaWidget::GetWindowDataSize() const
{
    return this->windowDataSize;
}

} // namespace ContentBrowser
//------------------------------------------------------------------------------
