//------------------------------------------------------------------------------
//  RakNetPlayer.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "network/raknet/raknetplayer.h"
#include "network/multiplayerserver.h"

namespace RakNet
{
__ImplementClass(RakNet::RakNetPlayer, 'RIPL', Base::PlayerBase);

using namespace Util;
using namespace Multiplayer;

//------------------------------------------------------------------------------
/**
*/
RakNetPlayer::RakNetPlayer():
    started(false)
{
}

//------------------------------------------------------------------------------
/**
*/
RakNetPlayer::~RakNetPlayer()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetPlayer::OnJoinSession(Multiplayer::Session* sessionPtr)
{
    PlayerBase::OnJoinSession(sessionPtr);    

#if NEBULA3_ENABLE_PROFILING
    Util::String prioNames[Multiplayer::PacketPriority::NumPacketPriorities] = {"LowPriority","NormalPriority","HighPriority"};
    const RakNetGUID& guid = this->GetUnqiueId().GetRaknetGuid();
    const SystemAddress& systemAddress = MultiplayerServer::Instance()->GetRakPeerInterface()->GetSystemAddressFromGuid(guid); 

    Util::String address(systemAddress.ToString(false));
    Util::String separator("/");
    IndexT i;
    for (i = 0; i < Multiplayer::PacketPriority::NumPacketPriorities; ++i)
    {   
        this->debugCounterSet.Add(Util::StringAtom("waitingSendMessages" + prioNames[i]), StatisticValue("Number of messages waiting in the send buffer, at " + prioNames[i] + " level. \nHigh values: You are sending more data than the available bandwidth. \nLow values: You are sending less data than the available bandwidth."));        
        this->debugCounterSet.Add(Util::StringAtom("sentMessages" + prioNames[i]), StatisticValue("Number of messages that have been sent at " + prioNames[i] + " level.\nHigh values: You are sending more data than the available bandwidth.\nLow values: You are sending less data than the available bandwidth."));          
        this->debugCounterSet.Add(Util::StringAtom("sentUserBits" + prioNames[i]), StatisticValue("How many bits of message data you have sent, at " + prioNames[i] + " level."));          
        this->debugCounterSet.Add(Util::StringAtom("sentFullBits" + prioNames[i]), StatisticValue("How many bits of total data has been sent, including RakNet's headers, at " + prioNames[i] + " level."));
    }
    this->debugCounterSet.Add(StringAtom("onlyAckSent"), StatisticValue("Number of packets that have been sent that contain only acknowlegements.\nHigh values: If this number rises continually, then the sender is flooding the recipient. Could also mean a network spike.\nZero value: Normal performance."));        
    this->debugCounterSet.Add(StringAtom("acknowlegdementSent"), StatisticValue("Number of acknowlegements sent for reliable messages. Each reliable message recieved generates one acknowlegement, including resends."));        
    this->debugCounterSet.Add(StringAtom("acknowlegdementPending"), StatisticValue("Number of acknowlegements waiting to be sent for reliable messages. Each reliable message recieved generates one acknowlegement, including resends.\nHigh values: The  update thread has stopped running or is spiking so no data is being sent.\nZero value: Normal performance."));        
    this->debugCounterSet.Add(StringAtom("acknowlegdementBitsSent"), StatisticValue("Total number of bits spent on sending acknowlegement packets."));        
    this->debugCounterSet.Add(StringAtom("onlyAckSentAndResend"), StatisticValue("Number of packets that have been sent that contain only acknowlegements and resends.\nHigh values: If this number rises continually then the sender is flooding the recipient at the same time the sender is not acknowleging packets. This could be poor network performance or could be when traffic is only flowing one way.\nLow values: Normal performance."));        
    this->debugCounterSet.Add(StringAtom("packetLoss"), StatisticValue("Number of messages that were not acknowleged in time so were resent. This is equivalent to packetloss.\nHigh values: High packetloss, large ping variations, and/or ly RakNet is not waiting long enough before doing resends.\n2% or less values: Normal performance."));        
    this->debugCounterSet.Add(StringAtom("packetUserBitsLoss"), StatisticValue("How many bits of data, excluding RakNet's headers, were resent due to packetloss."));        
    this->debugCounterSet.Add(StringAtom("packetFullBitsLoss"), StatisticValue("How many bits of data, including RakNet's headers, were resent due to packetloss."));        
    this->debugCounterSet.Add(StringAtom("messagesOnResendQueue"), StatisticValue("How many messages are waiting before they can be sent. This is the most useful statistic to see if you are sending faster than the reciever can handle..\nHigh Values: Send rate is exceeding bandwidth. Lag may be experienced.\nZero: High performance, a lot of bandwidth still available."));        
    this->debugCounterSet.Add(StringAtom("unsplitMessages"), StatisticValue("Number of messages sent that were under the MTU size, including RakNet's and the UDP headers.\nHigh values: Normal performance.\nLow values: Many messages are split. This will result in lag as messages wait to be reassembled."));        
    this->debugCounterSet.Add(StringAtom("splitMessages"), StatisticValue("Number of messages sent that were over the MTU size, including RakNet's and the UDP headers.\nHigh values: Many messages are split. This will result in lag as messages wait to be reassembled.\nLow values: Normal performance."));        
    this->debugCounterSet.Add(StringAtom("numTotalSplits"), StatisticValue("How many splits have been performed."));        
    this->debugCounterSet.Add(StringAtom("numPacketsSent"), StatisticValue("How many packets have been sent, containing any type of data.\nHigh values relative to messages sent: UDP headers will be taking a greater percentage of bandwidth. Manual Aggregation can improve bandwith usage.\nLow values relative to messages sent: UDP headers will be taking a small percentage of bandwidth. Messages are aggreggated efficiently"));        
    this->debugCounterSet.Add(StringAtom("encryptionBitsSent"), StatisticValue("Number of bits added to data solely due to usage of secure connections.\nHigh values relative to message bits sent: Many small messages are being sent and encryption is using bandwith inefficiently.\nLow values relative to message bits sent: Packets that are sent tend to be near the MTU size, using bandwidth efficiently."));        
    this->debugCounterSet.Add(StringAtom("totalBitsSent"), StatisticValue("Total number of bits sent - accounting for all data."));        
    this->debugCounterSet.Add(StringAtom("sequencedMessagesOutOfOrder"), StatisticValue("Number of sequenced messages that arrived out of order. This is equal to the number of sequenced messages dropped."));        
    this->debugCounterSet.Add(StringAtom("sequencedMessagesInOrder"), StatisticValue("Number of sequenced messages that arrived in order."));        
    this->debugCounterSet.Add(StringAtom("orderedMessagesOutOfOrder"), StatisticValue("Number of ordered messages that arrived out of order. Everytime this happens all ordered messages on that stream are delayed. If you encounter a lot of spiking this may be the cause."));        
    this->debugCounterSet.Add(StringAtom("orderedMessagesInOrder"), StatisticValue("Number of ordered messages that arrived in order."));        
    this->debugCounterSet.Add(StringAtom("numberPacketsReceived"), StatisticValue("When using secure connections, the number of packets that passed the CRC check. Otherwise, the actual number of packets that arrived.\nHigh values: Normal performance."));        
    this->debugCounterSet.Add(StringAtom("numberBitsReceived"), StatisticValue("When using secure connections, the number of packets that didn't pass the CRC check.\nNon-zero values: This can occur to connection sequence packets arriving late. If the values rises consistently and rapidly, then someone is modifying packets during transmission."));        
    this->debugCounterSet.Add(StringAtom("numberPacketsBadCRCReceived"), StatisticValue("Number of bits that have been recieved while the connection is active, excluding bits with a bad CRC if secure connections are used."));        
    this->debugCounterSet.Add(StringAtom("numberBitsBadCRCReceived"), StatisticValue("Number of bits comprising messages with a failed CRC check."));        
    this->debugCounterSet.Add(StringAtom("numberAckReceived"), StatisticValue("How many acknowlgements we recieved for message waiting to be resent. Values near the number of reliably sent messages indicate low packetloss."));        
    this->debugCounterSet.Add(StringAtom("numberDupAckReceived"), StatisticValue("How many acknowlgements we recieved for messages that have already been acknowleged - or were never reliable to begin with.\nNon-zero values: The sender has high packetloss."));        
    this->debugCounterSet.Add(StringAtom("numberMessagesReceived"), StatisticValue("Number of data messages that have arrived. This is always equal to or greater than the number of packets recieved. High values indicate good message aggreggation by the sender."));        
    this->debugCounterSet.Add(StringAtom("invalidMessagesReceived"), StatisticValue("How many data messages, other than acknowlgements, we recieved with corrupt  information.\nNon-zero values: Packet tampering or an  send error."));        
    this->debugCounterSet.Add(StringAtom("duplicateMessagesReceived"), StatisticValue("How many data messages, other than acknowlgements, we recieved more than once.\nHigh values: Acknowlegements are not sent fast enough for the messages received, the sender isn't waiting long enough to resend, or there's a lot of packetloss.\nLow values: Normal performance."));        
    this->debugCounterSet.Add(StringAtom("messagesWaitingForReassembly"), StatisticValue("How many split messages are waiting to be completed.\nHigh values: One or more very large messages have been sent at the same time, possibly accompanied by packetloss. Extremely high values may degrade performance.\nLow values: Normal performance."));        
    this->debugCounterSet.Add(StringAtom("OutputQueueSize"), StatisticValue("How many messages are waiting to be passed out of the reliability layer.\nNon-zero values: The update thread is spending all its time recieving messages and is not updating the reliability layer in a timely fashion. This could be due to handling too many clients on a slow server."));        
    this->debugCounterSet.Add(StringAtom("bitsPerSecond"), StatisticValue("Current bandwidth used, in bits per second."));        
    this->debugCounterSet.Add(StringAtom("bandwidthExceeded"), StatisticValue("On the last update cycle, RakNet did not send all the data waiting to go out due to flow control, and under certain conditions will increase the allowed output throughput. This will normally be true for the first 30 seconds of a new connection as the available bandwidth is established. It will be false if you are sending data below current allowed capacity."));        
#endif
}

} // namespace RakNet
