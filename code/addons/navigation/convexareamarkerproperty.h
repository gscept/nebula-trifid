#pragma once
//------------------------------------------------------------------------------
/**
    @class Navigation::ConvexAreaMarkerProperty

    Volume that can be used to define areas with tags in leveleditor
      
    (C) 2015 Individual contributors, see AUTHORS file
*/
#include "game/property.h"

//------------------------------------------------------------------------------
namespace Navigation
{
class ConvexAreaMarkerProperty : public Game::Property
{
	__DeclareClass(ConvexAreaMarkerProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    ConvexAreaMarkerProperty();
    /// destructor
    virtual ~ConvexAreaMarkerProperty();    
};
__RegisterClass(ConvexAreaMarkerProperty);
}; // namespace Navigation
//------------------------------------------------------------------------------
