#pragma once
//------------------------------------------------------------------------------
/**
    @class Actions::BaseAction
    
    A base action is an undo/redoable command.
    
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
namespace Actions
{
class BaseAction : public Core::RefCounted
{
	__DeclareClass(BaseAction);
public:
	/// constructor
	BaseAction();
	/// destructor
	virtual ~BaseAction();

    /// cleanup the action
    virtual void Cleanup();

	/// undo
	virtual void Undo();
	/// redo
	virtual void Redo();
	/// performs actual action
	virtual void Do();
	/// discards action and all its changes
	virtual void Discard();

	/// tags the current version as the final version
	void TagAsFinal();

	/// returns true if action queue isn't empty
	bool IsFinal() const;

protected:
	int currentVersion;
	int finalVersion;
}; 
} // namespace Actions
//------------------------------------------------------------------------------