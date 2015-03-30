//------------------------------------------------------------------------------
//  network/multiplayer/base/multiplayernotificationhandlerbase.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "multiplayer/base/multiplayernotificationhandlerbase.h"

namespace Base
{
__ImplementClass(MultiplayerNotificationHandlerBase, 'MNHB', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
MultiplayerNotificationHandlerBase::MultiplayerNotificationHandlerBase()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
MultiplayerNotificationHandlerBase::~MultiplayerNotificationHandlerBase()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerNotificationHandlerBase::PutNotification(const Ptr<Messaging::Message>& NotificationMsg)
{
    this->pendingNotificationMessages.Enqueue(NotificationMsg);
}

//------------------------------------------------------------------------------
/**
    Handle all pending Notification messages in the pending queue. This method
    must be called frequently from the thread which created this
    Notification handler.
*/
void
MultiplayerNotificationHandlerBase::HandlePendingNotifications()
{
    Util::Array<Ptr<Messaging::Message> > notifications;
    this->pendingNotificationMessages.DequeueAll(notifications);
    IndexT i;
    for (i = 0; i < notifications.Size(); i++)
    {
        this->HandleNotification(notifications[i]);
        notifications[i]->SetHandled(true);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerNotificationHandlerBase::HandleNotification(const Ptr<Messaging::Message>& NotificationMsg)
{
    // error
    n_error("MultiPlayerNotificationHandlerBase::HandleNotification() -> not implemented");
}
} // namespace Multiplayer