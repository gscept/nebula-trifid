#ifndef PHYSICS_COMPOSITELOADER_H
#define PHYSICS_COMPOSITELOADER_H
//------------------------------------------------------------------------------
/**
    @class Physics::CompositeLoader

    Creates a Physics::Composite from an XML stream.

    (C) 2005 RadonLabs GmbH
*/
#include "physics/composite.h"
#include "io/xmlreader.h"

//------------------------------------------------------------------------------
namespace Physics
{
class CompositeLoader
{
public:
    /// construct composite from xml file
    Util::Array<Ptr<Composite> > Load(const Util::String& name);

private:
    /// parse rigid body names of a joint and set on joint
    void ParseJointRigidBodies(Composite* composite, const Ptr<IO::XmlReader>& xmlReader, Joint* joint);
};

}; // namespace Physics

//------------------------------------------------------------------------------
#endif