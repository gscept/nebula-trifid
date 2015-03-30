#ifndef VEGETATION_GRASSPATCHCOORDS_H
#define VEGETATION_GRASSPATCHCOORDS_H
//------------------------------------------------------------------------------
/**
    @class Vegetation::GrassPatchCoords
    
    Contains an integer grass patch coordinate, and knows how to convert
    between world space and patch coordinates. The grass patch coordinate
    system is defined as follows:

    The center of the origin grass patch (0,0) is located at the
    world origin (GrassPatchSize*0.5, 0, GrassPatchSize*0.5). The world Y coordinate is irrelevant. The
    center of the grass patch at (2,0) is at the world coordinate
    (2.5 * GrassPatchSize, 0.0, 0.0) and so forth...
    
    (C) 2006 Radon Labs GmbH
*/
#include "core/types.h"
#include "math/vector.h"

//------------------------------------------------------------------------------
namespace Vegetation
{
class GrassPatchCoords
{
public:
    /// default constructor
    GrassPatchCoords();
    /// copy constructor
    GrassPatchCoords(const GrassPatchCoords& rhs);
    /// construct from x,y coords
    GrassPatchCoords(int x, int y);
    /// construct from world space coordinates
    GrassPatchCoords(const Math::vector& p);
    /// equality operator
    bool operator==(const GrassPatchCoords& rhs) const;
    /// inequality operator
    bool operator!=(const GrassPatchCoords& rhs) const;
    /// set x,y coordinates
    void Set(int x, int y);
    /// set from world space coordinates
    void SetFromWorldSpace(const Math::vector& p);
    /// get world space coordinates
    Math::vector GetAsWorldSpace() const;
    /// return center of grass patch in world coordinates
    static float GetGrassPatchSize();

    int x;
    int y;

private:
    friend class GrassRenderer;
    /// set the global grass patch size (called by grass renderer)
    static void SetGrassPatchSize(float s);

    static float grassPatchSize;
    static float oneDivGrassPatchSize;
    static float halfGrassPatchSize;
};

//------------------------------------------------------------------------------
/**
*/
inline
GrassPatchCoords::GrassPatchCoords() :
    x(0),
    y(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
GrassPatchCoords::GrassPatchCoords(const GrassPatchCoords& rhs) :
    x(rhs.x),
    y(rhs.y)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
GrassPatchCoords::GrassPatchCoords(int _x, int _y) :
    x(_x),
    y(_y)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
GrassPatchCoords::GrassPatchCoords(const Math::vector& p)
{
    this->SetFromWorldSpace(p);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
GrassPatchCoords::operator==(const GrassPatchCoords& rhs) const
{
    return (this->x == rhs.x) && (this->y == rhs.y);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
GrassPatchCoords::operator!=(const GrassPatchCoords& rhs) const
{
    return (this->x != rhs.x) || (this->y != rhs.y);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassPatchCoords::Set(int _x, int _y)
{
    this->x = _x;
    this->y = _y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassPatchCoords::SetFromWorldSpace(const Math::vector& v)
{
    this->x = Math::n_frnd(v.x() * oneDivGrassPatchSize);
    this->y = Math::n_frnd(v.z() * oneDivGrassPatchSize);
}

//------------------------------------------------------------------------------
/**
*/
inline
Math::vector
GrassPatchCoords::GetAsWorldSpace() const
{
    return Math::vector(this->x * grassPatchSize, 0.0f, this->y * grassPatchSize);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassPatchCoords::SetGrassPatchSize(float s)
{
    n_assert(s > 0.0f);
    grassPatchSize = s;
    oneDivGrassPatchSize = 1.0f / s;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
GrassPatchCoords::GetGrassPatchSize()
{
    return grassPatchSize;
}

} // namespace Vegetation
//------------------------------------------------------------------------------
#endif
    