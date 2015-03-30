#ifndef PHYSICS_COLLIDEMESHLOADER_H
#define PHYSICS_COLLIDEMESHLOADER_H
//------------------------------------------------------------------------------
/**
    @class Physics::CollideShapeLoader
  
    Opens a physics.xml file, and IF it contains shapes only, loads them
    and attaches them to the collide space. If not shapes, or any else
    then shapes are in the XML file, the Load() method will return
    an empty array!
    
    (C) 2006 Radon Labs GmbH
*/    
#include "physics/shape.h"

//------------------------------------------------------------------------------
namespace Physics
{
class CollideShapeLoader
{
public:
    /// read collide meshes, return empty array if not a pure collide xml file
    Util::Array<Ptr<Shape> > Load(const Util::String& name, const Math::matrix44& worldTransform);
};

} // namespace Physics
//------------------------------------------------------------------------------
#endif