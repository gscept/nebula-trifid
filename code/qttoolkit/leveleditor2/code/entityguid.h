#pragma once
//------------------------------------------------------------------------------
/**
    LevelEditor2::EntityId
    
    Defines the id type of an entity, as well as the EntityType enumerator.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/

#include "util/guid.h"
#include "basegamefeature/basegameattr/basegameattributes.h"

namespace LevelEditor2
{
typedef Util::Guid EntityGuid;
static const Util::Guid InvalidEntityId;

enum EntityType
{
	InvalidType = 0,
	Environment = 1,
	Game = 2,
	Light = 3,
	Group = 4,
	NavMesh = 5,
	Probe = 6,
    NavMeshArea = 7,
	MultiSelection = 8,
	LevelReference = 9
};
}