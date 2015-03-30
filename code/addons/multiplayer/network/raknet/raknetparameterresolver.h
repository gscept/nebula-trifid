#pragma once
//------------------------------------------------------------------------------
/**
    @class RakNet::RaknetParameterResolver

    This class specifies some methods for RakNet session parametrization

    ATTENTION: be sure, that the Bitstream uses after encoding NOT more than 
               SEARCH_INFO_ADDITONAL_DATA_SIZE in bytes. This define is declare in the 
               RakNet::MultiplayerServer.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "network/base/parameterresolverbase.h"
#include "raknet/BitStream.h"

//------------------------------------------------------------------------------
namespace RakNet
{
class RakNetParameterResolver : public Base::ParameterResolverBase
{
    __DeclareClass(RakNetParameterResolver);

public:
    /// constructor
    RakNetParameterResolver();
    /// destructor
    virtual ~RakNetParameterResolver();

    /// encode data to bitstream
    virtual const BitStream* EncodeToBitstream() const;
    /// decode data from bitstream
    virtual void DecodeFromBitstream(BitStream* stream);

    /// compare to data given in a bitstream
    virtual bool CompareParameterSetWithBitstream(BitStream* stream);
};  
}; // namespace RakNet
//------------------------------------------------------------------------------