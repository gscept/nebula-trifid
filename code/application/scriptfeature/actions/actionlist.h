#pragma once
//------------------------------------------------------------------------------
/**
    @class Actions::ActionList

    contains a list of masterEvents

    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "scriptfeature/actions/action.h"

//------------------------------------------------------------------------------
namespace Actions
{

class ActionList : public Actions::Action
{
    __DeclareClass(ActionList);
    
public:   
    /// called when state is activated
    virtual void OnActivate();
    /// called when state is deactivated
    virtual void OnDeactivate();
    /// execute the actions in action list
    virtual void Execute();
    /// assert all actions in action list are valid 
    virtual void Assert();
    /// like Assert() but adds errors to the info log object instead of closing the application
    virtual bool Assert(Ptr<Script::InfoLog> infoLog);

    /// set action list
    void SetActionList(const Util::Array<Ptr<Actions::Action> >& a);
    /// get action List
    const Util::Array<Ptr<Actions::Action> >& GetActionList() const;

	/// Set target entity to `v'.
	virtual void SetEntity(const Ptr<Game::Entity>& v);

    /// write to action reader
    virtual void Write(const Ptr<Script::ActionReader>& actionReader);
    /// read from action reader
    virtual void Read(const Ptr<Script::ActionReader>& actionReader);    
    /// init after creation, parse args and set entity
    virtual void Init();

protected:

    Util::Array<Ptr<Actions::Action> > actionlist;
};

__RegisterClass(ActionList);

}; // namespace Actions
//------------------------------------------------------------------------------    