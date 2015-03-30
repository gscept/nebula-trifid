#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::DefaultMultiplayerNotificationHandler

    This Notification handler accepts Notifications and commands and game object updates
    incoming from the network thread
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
#include "multiplayer/base/multiplayernotificationhandlerbase.h"
#include "notificationprotocol.h"
#include "multiplayerprotocol.h"

//------------------------------------------------------------------------------
namespace Multiplayer
{
class DefaultMultiplayerNotificationHandler : public Base::MultiplayerNotificationHandlerBase
{
    __DeclareClass(DefaultMultiplayerNotificationHandler);
public:
    /// constructor
    DefaultMultiplayerNotificationHandler();
    /// destructor
    virtual ~DefaultMultiplayerNotificationHandler();        

protected:   
    /// handle a single multiplayer Notification message
    virtual void HandleNotification(const Ptr<Messaging::Message>& msg);

    //////////////////////////////////////////////////////////////////////////     
    /// event notifications
    /// handle sign in state changed msg
    virtual void HandlePlayerSignInStateChanged(const Ptr<PlayerSignInChanged>& msg);
    /// handle player joined session msg
    virtual void HandlePlayerJoinedSession(const Ptr<PlayerJoinedSession>& msg);
    /// handle player left session msg
    virtual void HandlePlayerLeftSession(const Ptr<PlayerLeftSession>& msg);
    /// handle game starting message, host invoked a game start, load level etc
    virtual void HandleGameStarting(const Ptr<GameStarting>& msg);
    /// handle game started message: Game started on each player in session, we are ready to run gameplay
    virtual void HandleGameStarted(const Ptr<GameStarted>& msg);
	/// handle game ending message
	virtual void HandleGameEnding(const Ptr<GameEnding>& msg);
    /// handle game ended message
    virtual void HandleGameEnded(const Ptr<GameEnded>& msg);
    /// handle player ready changed message
    virtual void HandlePlayerReadyChanged(const Ptr<PlayerReadyChanged>& msg);
    /// handle data received message
    virtual void HandleDataReceived(const Ptr<DataReceived>& msg);
    /// handle disconnection from server message
    virtual void HandleDisconnectionFromServer();
    /// handle local player added
    virtual void HandleLocalPlayerAdded(const Ptr<LocalPlayerAdded>& msg);
    /// handle local player removed
    virtual void HandleLocalPlayerRemoved(const Ptr<LocalPlayerRemoved>& msg);
	/// handle chat message
	virtual void HandleChatMessage(const Ptr<ChatMessage>& msg);
    //////////////////////////////////////////////////////////////////////////
};
}; // namespace Multiplayer
//------------------------------------------------------------------------------