#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::NebulaWidget
    
    Creates a Nebula renderer widget using a Qt Widget
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QObject>
#include <QtGui/QFrame>
#include <QResizeEvent>


namespace QtFeature
{
class QtNebulaWidget : public QFrame
{
    Q_OBJECT
public:
    /// constructor
    QtNebulaWidget(QWidget* parent);
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

    /// generates widnow data structure
    void GenerateWindowData();
    /// returns window data as a void*, delete when usage is no longer needed
    void* GetWindowData() const;
    /// returns size of window data
    int GetWindowDataSize() const;

private:
	
    void* windowData;
    int windowDataSize;
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
