#pragma once
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::AttachmentManager
    
    Manager for attachment on game entities    

    (C) 2015 Individual contributors, see AUTHORS file	
*/
#include "game/manager.h"
#include "core/singleton.h"
#include "util/string.h"
#include "math/matrix44.h"
#include "util/stringatom.h"
#include "graphics/graphicsentity.h"
#include "math/matrix44.h"

namespace Graphics
{
	class GraphicsEntity;
	class ModelEntity;
}

namespace Game
{
    class Entity;
};

//------------------------------------------------------------------------------
namespace GraphicsFeature
{
class AttachmentManager : public Game::Manager
{
    __DeclareClass(AttachmentManager);
    __DeclareSingleton(AttachmentManager);

public:

	/// how to rotate the attached entity
	enum AttachmentRotation
	{
		/// apply offset, joint translation, and rotation
		Joint,
		/// apply offset and joint translation
		JointTranslationOnly,		
		/// apply offset to base transform
		TransformOnly,
	};

	/// constructor
    AttachmentManager();
    /// destructor
    virtual ~AttachmentManager();

    /// trigger
    virtual void OnBeginFrame();

    /// attach graphics entity onto other graphics entity, uses baseEntity transform as reference, ownership is transferred to attachmentmanager
	void Attach(const Ptr<Graphics::GraphicsEntity>& baseEntity,  
				const Ptr<Graphics::GraphicsEntity>& entityToAttach, 
				const Math::matrix44& offset,				
				AttachmentRotation rotation);

	/// attach graphics entity onto other modelentity, uses joint as reference, ownership is transferred to attachmentmanager
	void Attach(const Ptr<Graphics::ModelEntity>& baseEntity,
				const Util::StringAtom& joint,
				const Ptr<Graphics::GraphicsEntity>& entityToAttach,
				const Math::matrix44& offset,				
				AttachmentRotation rotation);

	/// attach game entity onto graphics entity, uses baseEntity transform as reference, will send message to entityToAttach upon detach
	void Attach(const Ptr<Graphics::GraphicsEntity>& baseEntity,
				const Ptr<Game::Entity>& entityToAttach,
				const Math::matrix44& offset,				
				AttachmentRotation rotation);

	/// attach game entity onto model entity, uses joint as reference, will send message to entityToAttach upon detach
	void Attach(const Ptr<Graphics::ModelEntity>& baseEntity,
				const Util::StringAtom& joint,
				const Ptr<Game::Entity>& entityToAttach,
				const Math::matrix44& offset,				
				AttachmentRotation rotation);
		
	/// detaches a graphics entity from the base model, will not remove it from the system
	void Detach(const Ptr<Graphics::GraphicsEntity>& entityToDetach);
	/// detaches a game entity from the base model, will not remove it from the system
	void Detach(const Ptr<Game::Entity>& entityToDetach);
    
    /// remove all attachments on given base entity, non-game entities will be removed from the stage as well
	void ClearAttachmentsOnEntity(const Ptr<Graphics::GraphicsEntity>& baseEntity);
    
	/// remove all attachments on a joint
	void ClearAttachmentsOnJoint(const Util::StringAtom& joint, const Ptr<Graphics::GraphicsEntity>& baseEntity);
    
private:	
	enum AttachmentType
	{
		GraphicsToGraphics,
		GraphicsToJoint,
		GameToGraphics,
		GameToJoint
	};

    /// attachment description
    struct Attachment
    {
		AttachmentType type;		
        Ptr<Graphics::GraphicsEntity> baseEntity;
		Ptr<Core::RefCounted> attachedEntity;		
		IndexT jointIndex;
		Util::StringAtom joint;
        Math::matrix44 offset;        
        AttachmentRotation rotation;        
    };

	/// calculate updated transform depending on attachment type
	Math::matrix44 CalculateTransform(const Attachment& attach);
	/// try to attach pending attachments
	void AttachPending();
	/// clear expired attachments
	void ClearInvalid();

	Util::Array<Attachment> attachments;
	Util::Array<Attachment> delayedAttachments;
}; 

} // namespace GraphicsFeature
//------------------------------------------------------------------------------


    
    