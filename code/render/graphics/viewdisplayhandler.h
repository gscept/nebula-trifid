#pragma once
//------------------------------------------------------------------------------
/**
	View display handler is used as a callback to the views notifying them the window they are rendering to should react.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/displayeventhandler.h"

namespace Graphics
{
class View;
class ViewDisplayHandler : public CoreGraphics::DisplayEventHandler
{
	__DeclareClass(ViewDisplayHandler);
public:
	/// constructor
	ViewDisplayHandler();
	/// destructor
	virtual ~ViewDisplayHandler();

	/// set view
	void SetView(const Ptr<Graphics::View>& view);

protected:
	/// called when an event should be processed, override this method in your subclass
	virtual bool HandleEvent(const CoreGraphics::DisplayEvent& event);

	Ptr<Graphics::View> view;
};

//------------------------------------------------------------------------------
/**
*/
inline void
ViewDisplayHandler::SetView(const Ptr<Graphics::View>& view)
{
	this->view = view;
}

} // namespace CoreGraphics