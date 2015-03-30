//------------------------------------------------------------------------------
//  fsmcondition.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/conditions/behaviour/fsmcondition.h"

namespace Conditions
{
__ImplementClass(Conditions::FSMCondition, 'FSCO', Conditions::Condition);

//------------------------------------------------------------------------------
/**
*/
void
FSMCondition::Notify(const Ptr<Messaging::Message>& msg)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
FSMCondition::OnActivate()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
FSMCondition::OnDeactivate()
{
    // empty
}

} // namespace Conditions