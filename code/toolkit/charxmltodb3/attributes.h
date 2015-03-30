#pragma once

//------------------------------------------------------------------------------
/**
    Attributes for the Char database
    
    (C) 2009 Radon Labs GmbH
*/
#include "attr/attrid.h"

//------------------------------------------------------------------------------
namespace Attr
{
    DeclareInt(ID,'CTID',Attr::ReadOnly);
    DeclareInt(AnimationID,'CTAD',Attr::ReadWrite);
    DeclareString(Character,'CTCA',Attr::ReadWrite);
    DeclareString(Animation,'CTAN',Attr::ReadWrite);
    DeclareString(HotSpot,'CTHS',Attr::ReadWrite);
    DeclareInt(SampleRate,'CTSR',Attr::ReadWrite);
    DeclareInt(LoopType,'CTLT',Attr::ReadWrite);
    DeclareFloat(FadenInFrames,'CTFF',Attr::ReadWrite);
    DeclareFloat(HotSpotPos,'CTHP',Attr::ReadWrite);
};
//------------------------------------------------------------------------------
