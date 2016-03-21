#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::EditorMultiselectProperty

    Property for modifying multiple entities
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "attr/attributedefinition.h"
#include "core/rtti.h"
#include "attr/attributecontainer.h"
#include "messaging/staticmessagehandler.h"
#include "editorproperty.h"


//------------------------------------------------------------------------------
namespace LevelEditor2
{
class EditorMultiselectProperty : public Game::Property
{
	__DeclareClass(EditorMultiselectProperty);
public:    
	///
	EditorMultiselectProperty();

	///
	virtual void SetupDefaultAttributes();
	///
	virtual void SetupAcceptedMessages();
	///
	virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
	


protected:
	friend class Messaging::StaticMessageHandler;   
	
	Attr::AttributeContainer attributes;
	Util::Array<Ptr<Game::Entity>> entities;

};
__RegisterClass(EditorMultiselectProperty);


}; // namespace LevelEditor2
//------------------------------------------------------------------------------

