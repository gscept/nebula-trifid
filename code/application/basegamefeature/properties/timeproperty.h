#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::TimePropoerty

    The time property adds the attribute "Time" to the entity. This 
    attributes contains the time since the time property has been
    attached to the entity.

    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "timing/time.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class TimeProperty : public Game::Property
{
	__DeclareClass(TimeProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    TimeProperty();
    /// destructor
    virtual ~TimeProperty();
    /// setup callbacks for this property
    virtual void SetupCallbacks();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called on begin of frame
    virtual void OnBeginFrame();

private:
    Timing::Time attachTime;
};
__RegisterClass(TimeProperty);
}; // namespace Property
//------------------------------------------------------------------------------
