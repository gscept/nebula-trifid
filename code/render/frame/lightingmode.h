#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::LightingMode
    
    The lighting mode to perform when rendering a frame batch.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "util/string.h"
    
//------------------------------------------------------------------------------
namespace Frame
{
class LightingMode
{
public:
    /// lighting mode enum
    enum Code
    {
        None,           // no lighting
        Forward,        // one-pass-lighting
        Deferred,       // pass-per-light
    };

    /// convert from string
    static Code FromString(const Util::String& str);
    /// convert to string
    static Util::String ToString(Code c);
};

} // namespace Frame
//------------------------------------------------------------------------------
 