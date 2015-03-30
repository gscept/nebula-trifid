//------------------------------------------------------------------------------
//  attr/attributes.h
//  (C) 2005 Radon Labs GmbH
//  (C) 2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "attributes.h"

namespace Attr
{
    DefineInt(ID,'CTID',Attr::ReadOnly);
    DefineInt(AnimationID,'CTAD',Attr::ReadWrite);
    DefineString(Character,'CTCA',Attr::ReadWrite);
    DefineString(Animation,'CTAN',Attr::ReadWrite);
    DefineString(HotSpot,'CTHS',Attr::ReadWrite);
    DefineInt(SampleRate,'CTSR',Attr::ReadWrite);
    DefineInt(LoopType,'CTLT',Attr::ReadWrite);
    DefineFloat(FadenInFrames,'CTFF',Attr::ReadWrite);
    DefineFloat(HotSpotPos,'CTHP',Attr::ReadWrite);
};
