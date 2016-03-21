#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::EditorNavAreaProperty    
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "attr/attributedefinition.h"
#include "core/rtti.h"
#include "attr/attributecontainer.h"

//------------------------------------------------------------------------------
namespace LevelEditor2
{
class EditorNavAreaProperty : public Game::Property
{
	__DeclareClass(EditorNavAreaProperty);
	__SetupExternalAttributes();

public:    
	///
	EditorNavAreaProperty();
};
__RegisterClass(EditorNavAreaProperty);

}; // namespace LevelEditor2
//------------------------------------------------------------------------------

