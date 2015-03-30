#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::AttachmentServer
    
    Renderthread side server for managing attachments. Is call once per frame.
    Manages the updates of the positions of the attached entities, the attaching
    and detaching of attachments

    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "graphics/graphicsentity.h"
#include "util/stringatom.h"
#include "io/uri.h"
#include "resources/resourceid.h"
#include "timing/timer.h"

// forward declarations
namespace Graphics
{
    class BindAttachment;
    class UnbindAttachment;
}
namespace Graphics
{
    class ModelEntity;
}
namespace Animation
{
    class AnimEventInfo;
}

//------------------------------------------------------------------------------
namespace Graphics
{
class AttachmentServer : public Core::RefCounted
{
    __DeclareClass(AttachmentServer);
    __DeclareSingleton(AttachmentServer);

public:
    /// how to rotate the attached entity
    enum AttachmentRotation
    {
        Local,
        World,
        Entity,

        NumAttachmentRotations,
        InvalidAttachmentRotation
    };

    /// types for clearing attachments
    enum ClearType
    {
        ClearAll,                               // clear all attachments in world
        ClearAllOnEntity,                       // clear all attachments on entity
        ClearAllOnJoint,                        // clear all attachments on joint
        ClearAllIdenticalOnEntity,              // clear all identical attachments on entity
        ClearAllIdenticalOnJoint,               // clear all identical attachments on joint

        ClearNone,                              // do not clear anything

        NumClearTypes,
        InvalidClearType
    };

	/// constructor
    AttachmentServer();
    /// destructor
    virtual ~AttachmentServer();
    
    /// open the server
    bool Open();
    /// close the server
    void Close();
    /// return true if open
    bool IsOpen() const;

    /// call this once per frame
    void OnFrame(Timing::Time time);

    /// remove invalid attachments
    void RemoveInvalidAttachments();

    /// attach a new entity
    void AttachEntity(const Math::matrix44& offset,
                      const ClearType& clearType,
                      const Util::StringAtom& joint,
                      const Ptr<GraphicsEntity>& entityToAttach,
                      const Ptr<ModelEntity>& baseEntity,
                      bool keepLocal,
                      AttachmentRotation rotation,
                      bool attachedFromOtherThread);

    /// attach a new entity, and detach it after given time
    void AttachEntityTemporary( const Math::matrix44& offset,
                                const ClearType& clearType,
                                const Util::StringAtom& joint,
                                const Ptr<GraphicsEntity>& entityToAttach,
                                const Ptr<ModelEntity>& baseEntity,
                                bool keepLocal,
                                Timing::Time duration,
                                AttachmentRotation rotation,
                                bool attachedFromOtherThread);

    /// switch a attached entity to another joint
    void SwitchEntity(const Util::StringAtom& oldJoint, const Util::StringAtom& newJoint, const Ptr<ModelEntity>&  baseEnitty, const Ptr<GraphicsEntity>& entityToAttach);

    /// detach an entity
    void DetachEntity(const ClearType& clearType,
                      const Util::StringAtom& joint,
                      const Ptr<GraphicsEntity>& entityToDetach,
                      const Ptr<ModelEntity>& baseEntity);
    /// remove attachment
    void DetachEntity(const Ptr<Graphics::GraphicsEntity>& entity);
    /// hide or show all attachments on entity
    void SetVisibilityOnAttachments(const Ptr<Graphics::ModelEntity>& baseEntity, bool visible);

private:
    /// calculate the desired rotation

    /// final attachments for rendering
    struct Attachment
    {
        Ptr<Graphics::ModelEntity> baseEntity;
        Ptr<Graphics::GraphicsEntity> attachedEntity;
        IndexT jointIndex;
        Math::matrix44 offset;
        Timing::Time startTime;
        Timing::Time duration;
        bool keepLocal;
        bool initPosSet;
        bool attachedFromOtherThread;
        AttachmentRotation rotation;
    };

    /// all attachments in the world
    Util::Array<Attachment> attachments;
    bool isOpen;

    Timing::Time time;
}; 

//------------------------------------------------------------------------------
/**
*/
inline bool
AttachmentServer::IsOpen() const
{
    return this->isOpen;
}

} // namespace Graphics
//------------------------------------------------------------------------------

    
    