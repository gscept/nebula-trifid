#pragma once
//------------------------------------------------------------------------------
/**
    @class Conditions::Not
    
    Returns the inverted result of an given condition.

    (C) 2006 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "scriptfeature/conditions/condition.h"

//------------------------------------------------------------------------------
namespace Conditions
{
class Not : public Condition
{
    __DeclareClass(Not);
    __DeclareMsgId;
public:
    /// check whether the condition is true
    virtual bool Evaluate();
    /// assert that all required data is present in the world database
    virtual void Assert();
    /// like Assert() but adds errors to the info log object instead of closing the application
    virtual bool Assert(const Ptr<Script::InfoLog>& infoLog);

	/// called when parent states becomes active
	virtual void OnActivate();
	/// called when parent states becomes inactive
	virtual void OnDeactivate();
	/// notify FSMConditions about message
	virtual void Notify(const Ptr<Messaging::Message>& msg);

    /// set the condition 
    void SetCondition(const Ptr<Conditions::Condition>& condition);
    /// get the condition
    const Ptr<Conditions::Condition>& GetCondition() const;

	/// set optional entity to entity
	virtual void SetEntity(const Ptr<Game::Entity>& entity);

    /// write to action reader
    virtual void Write(const Ptr<Script::ActionReader>& actionReader);
    /// read from action reader
    virtual void Read(const Ptr<Script::ActionReader>& actionReader);

private:
	// the condition to invert
	Ptr<Conditions::Condition> condition;
};
__RegisterClass(Not);

//------------------------------------------------------------------------------
/**
*/
inline
void
Not::SetCondition(const Ptr<Conditions::Condition>& condition)
{
    n_assert(condition != 0);
	this->condition = condition;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Ptr<Conditions::Condition>&
Not::GetCondition() const
{
    return this->condition;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Not::SetEntity(const Ptr<Game::Entity>& entity)
{
    Condition::SetEntity(entity);
    if (entity != 0)
    {
        this->condition->SetEntity(entity);
    }
}


}; // namespace Conditions
//------------------------------------------------------------------------------