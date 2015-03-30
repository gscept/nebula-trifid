//------------------------------------------------------------------------------
//  playerbase.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "network/base/playerbase.h"
#include "network/session.h"

namespace Base
{
__ImplementClass(Base::PlayerBase, 'IPLB', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
PlayerBase::PlayerBase():    
    signInState(Unkown),
    isReady(false),
    isLocal(true),
    isHost(false),
    isInPublicSlot(true),
    session(0)
{
}

//------------------------------------------------------------------------------
/**
*/
PlayerBase::~PlayerBase()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
PlayerBase::OnJoinSession(Multiplayer::Session* sessionPtr)
{
    n_assert(sessionPtr != 0);
    n_assert(this->session == 0);
    this->session = sessionPtr;
}

//------------------------------------------------------------------------------
/**
*/
void 
PlayerBase::OnLeaveSession()
{
    n_assert(this->session != 0);
    this->session = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
PlayerBase::SetReady(bool val)
{
    this->isReady = val;
}

//------------------------------------------------------------------------------
/**
*/
bool 
PlayerBase::IsReady() const
{  
    return this->isReady;
}

//------------------------------------------------------------------------------
/**
*/
void 
PlayerBase::AppendPlayerInfoToStream(const Ptr<Multiplayer::BitWriter>& writer)
{
    // first id
    this->id.Encode(writer);
    // second name
    writer->WriteString(this->playerName.Value());
    // third ready state
    writer->WriteBool(this->IsReady());
    // is this player the host
    writer->WriteBool(this->IsHost());
}

//------------------------------------------------------------------------------
/**
*/
void 
PlayerBase::SetPlayerInfoFromStream(const Ptr<Multiplayer::BitReader>& reader)
{
    // id 
    this->id.Decode(reader);
    // player name
    this->playerName = reader->ReadString();    
    // ready state
    this->SetReady(reader->ReadBool());
    // is this player the host
    this->SetHost(reader->ReadBool());
}
} // namespace Multiplayer
