//------------------------------------------------------------------------------
//  leveleditor2/propeerties/editornavareaproperty.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/editornavareaproperty.h"
#include "leveleditor2protocol.h"
#include "basegamefeature/basegameprotocol.h"
#include "leveleditor2app.h"

using namespace LevelEditor2;

namespace LevelEditor2
{

__ImplementClass(LevelEditor2::EditorNavAreaProperty, 'ENAP', Game::Property);

//------------------------------------------------------------------------------
/**
*/
EditorNavAreaProperty::EditorNavAreaProperty()
{
    // empty
}

}