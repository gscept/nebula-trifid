#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::Action
    
    Abstract class for undoable actions. Note that Perform() and Revert() must
	only be called by the ActionManager via the method PerformAction().
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace LevelEditor2
{
class Action : public Core::RefCounted
{
protected:
	friend class ActionManager;

	/// perform the action
	virtual void Perform() = 0;
	/// undo the performed action
	virtual void Undo() = 0;
};

} // namespace LevelEditor2
//------------------------------------------------------------------------------