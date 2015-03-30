#pragma once
//------------------------------------------------------------------------------
/**
    @class FmodCoreAudio::FmodListener

    This class represents a listener in 3D space, encapsulates the fmod-calls

    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/

#include "math/matrix44.h"
#include "math/vector.h"

//------------------------------------------------------------------------------
namespace FmodCoreAudio
{
class FmodListener
{
public:
    /// constructor
    FmodListener();
    /// destructor
    virtual ~FmodListener();

    /// set the 3d listener properties
    void Set(const Math::matrix44& transform, const Math::vector& velocity);
    /// on frame update method
    void OnFrame();

private:
    bool isDirty;
    Math::matrix44 transform;
    Math::vector velocity;
};

} // namespace FmodCoreAudio
//------------------------------------------------------------------------------
