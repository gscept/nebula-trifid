#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::MultiplayerTestNotificationHandler

    (C) 2009 Radon Labs GmbH
*/
#include "multiplayer/defaultmultiplayernotificationhandler.h"
#include "notificationprotocol.h"

//------------------------------------------------------------------------------
namespace Test
{
class MultiplayerTestNotificationHandler : public Multiplayer::DefaultMultiplayerNotificationHandler
{
    __DeclareClass(MultiplayerTestNotificationHandler);
public:
    /// constructor
    MultiplayerTestNotificationHandler();
    /// destructor
    virtual ~MultiplayerTestNotificationHandler();                             

    static uint playerLeftCounter;       
    static uint reconnections;

protected:   
    /// handle a single multiplayer Notification message
    virtual void HandleNotification(const Ptr<Messaging::Message>& msg);     
    /// handle sing in state changed msg
    virtual void HandlePlayerSignInStateChanged(const Ptr<Multiplayer::PlayerSignInChanged>& msg);
    /// handle player joined session msg
    virtual void HandlePlayerJoinedSession(const Ptr<Multiplayer::PlayerJoinedSession>& msg);
    /// handle player left session msg
    virtual void HandlePlayerLeftSession(const Ptr<Multiplayer::PlayerLeftSession>& msg);
    /// handle game starting message, host invoked a game start, load level etc
    virtual void HandleGameStarting(const Ptr<Multiplayer::GameStarting>& msg);
    /// handle game started message: Game started on each player in session, we are ready to run gameplay
    virtual void HandleGameStarted(const Ptr<Multiplayer::GameStarted>& msg);
    /// handle game ended message
    virtual void HandleGameEnded(const Ptr<Multiplayer::GameEnded>& msg);
    /// handle player ready changed message
    virtual void HandlePlayerReadyChanged(const Ptr<Multiplayer::PlayerReadyChanged>& msg);
    /// handle data received message
    virtual void HandleDataReceived(const Ptr<Multiplayer::DataReceived>& msg);
    /// handle disconnection from server message
    virtual void HandleDisconnectionFromServer();
};
}; // namespace Multiplayer
//------------------------------------------------------------------------------