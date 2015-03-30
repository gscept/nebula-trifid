#ifndef ATTR_PHYSICSATTR_H
#define ATTR_PHYSICSATTR_H
//------------------------------------------------------------------------------
/**
    @file physicsattr/physicsattributes.h

    This is the central attribute registry for thisfeature. For more information
    on attributes, see Attr::Attribute.

    (C) 2007 Radon Labs GmbH
*/
#include "attr/attrid.h"
#include "physicsattr/physicsattributes.h"

//------------------------------------------------------------------------------
namespace Attr
{   
    DeclareString(Physics, 'PHYS', ReadWrite);
    DeclareBool(Collide, 'COLL', ReadWrite);
    DeclareString(PhysicMaterial, 'PHMA', ReadWrite);
    DeclareInt(TargetEntityId, 'TAEN', ReadWrite);
    //DeclareString(NoGoArea, '', ReadWrite);
    //DeclareMatrix44(NoGoAreaTransform, '', ReadWrite);
    
    // PhysicsProprty
    DeclareFloat4(VelocityVector, 'VELV', ReadWrite);		// the current absolute velocity vector

    // ActorPhysicsProperty
    DeclareFloat(RelVelocity, 'RVEL', ReadWrite);			// the currently set relative velocity (0..1)
    DeclareFloat(MaxVelocity, 'MVEL', ReadWrite);			// the maximum velocity
    DeclareBool(Following, 'FOLL', ReadWrite);				// true if currently following somebody
    DeclareBool(Moving, 'MOVG', ReadWrite);					// true if currently moving

	// Trigger Property
	DeclareString(Shape, 'SHPS', ReadWrite);				// the shape of the trigger
	DeclareBool(TriggerActive, 'TRGA', ReadWrite);			// indicates trigger enabled/disabled
	DeclareBool(OneWayTrigger, 'OWTR', ReadWrite);			// indicates if trigger is a oneway trigger
	DeclareBool(TriggerOnce, 'TRGO', ReadWrite);			// triggered once?
	DeclareFloat(TriggerPhaseTime, 'TPHT', ReadWrite);		// trigger phase time
	DeclareBool(ReactivateTrigger, 'REAT', ReadWrite);		// reactivate trigger after time?
	DeclareFloat(ReactivateTriggerTime, 'RATT', ReadWrite);	// time for reactivation
	DeclareFloat(TriggerTimeStamp, 'TTST', ReadWrite);		// timestamp 

}; // namespace Attr
//------------------------------------------------------------------------------
#endif