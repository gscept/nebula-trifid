//------------------------------------------------------------------------------
//  actionreader.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/actionreader.h"
#include "managers/entitymanager.h"
#include "basegamefeature/basegameattr/basegameattributes.h"

namespace Script
{
__ImplementClass(ActionReader, 'ACRE', Core::RefCounted);
//------------------------------------------------------------------------------
/**
*/
ActionReader::ActionReader() :
    currToken(0),
    actionStringVersion(-1),
    actionString("")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ActionReader::~ActionReader()
{
}

//------------------------------------------------------------------------------
/**
*/
void
ActionReader::PutClass(Ptr<Core::RefCounted>& refcounted)
{
    this->actionString.Append(refcounted->GetRtti()->GetName());
    this->actionString.Append(LIMITER);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Actions::Action>
ActionReader::GetAction()
{
    Ptr<Actions::Action> genericAction;
    Util::String className = this->tokens[this->currToken];
	Ptr<Core::RefCounted> newAction = Core::Factory::Instance()->Create(className);
    n_assert2(newAction->IsA(Actions::Action::RTTI), "action reader: wrong action type");
	genericAction = newAction.cast<Actions::Action>();
    genericAction->Read(this);
    return genericAction;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Conditions::Condition>
ActionReader::GetCondition()
{
    Ptr<Conditions::Condition> genericCondition;
    Util::String className = this->tokens[this->currToken];
    Ptr<Core::RefCounted> newCondition = Core::Factory::Instance()->Create(className);
    n_assert2(newCondition->IsA(Conditions::Condition::RTTI), "action reader: wrong condition type");
    genericCondition = newCondition.cast<Conditions::Condition>();
    genericCondition->Read(this);
    return genericCondition;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Game::Entity>
ActionReader::GetEntity()
{
    n_assert2(this->currToken < this->tokens.Size(), "action reader: no tokens left");
    const Util::String& guid = this->GetString();
    if ("none" == guid)
    {
        return Ptr<Game::Entity>(0);
    }
    else
    {
		Attr::Attribute guidAttr(Attr::Guid, Util::Guid::FromString(guid));
        Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByAttr(guidAttr, false);
        return entity;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ActionReader::PutEntity(Ptr<Game::Entity> entity)
{
    if(entity.isvalid())
    {
        PutString(entity->GetGuid(Attr::Guid).AsString());
    }
    else
    {
        PutString("none");
    }
}

}; // namespace Util
