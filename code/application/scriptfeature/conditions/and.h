#pragma once
//------------------------------------------------------------------------------
/**
    @class Conditions::And
    
    Returns the logical AND result of a list of conditions.

    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "scriptfeature/conditions/condition.h"

//------------------------------------------------------------------------------
namespace Conditions
{
class And : public Condition
{
    __DeclareClass(And);
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

    /// add one condition
    void AddCondition(const Ptr<Conditions::Condition>& condition);
    /// get a list of conditions
    const Util::Array<Ptr<Conditions::Condition> >& GetConditions() const;

	/// set optional entity 
	virtual void SetEntity(const Ptr<Game::Entity>& entity);

    /// write to action reader
    virtual void Write(const Ptr<Script::ActionReader>& actionReader);
    /// read from action reader
    virtual void Read(const Ptr<Script::ActionReader>& actionReader);

private:
	// the conditions to evaluate
	Util::Array<Ptr<Conditions::Condition> > conditions;
};

__RegisterClass(And);

//------------------------------------------------------------------------------
/**
*/
inline
const Util::Array<Ptr<Conditions::Condition> >&
And::GetConditions() const
{
	// return the pointers
    return this->conditions;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
And::SetEntity(const Ptr<Game::Entity>& entity)
{
    Condition::SetEntity(entity);
    if (entity != 0)
    {
        int i;
        for (i = 0; i < this->conditions.Size(); i++)
        {
            this->conditions[i]->SetEntity(entity);
        }
    }
}

}; // namespace Conditions
//------------------------------------------------------------------------------