#pragma once
//------------------------------------------------------------------------------
/**
    @class UI::UiEventHandler
    
    Base class for handling UiEvents. Override HandleEvent to implement your
	own application gui logic.
*/
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace UI
{
class UiEvent;

class UiEventHandler : public Core::RefCounted
{
	__DeclareClass(UiEventHandler);
public:
	/// constructor
	UiEventHandler();
	/// destructor
	virtual ~UiEventHandler();

	/// override this method to handle your user interface events
	virtual void HandleEvent(const UiEvent& e);
};

}; // namespace UI
