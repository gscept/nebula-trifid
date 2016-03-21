#pragma once
//------------------------------------------------------------------------------
/**
    @class Actions::BaseAction
    
    A base action is an undo/redoable command.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
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
	/// performs action and makes it to current
	virtual void DoAndMakeCurrent();
	/// discards action and all its changes
	virtual void Discard();

	/// tags the current version as the final version
	void TagAsFinal();

	/// returns true if action queue isn't empty
	bool IsFinal() const;

protected:
	IndexT previousVersion;
	int numVersions;
	int currentVersion;
	int finalVersion;
}; 
} // namespace Actions
//------------------------------------------------------------------------------