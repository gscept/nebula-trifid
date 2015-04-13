#pragma once
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::AttachmentUtil
  
    Helper for quickly adding attachments
        
    (C) 2015 Individual contributors, see AUTHORS file
*/    
#include "graphics/graphicsprotocol.h"
#include "game/entity.h"
#include "graphics/graphicsentity.h"
#include "graphicsfeature/managers/attachmentmanager.h"


//------------------------------------------------------------------------------
namespace GraphicsFeature
{
class AttachmentUtil
{
public:
    /// enqueue animation on a game entity
    static void AddAttachment(const Ptr<Game::Entity> & entity,
							  const Util::StringAtom& joint,
							  const Util::StringAtom& resourceId,
							  const Math::matrix44& offset,
							  GraphicsFeature::AttachmentManager::AttachmentRotation rotation
                             );
	/// enqueue animation on a game entity
	static void AddAttachment(	const Ptr<Game::Entity> & entity,		
								const Util::StringAtom& resourceId,
								const Math::matrix44& offset								
							 );   
};

}