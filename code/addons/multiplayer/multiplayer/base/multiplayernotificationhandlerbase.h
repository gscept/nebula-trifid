#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::MultiPlayerNotificationHandlerBase

    Base class for handling incoming network notifications

    Implement a subclass and register through multiplayer manager
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "threading/safequeue.h"
#include "messaging/message.h"

//------------------------------------------------------------------------------
namespace Base
{
class MultiplayerNotificationHandlerBase : public Core::RefCounted
{
    __DeclareClass(MultiplayerNotificationHandlerBase);
public:
    /// constructor
    MultiplayerNotificationHandlerBase();
    /// destructor
    virtual ~MultiplayerNotificationHandlerBase();

    /// append a Notification message to handler
    void PutNotification(const Ptr<Messaging::Message>& NotificationMsg);

    /// handle all pending Notifications
    void HandlePendingNotifications();

protected:   
    /// handle a single multiplayer Notification message, overwrite this in subclass
    virtual void HandleNotification(const Ptr<Messaging::Message>& NotificationMsg);

    Threading::SafeQueue<Ptr<Messaging::Message> > pendingNotificationMessages;
};
}; // namespace Base
//------------------------------------------------------------------------------