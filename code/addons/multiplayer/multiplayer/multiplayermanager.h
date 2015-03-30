#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::MultiplayerManager

    This is the multiplayer frontend to the multiplayer thread
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file	
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "util/stringatom.h"
#include "util/fixedarray.h"
#include "messaging/message.h"
#include "network/multiplayer/session.h"
#include "network/multiplayer/player.h"
#include "multiplayerprotocol.h"
#include "notificationprotocol.h"

//------------------------------------------------------------------------------
namespace Multiplayer
{
class DefaultMultiplayerNotificationHandler;

class MultiplayerManager : public Core::RefCounted
{
    __DeclareClass(MultiplayerManager);
    __DeclareSingleton(MultiplayerManager);

public:    
    enum GameSearchState
    {
        GameSearchNoResults,
        GameSearchActive,
        GameSearchFinished
    };

    enum GameState
    {
        Idle,
        Starting,
        Running,
        Stopping
    };

    enum GameFunction
    {
        None,

        InitializingAsHost,
        HostingGame,

        InitializingAsClient,
        JoinedGame,
    };

    enum SessionState
    {
        NoSession,
        
        CreatingSession,
        SessionCreated,

        JoiningSession,
        SessionJoined,

        LeavingSession,
        SessionLeft,

        DeletingSession,
        SessionDeleted,

        Aborted,
    };
   
    /// constructor
    MultiplayerManager();
    /// destructor
    virtual ~MultiplayerManager();        

    /// open the ui subsystem
    bool Open();
    /// close the ui subsystem
    void Close();
    /// return true if currently open
    bool IsOpen() const;

	/// trigger the server
    void Trigger();
    /// debug output
    void RenderDebug();

    /// set maximum of local players (app specific)
    void SetMaxLocalPlayers(SizeT num);
    /// get maximum of local players
    SizeT GetMaxLocalPlayers() const;
    /// get number of local players
    SizeT GetNumLocalPlayers() const;

    /// register a notification handler
    void RegisterNotificationHandler(const Ptr<Base::MultiplayerNotificationHandlerBase>& handler);
    /// unregister a notification handler
    void UnregisterNotificationHandler(const Ptr<Base::MultiplayerNotificationHandlerBase>& handler);

    /// find the player with this handle
    Ptr<Player> GetPlayer(const InternalMultiplayer::PlayerHandle& handle) const;
    /// get the active session
    Ptr<Session> GetSession() const;

    ////////////////////// LOGIN STUFF ///////////////////////////////////////
    /// show dialog to login desired amount of players
    virtual void CreatePlayers(SizeT playerCount);
	/// create players using provided names
	virtual void CreatePlayers(SizeT playerCount, const Util::Array<Util::String> & names);
    /// is a creation request running
    bool IsCreatePlayersActive() const;

    ////////////////////// GAME SEARCH STUFF /////////////////////////////////
    /// start game search
	void StartGameSearch(Ptr<Base::ParameterResolverBase> parameterResolver, SizeT maxResults,Util::String IP, Timing::Time timeOut = 3.0);
    /// get state of game search
    GameSearchState GetGameSearchState() const;
    /// get the last game search results
    const Util::Array<InternalMultiplayer::SessionInfo>& GetLastGameSearchResults() const;
    /// clear last game search
    void ClearLastGameSearch();

    /////////////////////// HOSTING STUFF ////////////////////////////////////
    /// host a session
    void HostSession( Ptr<Base::ParameterResolverBase> parameterResolver,
                      ushort publicSlots, 
                      ushort privateSlots, 
                      InternalMultiplayer::MultiplayerType::Code type, 
                      IndexT playerIndex);
    /// delete a session
    void DeleteSession();

    /////////////////////// JOINING STUFF ////////////////////////////////////
    /// join a local couch coop player to current session, return the player index which was assigned
    void AddLocalPlayerToCurrentSession();
    /// leave local couch coop player from current session
    void RemoveLocalPlayerFromCurrentSession(IndexT playerIndex);

    /// join a session
    void JoinSession(InternalMultiplayer::SessionInfo session, IndexT playerIndex);
    /// leave a session 
    void LeaveSession(IndexT playerIndex);

    /////////////////////// GAME STUFF ///////////////////////////////////////
    /// set a player ready
    void SetPlayerReady(bool ready, IndexT playerIndex);
    /// is player ready ?
    bool GetPlayerReady(IndexT playerIndex) const;
    /// start a game
    void StartGame();
    /// notify server of game start finished
    void StartingFinished();
    /// end a game
    void EndGame();
	/// notify server of game end finished
	void EndingFinished();
	//////////////////////////// CHAT STUFF /////////////////////////////////
	/// Send chat message to player
	void SendChatMessage(const Ptr<Player>& player, const Util::String & message, int action = 0);	

    /////////////////////// GENERAL STATE STUFF /////////////////////////////
    /// get the state of game
    GameState GetGameState() const;
    /// get the part of us
    GameFunction GetGameFunction() const;
    /// get the session state
    SessionState GetSessionState() const;

    /// game function to string
    static Util::String GameFunctionAsString(GameFunction func);
    /// game state to string
    static Util::String GameStateAsString(GameState state);
    /// session state as string
    static Util::String SessionStateAsString(SessionState sessionState);

    /// send stream to player
    void SendStreamToPlayer(const Ptr<Player>& player, 
                            const Ptr<InternalMultiplayer::NetStream>& stream, 
                            bool reliable, 
                            bool sequential, 
                            bool ordered, 
                            InternalMultiplayer::PacketChannel::Code channel = InternalMultiplayer::PacketChannel::DefaultChannel);
    /// send stream to host
    void SendStreamToHost(const Ptr<InternalMultiplayer::NetStream>& stream, 
                          bool reliable, 
                          bool sequential, 
                          bool ordered, 
                          InternalMultiplayer::PacketChannel::Code channel = InternalMultiplayer::PacketChannel::DefaultChannel);

    /// handle a single multiplayer Notification message
    virtual void HandleNotification(const Ptr<Messaging::Message>& msg);

private:
    friend class Multiplayer::DefaultMultiplayerNotificationHandler;
    friend class Base::MultiplayerNotificationHandlerBase;

    /// set game state
    void SetGameState(GameState newState);
    /// set the game function
    void SetGameFunction(GameFunction newFunction);
    /// set the session state
    void SetSessionState(SessionState newState);

    /// check pending messages
    void CheckPendingMessages();
    
    /// setup new session
    void SetupNewSession(SizeT publicSlots,
                         SizeT privateSlots,
                         InternalMultiplayer::SessionHandle sessionHandle,
                         InternalMultiplayer::MultiplayerType::Code sessionType,
                         bool addLocalplayers,
                         bool addPublic);

    /// build array with all player handles
    Util::Array<InternalMultiplayer::PlayerHandle> GetLocalPlayerHandles() const;

    /// handle disconnection from server
    void OnDisconnectFromServer();
    /// on a full leave of a local player from session
    void OnLocalPlayerLeftSessionConfirmed(const InternalMultiplayer::PlayerHandle& playerHandle);
    /// delete players, log out and destroy
    void DeletePlayers();

    //////////////////////// return notifications ////////////////////////
    /// handle create players succeeded
    virtual void HandleCreatePlayersSucceeded(const Ptr<CreatePlayersSucceeded>& msg);
    /// handle create players failed
    virtual void HandleCreatePlayersFailed(const Ptr<CreatePlayersFailed>& msg);
    /// handle 
    virtual void HandleHostSessionSucceeded(const Ptr<HostSessionSucceeded>& msg);
    /// handle 
    virtual void HandleHostSessionFailed(const Ptr<HostSessionFailed>& msg);
    /// handle 
    virtual void HandleStartGameSearchFinished(const Ptr<StartGameSearchFinished>& msg);
    /// handle 
    virtual void HandleJoinSessionSucceeded(const Ptr<JoinSessionSucceeded>& msg);
    /// handle 
    virtual void HandleJoinSessionFailed(const Ptr<JoinSessionFailed>& msg);
    /// handle 
    virtual void HandleStartGameSucceeded(const Ptr<StartGameSucceeded>& msg);
    /// handle 
    virtual void HandleStartGameFailed(const Ptr<StartGameFailed>& msg);        
    /// handle 
    virtual void HandleEndGameSucceeded(const Ptr<EndGameSucceeded>& msg);
    /// handle 
    virtual void HandleEndGameFailed(const Ptr<EndGameFailed>& msg);
    /// handle 
    virtual void HandleLeaveSessionSucceeded(const Ptr<LeaveSessionSucceeded>& msg);
    /// handle 
    virtual void HandleLeaveSessionFailed(const Ptr<LeaveSessionFailed>& msg);
    /// handle 
    virtual void HandleDeleteSessionSucceded(const Ptr<DeleteSessionSucceded>& msg);  
    /// handle 
    virtual void HandleDeleteSessionFailed(const Ptr<DeleteSessionFailed>& msg);
    /// handle
    virtual void HandleLocalPlayerAdded(const Ptr<LocalPlayerAdded>& msg);
    /// handle
    virtual void HandleLocalPlayerRemoved(const Ptr<LocalPlayerRemoved>& msg);		
	/// handle
	virtual void HandleChatMessage(const Ptr<ChatMessage>&msg, bool fromNetwork);
    //////////////////////////////////////////////////////////////////////////

    Ptr<Session> activeSession;                                             // active online session
    Util::Array< Ptr<Player> > localPlayers;                                // array of local players
    SizeT maxLocalPlayers;

    bool inCreatePlayers;

    bool isOpen;

    SessionState sessionState;                                              // state of the current session
    GameFunction gameFunction;                                              // part of the game (host/client)
    GameState gameState;                                                    // game state
    GameSearchState gamesearchState;                                        // game search state
    Util::Array<InternalMultiplayer::SessionInfo> gamesearchResults;        // last game search results
};

//------------------------------------------------------------------------------
/**
*/
inline void 
MultiplayerManager::SetMaxLocalPlayers( SizeT num )
{
    n_assert(num <= NEBULA3_MAX_LOCAL_PLAYERS);
    this->maxLocalPlayers = num;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT 
MultiplayerManager::GetMaxLocalPlayers() const
{
    n_assert(this->maxLocalPlayers <= NEBULA3_MAX_LOCAL_PLAYERS);
    return this->maxLocalPlayers;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
MultiplayerManager::GetNumLocalPlayers() const
{
    return this->GetLocalPlayerHandles().Size();
}

//------------------------------------------------------------------------------
/**
*/
inline void
MultiplayerManager::SetSessionState(SessionState newState)
{
    switch (this->sessionState)
    {
    case NoSession:
        n_assert(newState == CreatingSession || newState == JoiningSession);
        break;
    case CreatingSession:
        n_assert(newState == SessionCreated || newState == Aborted);
        break;
    case SessionCreated:
        n_assert2(newState == DeletingSession, "Host can not leave session, only delete it!");
        break;
    case JoiningSession:
        n_assert(newState == SessionJoined || newState == Aborted);
        break;
    case SessionJoined:
        n_assert2(newState == LeavingSession, "Client can only leave a session, not delete it!");
        break;
    case LeavingSession:
        n_assert(newState == SessionLeft);
        break;
    case SessionLeft:
        n_assert(newState == NoSession || newState == JoiningSession || newState == CreatingSession);
        break;
    case DeletingSession:
        n_assert(newState == SessionDeleted);
        break;
    case SessionDeleted:
        n_assert(newState == NoSession || newState == JoiningSession || newState == CreatingSession);
        break;
    case Aborted:
        n_assert(newState == NoSession || newState == JoiningSession || newState == CreatingSession);
        break;
    default:
        n_error("Unknown session State!");
        break;
    }

    this->sessionState = newState;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MultiplayerManager::SetGameState(GameState newState)
{
    switch (this->gameState)
    {
    case Idle:
        n_assert(newState == Idle || newState == Starting);
        break;
    case Starting:
        n_assert(newState == Idle || newState == Running);
        break;
    case Running:
        n_assert(newState == Stopping);
        break;
    case Stopping:
        n_assert(newState == Idle);
        break;
    default:
        n_error("Unknown game State!");
        break;
    }

    this->gameState = newState;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MultiplayerManager::SetGameFunction(GameFunction newFunction)
{
    // first assert
    switch (this->gameFunction)
    {
    case None:
        n_assert(newFunction != None);
        break;
    case InitializingAsClient:
        n_assert(newFunction == None || newFunction == JoinedGame);
        break;
    case HostingGame:
        n_assert(newFunction == None);
        break;
    case InitializingAsHost:
        n_assert(newFunction == None || newFunction == HostingGame);
        break;
    case JoinedGame:
        n_assert(newFunction == None);
        break;
    default:
        n_error("Unknown game function!");
        break;
    }

    this->gameFunction = newFunction;
}
//------------------------------------------------------------------------------
/**
*/
inline bool
MultiplayerManager::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline MultiplayerManager::GameSearchState
MultiplayerManager::GetGameSearchState() const
{
    return this->gamesearchState;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<InternalMultiplayer::SessionInfo>& 
MultiplayerManager::GetLastGameSearchResults() const
{
    return this->gamesearchResults;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MultiplayerManager::ClearLastGameSearch()
{
    this->gamesearchState = GameSearchNoResults;
    this->gamesearchResults.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline MultiplayerManager::GameState
MultiplayerManager::GetGameState() const
{
    return this->gameState;
}

//------------------------------------------------------------------------------
/**
*/
inline MultiplayerManager::GameFunction
MultiplayerManager::GetGameFunction() const
{
    return this->gameFunction;
}

//------------------------------------------------------------------------------
/**
*/
inline MultiplayerManager::SessionState 
MultiplayerManager::GetSessionState() const
{
    return this->sessionState;
}

//------------------------------------------------------------------------------
/**
*/
inline Ptr<Session>
MultiplayerManager::GetSession() const
{
    return this->activeSession;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
MultiplayerManager::IsCreatePlayersActive() const
{
    return inCreatePlayers;
}
}; // namespace Multiplayer
//------------------------------------------------------------------------------