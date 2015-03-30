#pragma once
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::AttachmentManager
    
    Main thread side manager (frontend) for managing attachments. 

    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file	
*/
#include "game/manager.h"
#include "core/singleton.h"
#include "util/string.h"
#include "math/matrix44.h"
#include "graphics/attachmentserver.h"
#include "graphics/pointlightentity.h"

// forward decl
namespace Graphics
{
    class GraphicsEntity;
};

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
	/// constructor
    AttachmentManager();
    /// destructor
    virtual ~AttachmentManager();

    /// trigger
    virtual void OnFrame();

    /// add an attachment by supplying a resource id which creates the entity
    void AddGraphicsAttachment( const Util::StringAtom& jointName,
                                const Ptr<Game::Entity>& baseEntityPtr,
                                const Resources::ResourceId& entityToAttachResId,
                                const Math::matrix44& offset,
                                bool keepLocal,
                                Graphics::AttachmentServer::AttachmentRotation rotation);

	/// add an attachment with an already created graphics entity
	void AddGraphicsAttachment( const Util::StringAtom& jointName,
								const Ptr<Game::Entity>& baseEntityPtr,
								const Ptr<Graphics::GraphicsEntity>& entityToAttach,
								const Math::matrix44& offset,
								bool keepLocal,
								Graphics::AttachmentServer::AttachmentRotation rotation);

	/// add an attachment which is automatically deleted after a certain of time, takes an already created entity to attach
	void AddGraphicsAttachmentTemporary( const Util::StringAtom& jointName,
								const Ptr<Game::Entity>& baseEntityPtr,
								const Ptr<Graphics::GraphicsEntity>& entityToAttach,
								const Math::matrix44& offset,
								bool keepLocal,
								Graphics::AttachmentServer::AttachmentRotation rotation,
								Timing::Time duration);

    /// attach a attachment temporary which is automatically deleted after a certain time, takes a resource id to which to attach
    void AddGraphicsAttachmentTemporary( const Util::StringAtom& jointName,
                                         const Ptr<Game::Entity>& baseEntityPtr,
                                         const Resources::ResourceId& entityToAttachResId,
                                         const Math::matrix44& offset,
                                         bool keepLocal,
                                         Graphics::AttachmentServer::AttachmentRotation rotation,
                                         Timing::Time duration);

    /// add a light attachment
    void AddLightAttachment( const Util::StringAtom& jointName,
                                const Ptr<Game::Entity>& baseEntityPtr,
                                const Ptr<Graphics::AbstractLightEntity>& light,
                                const Math::matrix44& offset,
                                bool keepLocal,
                                Graphics::AttachmentServer::AttachmentRotation rotation);

	/// attach a light temporary, will be removed after the given duration
	void AddLightAttachmentTemporary( const Util::StringAtom& jointName,
										const Ptr<Game::Entity>& baseEntityPtr,
										const Ptr<Graphics::AbstractLightEntity>& light,
										const Math::matrix44& offset,
										bool keepLocal,
										Graphics::AttachmentServer::AttachmentRotation rotation,
										Timing::Time duration);

    /// clear attachments
    void ClearAttachments();

    /// remove all attachments on given base entity
    void ClearAttachmentsOnEntity(const Ptr<Game::Entity>& baseEntity);
    
	/// Clear Joint
	void ClearAttachmentsOnJoint(const Util::StringAtom& joint,const Ptr<Game::Entity>& baseEntity);
    
private:
    /// attachment description
    struct Attachment
    {
        Ptr<Graphics::GraphicsEntity> baseEntity;
        Ptr<Graphics::GraphicsEntity> attachedEntity;
        Util::StringAtom joint;        
        Math::matrix44 offset;
        bool keepLocal;
        Graphics::AttachmentServer::AttachmentRotation rotation;
        Timing::Time duration;
        Timing::Time startTime;
        Util::StringAtom newJoint;
    };

    /// get a graphics entity by base entity
    Ptr<Graphics::GraphicsEntity> GetAttachmentGfxEntity(const Ptr<Graphics::GraphicsEntity>& gfxBaseEntity, const Util::StringAtom& jointName) const;

    /// check if we have to delete some attachments
    void CheckTemporaryAttachments();

    /// send final attachment to render thread
    void SendAttachmentMessage( const Attachment& attachment );

    /// send final detachment to render thread
    void SendDetachmentMessage( const Attachment& detachment );

    Util::Array<Attachment> attachments;                 // they are still attached
}; 

} // namespace GraphicsFeature
//------------------------------------------------------------------------------


    
    