#pragma once
//------------------------------------------------------------------------------
/**
    @class Conditions::Or
    
    Returns the logical OR result of a list of conditions.

    (C) 2006 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "scriptfeature/conditions/condition.h"

//------------------------------------------------------------------------------
namespace Conditions
{
class Or : public Condition
{
    __DeclareClass(Or);
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

    /// write to action reader
    virtual void Write(const Ptr<Script::ActionReader>& actionReader);
    /// read from action reader
    virtual void Read(const Ptr<Script::ActionReader>& actionReader);

    /// set optional entity 
	virtual void SetEntity(const Ptr<Game::Entity>& entity);

private:
	// the conditions to evaluate
	Util::Array<Ptr<Conditions::Condition> > conditions;
};
__RegisterClass(Or);

//------------------------------------------------------------------------------
/**
*/
inline
const Util::Array<Ptr<Conditions::Condition> >&
Or::GetConditions() const
{
	// return the pointers
    return this->conditions;
}
//------------------------------------------------------------------------------
/**
*/
inline
void
Or::SetEntity(const Ptr<Game::Entity>& entity)
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