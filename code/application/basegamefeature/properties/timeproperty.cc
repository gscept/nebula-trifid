//------------------------------------------------------------------------------
//  properties/timeproperty.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/timeproperty.h"
#include "game/entity.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "basegametiming/gametimesource.h"

namespace BaseGameFeature
{
__ImplementClass(BaseGameFeature::TimeProperty, 'TIMP', Game::Property);

using namespace Timing;

//------------------------------------------------------------------------------
/**
*/
TimeProperty::TimeProperty() :
    attachTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TimeProperty::~TimeProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
TimeProperty::SetupCallbacks()
{
    this->entity->RegisterPropertyCallback(this, BeginFrame);
}

//------------------------------------------------------------------------------
/**
*/
void
TimeProperty::OnActivate()
{
    Property::OnActivate();
    this->attachTime = GameTimeSource::Instance()->GetTime();
}

//------------------------------------------------------------------------------
/**
*/
void
TimeProperty::OnBeginFrame()
{
    Time absTime = GameTimeSource::Instance()->GetTime();
    Time relTime = absTime - this->attachTime;
    this->GetEntity()->SetFloat(Attr::Time, float(relTime));
}

}; // namespace Properties
