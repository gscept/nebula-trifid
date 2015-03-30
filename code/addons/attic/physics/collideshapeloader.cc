//------------------------------------------------------------------------------
//  physics/collideshapeloader.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/collideshapeloader.h"
#include "physics/physicsserver.h"
#include "io/xmlreader.h"
#include "io/ioserver.h"

namespace Physics
{

//------------------------------------------------------------------------------
/**
    This loads the given XML file and checks if there are only
    mesh shapes in the file. If yes, the mesh shapes will
    be created an added to the collide shape, and the method
    returns true. If there are no meshes, or meshes plus
    other elements, nothing will be done and the method returns false.
*/
Util::Array<Ptr<Shape> >
CollideShapeLoader::Load(const Util::String& filename, const Math::matrix44& worldTransform)
{
    Util::Array<Ptr<Shape> > result;

    // open stream for reading
    Util::String path;
    path.Format("physics:%s.xml", filename.AsCharPtr());    

    Ptr<IO::XmlReader> xmlReader = IO::XmlReader::Create();
    xmlReader->SetStream(IO::IoServer::Instance()->CreateStream(path));

    if (xmlReader->Open())
    {
        // make sure it's a physics file
        if (!xmlReader->HasNode("/Physics"))
        {
            n_error("Not a valid physics file: '%s'!", path.AsCharPtr());
            return result;
        }
        xmlReader->SetToNode("/Physics/Composite");

        // get number of bodies and joints in composite
        int numBodies = xmlReader->GetInt("numBodies");
        int numJoints = xmlReader->GetInt("numJoints");
        int numShapes = xmlReader->GetInt("numMeshes");
        if ((numShapes > 0) && (numBodies == 0) && (numJoints == 0))
        {
            // ok, a pure collide file
            if (xmlReader->SetToFirstChild("MeshShape")) do
            {
                Util::String filename = xmlReader->GetString("file");
                MaterialType matType = MaterialTable::StringToMaterialType(xmlReader->GetString("mat").AsCharPtr());
                int meshGroupIndex = xmlReader->GetInt("meshGroup");
                Ptr<Shape> shape = (Shape*) Physics::PhysicsServer::Instance()->CreateMeshShape(worldTransform, matType, filename, meshGroupIndex);
                result.Append(shape);
            }
            while (xmlReader->SetToNextChild("MeshShape"));
        }
        xmlReader->Close();
    }
    return result;
}    

} // namespace Physics