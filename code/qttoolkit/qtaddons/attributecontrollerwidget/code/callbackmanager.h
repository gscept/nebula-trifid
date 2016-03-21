#pragma once
//------------------------------------------------------------------------------
/**
    @class QtAttributeControllerAddon::CallbackManager
    
    Base manager for handling callbacks from attribute controllers, this only
	updates the attribute tables and nothing more. Derive from this to add extra
	functionality for callbacks.

	(This probably should not be a manager, you can change it if you want)
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/

#include "core/singleton.h"
#include "game/manager.h"

//------------------------------------------------------------------------------
namespace QtAttributeControllerAddon
{
class BaseAttributeController;
class VariableControllerWidget;

class CallbackManager: public Game::Manager
{
	__DeclareClass(CallbackManager);
	__DeclareSingleton(CallbackManager);
public:
	/// constructor
	CallbackManager();
	/// destructor
	~CallbackManager();

protected:
	friend class BaseAttributeController;
	friend class VariableControllerWidget;

	/// updates the attribute value with the controllers value
	virtual void OnValueChanged(BaseAttributeController* controller);
	/// updates the variable with the controllers value
	virtual void OnValueChanged(VariableControllerWidget* controller);
};
}