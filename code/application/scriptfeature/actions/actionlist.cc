//------------------------------------------------------------------------------
//  masterEvents/script/actionlist.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/actions/actionlist.h"
#include "scriptfeature/log/infolog.h"
#include "scriptfeature/actionreader.h"

namespace Actions
{
__ImplementClass(Actions::ActionList, 'ACLI', Actions::Action);

//------------------------------------------------------------------------------
/**
    execute action in action list
*/
void
ActionList::Execute()
{
    // set trigger entities and entities attached to if it has not already happend
    IndexT i;
    if (this->HasEntity())
    {
        for (i = 0; i < this->GetActionList().Size(); i++)
        {
            this->actionlist[i]->SetEntity(this->GetEntity());
        }
    }
    // assert all action are valid
    //this->Assert();
    
    // execute actions
    for (i = 0; i < this->GetActionList().Size(); i++)
    {
        this->actionlist[i]->Execute();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ActionList::Assert()
{
    n_assert(this->actionlist.Size() > 0);
    IndexT i;
    for (i = 0; i < this->actionlist.Size(); i++)
    {
        this->actionlist[i]->Assert();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ActionList::Assert(Ptr<Script::InfoLog> infoLog)
{
    n_assert(this->actionlist.Size() > 0);

    bool noErrors = true;

    IndexT i;
    for (i = 0; i < this->actionlist.Size(); i++)
    {
        if (!this->actionlist[i]->Assert(infoLog))
        {
            noErrors = false;
        }
    }

    return noErrors;
}

//------------------------------------------------------------------------------
/**
*/    
void 
ActionList::SetActionList(const Util::Array<Ptr<Actions::Action> >& a)
{
    this->actionlist = a;
}

//------------------------------------------------------------------------------
/**
*/    
const Util::Array<Ptr<Actions::Action> >& 
ActionList::GetActionList() const
{
    return this->actionlist;
}

//------------------------------------------------------------------------------
/**
*/    
void 
ActionList::SetEntity(const Ptr<Game::Entity>& entity)
{
    Action::SetEntity(entity);

    // set entity on all actions in list
    IndexT i;
    for (i = 0; i < this->actionlist.Size(); i++)
    {
        this->actionlist[i]->SetEntity(entity);
    }
}
//------------------------------------------------------------------------------
/**
*/
void
ActionList::Write(const Ptr<Script::ActionReader>& actionReader)
{
    Action::Write(actionReader);
    actionReader->PutInt(this->actionlist.Size());
    IndexT i;
    for (i = 0; i < this->actionlist.Size(); i++)
    {
        this->actionlist[i]->Write(actionReader);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ActionList::Read(const Ptr<Script::ActionReader>& actionReader)
{
    Action::Read(actionReader);

    int arraySize = actionReader->GetInt();
    IndexT i;
    for (i = 0; i < arraySize; i++)
    {
        Ptr<Actions::Action> action = (Actions::Action*)actionReader->GetAction();
        n_assert(action != 0);
        if (action->IsA(Actions::Action::RTTI))
        {
            this->actionlist.Append(static_cast<Actions::Action*>(action.get()));
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ActionList::OnActivate()
{
    IndexT i;
    for (i = 0; i < this->actionlist.Size(); i++)
    {
        this->actionlist[i]->OnActivate();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ActionList::OnDeactivate()
{
    IndexT i;
    for (i = 0; i < this->actionlist.Size(); i++)
    {
        this->actionlist[i]->OnDeactivate();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ActionList::Init()
{
    IndexT i;
    for (i = 0; i < this->actionlist.Size(); i++)
    {
        this->actionlist[i]->Init();
    }
}
}; // namespace Actions
