//------------------------------------------------------------------------------
//  nebulawidget.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "qtnebulawidget.h"
#include "graphics/graphicsprotocol.h"
#include "events/event.h"
#include "graphics/graphicsinterface.h"

#if __WIN32__
    #include <windows.h>

    // create struct for data
    struct Win32Data
	{
		int width;
		int height;
		HWND window;
    };
#elif __LINUX__
    #include <X11/Xlib.h>
    #include <QX11Info>
    #include <GL/glx.h>

    // create struct for data
    struct X11Data
    {
        int width;
        int height;
        Colormap map;
        Window handle;
        int screen;
        Display* display;
        XVisualInfo* visual;
    };
#endif
#include "qtfeatureunit.h"


using namespace CoreGraphics;
namespace QtFeature
{

//------------------------------------------------------------------------------
/**
*/
QtNebulaWidget::QtNebulaWidget(QWidget* parent) : 
	windowId(0),
	view(0),
	QFrame(parent)
	
{
    // tell Qt to render the Widget to screen, and to handle it as a native window
	//this->setAttribute(Qt::WA_PaintOnScreen);
	this->setAttribute(Qt::WA_NativeWindow);
	this->setAttribute(Qt::WA_NoChildEventsForParent);
	this->setAttribute(Qt::WA_PaintUnclipped);
    this->setFocusPolicy(Qt::NoFocus);
}


//------------------------------------------------------------------------------
/**
*/
QtNebulaWidget::QtNebulaWidget() :
	windowId(0)
{
	// tell Qt to render the Widget to screen, and to handle it as a native window
	//this->setAttribute(Qt::WA_PaintOnScreen);
	this->setAttribute(Qt::WA_NativeWindow);
	this->setAttribute(Qt::WA_NoChildEventsForParent);
	this->setAttribute(Qt::WA_PaintUnclipped);
	this->setFocusPolicy(Qt::NoFocus);
}
//------------------------------------------------------------------------------
/**
*/
QtNebulaWidget::~QtNebulaWidget()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void QtNebulaWidget::GenerateWindowData()
{
#if __WIN32__
    // create instance of win32 data
    Win32Data data;
	data.width = Math::n_max(1, this->size().width());
	data.height = Math::n_max(1, this->size().height());
#ifdef __USE_QT4
    data.window = this->effectiveWinId();
#else
	//FIXME this is supposed to be ok, but only inofficially
    // http://lists.qt-project.org/pipermail/interest/2013-June/007650.html
    data.window = (HWND)this->winId();    
#endif

    // allocate memory for window data pointer
    this->windowData = malloc(sizeof(Win32Data));
    this->windowDataSize = sizeof(Win32Data);

    // copy from struct to void*
    memcpy(this->windowData, &data, sizeof(Win32Data));
#else
    // get info from Qt
    X11Data data;
    QX11Info info = this->x11Info();

    data.height = this->size().height();
    data.width = this->size().width();
    data.map = info.colormap();
    data.handle = this->winId();
    data.screen = info.screen();
    data.display = QX11Info::display();

    
    //XVisualInfo* visInf;
    //XMatchVisualInfo(data.display, info.screen(), info.depth(), DirectColor, data.visual);
    //data.visual->visual = (Visual*)info.visual();
    //data.visual->visual->visualid = XVisualIDFromVisual(data.visual->visual);
    

    // allocate memory for window data pointer
    this->windowData = malloc(sizeof(X11Data));
    this->windowDataSize = sizeof(X11Data);

    // copy from struct to data
    memcpy(this->windowData, &data, sizeof(X11Data));
#endif
}



//------------------------------------------------------------------------------
/**
*/
void 
QtNebulaWidget::keyPressEvent(QKeyEvent* e)
{
	QFrame::keyPressEvent(e);
}

//------------------------------------------------------------------------------
/**
*/
void 
QtNebulaWidget::keyReleaseEvent(QKeyEvent* e)
{
	QFrame::keyReleaseEvent(e);
}

//------------------------------------------------------------------------------
/**
*/
void
QtNebulaWidget::focusInEvent(QFocusEvent* event)
{
	//n_assert(this->view.isvalid());
	//this->view->SetUpdatePerFrame(true);
}

//------------------------------------------------------------------------------
/**
*/
void
QtNebulaWidget::focusOutEvent(QFocusEvent* event)
{
	//n_assert(this->view.isvalid());
	//this->view->SetUpdatePerFrame(false);
}

//------------------------------------------------------------------------------
/**
*/
void
QtNebulaWidget::resizeEvent(QResizeEvent* e)
{
    if (Graphics::GraphicsInterface::HasInstance())
    {
        QSize size = e->size();

        // update display mode
        Ptr<Graphics::UpdateWindow> msg = Graphics::UpdateWindow::Create();
        CoreGraphics::DisplayMode mode;
        mode.SetWidth(size.width());
        mode.SetHeight(size.height());
		mode.SetAspectRatio(Math::n_max(size.width() / float(size.width()), 0.01f));
        msg->SetFullscreen(false);
        msg->SetWindowData(Util::Blob(this->windowData, this->windowDataSize));
        msg->SetDisplayMode(mode);
		msg->SetWindow(this->windowId);
        Graphics::GraphicsInterface::Instance()->Send(msg.upcast<Messaging::Message>());  
    }
}


} // namespace ContentBrowser