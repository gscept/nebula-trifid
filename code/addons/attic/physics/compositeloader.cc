//------------------------------------------------------------------------------
//  physics/compositeloader.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/compositeloader.h"
#include "physics/rigidbody.h"
#include "physics/physicsserver.h"
#include "physics/boxshape.h"
#include "physics/sphereshape.h"
#include "physics/capsuleshape.h"
#include "physics/hingejoint.h"
#include "physics/meshshape.h"
#include "physics/universaljoint.h"
#include "physics/sliderjoint.h"
#include "physics/balljoint.h"
#include "physics/hinge2joint.h"
#include "io/xmlreader.h"
#include "io/ioserver.h"
#include "math/quaternion.h"

namespace Physics
{

using namespace Math;
//------------------------------------------------------------------------------
/**
    Read the rigid body definitions from a joint definition and set on
    joint.
*/
void
CompositeLoader::ParseJointRigidBodies(Composite* composite, const Ptr<IO::XmlReader>& xmlReader, Joint* joint)
{
    n_assert(joint);
    RigidBody* body1 = 0;
    RigidBody* body2 = 0;
    if (xmlReader->HasAttr("body1"))
    {
        body1 = composite->GetBodyByName(xmlReader->GetString("body1"));
    }
    if (xmlReader->HasAttr("body2"))
    {
        body2 = composite->GetBodyByName(xmlReader->GetString("body2"));
    }
    joint->SetBodies(body1, body2);
}

//------------------------------------------------------------------------------
/**
    Constructs a complete Physics::Composite object from a composite
    xml file.

    @param  filename    composite resource name
    @return             pointer to Physics::Composite object or 0 on error

    - 25-Oct-05 floh    added support for static collide shapes
    - 07-Aug-06 floh    now returns array of composites (new physics.xml format)    
*/
Util::Array<Ptr<Composite> >
CompositeLoader::Load(const Util::String& filename)
{
    n_assert(!filename.IsEmpty());

    Util::Array<Ptr<Composite> > result;

    // open stream for reading
    Util::String path;
    path.Format("physics:%s.xml", filename.AsCharPtr());    

    Ptr<IO::XmlReader> xmlReader = IO::XmlReader::Create();
    xmlReader->SetStream(IO::IoServer::Instance()->CreateStream(path));    

    if (xmlReader->Open())
    {
        // make sure its a composite file
        if (!xmlReader->HasNode("/Physics"))
        {
            n_error("Not a valid physics composite file: '%s'!", path.AsCharPtr());
            return result;
        }
        xmlReader->SetToNode("/Physics");

        // for each composite in the file...
        if (xmlReader->SetToFirstChild("Composite")) do
        {
            // create composite object
            Composite* composite = 0;
            if (xmlReader->HasAttr("type") && (xmlReader->GetString("type") == "ragdoll"))
            {
                composite = (Composite*) Physics::PhysicsServer::Instance()->CreateRagdoll();
                n_assert(composite->IsA(Composite::RTTI));
            }
            else
            {
                composite = Physics::PhysicsServer::Instance()->CreateComposite();
            }
            n_assert(composite);

            // set composite name
            composite->SetName(xmlReader->GetString("name"));

            // get number of bodies and joints in composite
            int numBodies = xmlReader->GetInt("numBodies");
            int numJoints = xmlReader->GetInt("numJoints");
            int numShapes = xmlReader->GetInt("numMeshes");
            if (numBodies > 0)
            {
                composite->BeginBodies(numBodies);
            }
            if (numJoints > 0)
            {
                composite->BeginJoints(numJoints);
            }
            if (numShapes > 0)
            {
                composite->BeginShapes(numShapes);
            }

            // iterate rigid bodies
            if (xmlReader->SetToFirstChild("RigidBody")) do
            {
                // construct a rigid body and add to composite
                Ptr<RigidBody> body = Physics::PhysicsServer::Instance()->CreateRigidBody();
                n_assert(body != 0);
                body->SetName(xmlReader->GetString("name"));
                if (xmlReader->HasAttr("jointCollide"))
                {
                    body->SetConnectedCollision(xmlReader->GetBool("jointCollide"));
                }
                if (xmlReader->HasAttr("model"))
                {
                    body->SetLinkName(RigidBody::ModelNode, xmlReader->GetString("model"));
                }
                if (xmlReader->HasAttr("shadow"))
                {
                    body->SetLinkName(RigidBody::ShadowNode, xmlReader->GetString("shadow"));
                }
                if (xmlReader->HasAttr("joint"))
                {
                    body->SetLinkName(RigidBody::JointNode, xmlReader->GetString("joint"));
                }
                body->BeginShapes(xmlReader->GetInt("numShapes"));
               
                Math::vector initialPos;
                Math::float4 initialRot(0.0f, 0.0f, 0.0f, 1.0f);
                if (xmlReader->HasAttr("pos"))
                {
                    initialPos = xmlReader->GetFloat4("pos");
                }
                if (xmlReader->HasAttr("rot"))
                {
                    initialRot = xmlReader->GetFloat4("rot");
                }

                Math::matrix44 initialTransform = matrix44::rotationquaternion(quaternion(initialRot.x(), initialRot.y(), initialRot.z(), initialRot.w()));
                if (initialRot.length() == 0) initialTransform = matrix44::identity();
                initialTransform.translate(initialPos);
                body->SetInitialTransform(initialTransform);

                // parse box shapes
                if (xmlReader->SetToFirstChild("BoxShape")) do
                {
                    Math::vector pos = xmlReader->GetFloat4("pos");
                    Math::float4 rot = xmlReader->GetFloat4("rot");
                    Math::vector size = xmlReader->GetFloat4("size");
                    MaterialType matType = MaterialTable::StringToMaterialType(xmlReader->GetString("mat").AsCharPtr());

                    Math::matrix44 m = matrix44::rotationquaternion(quaternion(rot.x(), rot.y(), rot.z(), rot.w()));
                    m.translate(pos);

                    Ptr<BoxShape> shape = PhysicsServer::Instance()->CreateBoxShape(m, matType, size);
                    n_assert(shape != 0);
                    body->AddShape(shape);
                }
                while (xmlReader->SetToNextChild("BoxShape"));

                // parse sphere shapes
                if (xmlReader->SetToFirstChild("SphereShape")) do
                {
                    Math::vector pos  = xmlReader->GetFloat4("pos");
                    float radius = xmlReader->GetFloat("radius");
                    MaterialType matType = MaterialTable::StringToMaterialType(xmlReader->GetString("mat").AsCharPtr());

                    Math::matrix44 m = Math::matrix44::identity();
                    m.translate(pos);

                    Ptr<SphereShape> shape = Physics::PhysicsServer::Instance()->CreateSphereShape(m, matType, radius);
                    body->AddShape(shape);
                }
                while (xmlReader->SetToNextChild("SphereShape"));

                // parse capsule shapes
                if (xmlReader->SetToFirstChild("CapsuleShape")) do
                {
                    Math::vector pos  = xmlReader->GetFloat4("pos");
                    Math::float4 rot  = xmlReader->GetFloat4("rot");
                    float radius = xmlReader->GetFloat("radius");
                    float length = xmlReader->GetFloat("length");
                    MaterialType matType = MaterialTable::StringToMaterialType(xmlReader->GetString("mat").AsCharPtr());

                    Math::matrix44 m = matrix44::rotationquaternion(quaternion(rot.x(), rot.y(), rot.z(), rot.w()));
                    m.translate(pos);
                    
                    Ptr<CapsuleShape> shape = PhysicsServer::Instance()->CreateCapsuleShape(m, matType, radius, length);
                    n_assert(shape != 0);
                    body->AddShape(shape);
                }
                while (xmlReader->SetToNextChild("CapsuleShape"));

                // the body is ready, add it to the composite
                body->EndShapes();
                composite->AddBody(body);
            }
            while (xmlReader->SetToNextChild("RigidBody"));

            // iterate BallAndSocket joints
            if (xmlReader->SetToFirstChild("BallAndSocket")) do
            {
                // create a Ball-And-Socket joint
                Ptr<BallJoint> joint = Physics::PhysicsServer::Instance()->CreateBallJoint();

                this->ParseJointRigidBodies(composite, xmlReader, joint);
                if (xmlReader->HasAttr("joint"))
                {
                    joint->SetLinkName(xmlReader->GetString("joint"));
                }
                joint->SetAnchor(xmlReader->GetFloat4("anchor"));
                composite->AddJoint(joint);
            }
            while (xmlReader->SetToNextChild("BallAndSocket"));

            // iterate Hinge joints
            if (xmlReader->SetToFirstChild("Hinge")) do
            {
                // create a Hinge joint
                Ptr<HingeJoint> joint = Physics::PhysicsServer::Instance()->CreateHingeJoint();

                this->ParseJointRigidBodies(composite, xmlReader, joint);
                if (xmlReader->HasAttr("joint"))
                {
                    joint->SetLinkName(xmlReader->GetString("joint"));
                }
                joint->SetAnchor(xmlReader->GetFloat4("anchor"));

                JointAxis& axis = joint->AxisParams();
                axis.SetAxis(xmlReader->GetFloat4("axis"));
                if (xmlReader->HasAttr("lostop0"))
                {
                    axis.SetLoStopEnabled(true);
                    axis.SetLoStop(n_deg2rad(xmlReader->GetFloat("lostop0")));
                }
                if (xmlReader->HasAttr("histop0"))
                {
                    axis.SetHiStopEnabled(true);
                    axis.SetHiStop(n_deg2rad(xmlReader->GetFloat("histop0")));
                }
                axis.SetVelocity(n_deg2rad(xmlReader->GetFloat("vel0")));
                axis.SetFMax(xmlReader->GetFloat("fmax0"));
                axis.SetFudgeFactor(xmlReader->GetFloat("fudge0"));
                axis.SetBounce(xmlReader->GetFloat("bounce0"));
                axis.SetCFM(xmlReader->GetFloat("cfm0"));
                axis.SetStopERP(xmlReader->GetFloat("stopErp0"));
                axis.SetStopCFM(xmlReader->GetFloat("stopCfm0"));

                composite->AddJoint(joint);
            }
            while (xmlReader->SetToNextChild("Hinge"));

            // iterate Universal joints
            if (xmlReader->SetToFirstChild("UniversalJoint")) do
            {
                // create a Universal joint
                Ptr<UniversalJoint> joint = Physics::PhysicsServer::Instance()->CreateUniversalJoint();

                this->ParseJointRigidBodies(composite, xmlReader, joint);
                if (xmlReader->HasAttr("joint"))
                {
                    joint->SetLinkName(xmlReader->GetString("joint"));
                }
                joint->SetAnchor(xmlReader->GetFloat4("anchor"));

                JointAxis& axis0 = joint->AxisParams(0);
                axis0.SetAxis(xmlReader->GetFloat4("axis1"));
                if (xmlReader->HasAttr("lostop0"))
                {
                    axis0.SetLoStopEnabled(true);
                    axis0.SetLoStop(n_deg2rad(xmlReader->GetFloat("lostop0")));
                }
                if (xmlReader->HasAttr("histop0"))
                {
                    axis0.SetHiStopEnabled(true);
                    axis0.SetHiStop(n_deg2rad(xmlReader->GetFloat("histop0")));
                }
                axis0.SetVelocity(n_deg2rad(xmlReader->GetFloat("vel0")));
                axis0.SetFMax(xmlReader->GetFloat("fmax0"));
                axis0.SetFudgeFactor(xmlReader->GetFloat("fudge0"));
                axis0.SetBounce(xmlReader->GetFloat("bounce0"));
                axis0.SetCFM(xmlReader->GetFloat("cfm0"));
                axis0.SetStopERP(xmlReader->GetFloat("stopErp0"));
                axis0.SetStopCFM(xmlReader->GetFloat("stopCfm0"));

                JointAxis& axis1 = joint->AxisParams(1);
                axis1.SetAxis(xmlReader->GetFloat4("axis2"));
                if (xmlReader->HasAttr("lostop1"))
                {
                    axis1.SetLoStopEnabled(true);
                    axis1.SetLoStop(n_deg2rad(xmlReader->GetFloat("lostop1")));
                }
                if (xmlReader->HasAttr("histop1"))
                {
                    axis1.SetHiStopEnabled(true);
                    axis1.SetHiStop(n_deg2rad(xmlReader->GetFloat("histop1")));
                }
                axis1.SetVelocity(n_deg2rad(xmlReader->GetFloat("vel1")));
                axis1.SetFMax(xmlReader->GetFloat("fmax1"));
                axis1.SetFudgeFactor(xmlReader->GetFloat("fudge1"));
                axis1.SetBounce(xmlReader->GetFloat("bounce1"));
                axis1.SetCFM(xmlReader->GetFloat("cfm1"));
                axis1.SetStopERP(xmlReader->GetFloat("stopErp1"));
                axis1.SetStopCFM(xmlReader->GetFloat("stopCfm1"));

                composite->AddJoint(joint);
            }
            while (xmlReader->SetToNextChild("UniversalJoint"));

            // iterate slider joints
            if (xmlReader->SetToFirstChild("Slider")) do
            {
                // create a slider joint
                Ptr<SliderJoint> joint = Physics::PhysicsServer::Instance()->CreateSliderJoint();

                this->ParseJointRigidBodies(composite, xmlReader, joint);
                if (xmlReader->HasAttr("joint"))
                {
                    joint->SetLinkName(xmlReader->GetString("joint"));
                }

                JointAxis& axis = joint->AxisParams();
                axis.SetAxis(xmlReader->GetFloat4("axis"));
                if (xmlReader->HasAttr("lostop0"))
                {
                    axis.SetLoStopEnabled(true);
                    axis.SetLoStop(n_deg2rad(xmlReader->GetFloat("lostop0")));
                }
                if (xmlReader->HasAttr("histop0"))
                {
                    axis.SetHiStopEnabled(true);
                    axis.SetHiStop(n_deg2rad(xmlReader->GetFloat("histop0")));
                }
                axis.SetVelocity(n_deg2rad(xmlReader->GetFloat("vel0")));
                axis.SetFMax(xmlReader->GetFloat("fmax0"));
                axis.SetFudgeFactor(xmlReader->GetFloat("fudge0"));
                axis.SetBounce(xmlReader->GetFloat("bounce0"));
                axis.SetCFM(xmlReader->GetFloat("cfm0"));
                axis.SetStopERP(xmlReader->GetFloat("stopErp0"));
                axis.SetStopCFM(xmlReader->GetFloat("stopCfm0"));

                composite->AddJoint(joint);
            }
            while (xmlReader->SetToNextChild("Slider"));

            // iterate Hinge2 joints
            if (xmlReader->SetToFirstChild("Hinge2")) do
            {
                // create hinge2 joint
                Ptr<Hinge2Joint> joint = Physics::PhysicsServer::Instance()->CreateHinge2Joint();

                this->ParseJointRigidBodies(composite, xmlReader, joint);
                if (xmlReader->HasAttr("joint"))
                {
                    joint->SetLinkName(xmlReader->GetString("joint"));
                }
                joint->SetAnchor(xmlReader->GetFloat4("anchor"));

                JointAxis& axis0 = joint->AxisParams(0);            
                axis0.SetAxis(xmlReader->GetFloat4("axis1"));            
                if (xmlReader->HasAttr("lostop0"))
                {
                    axis0.SetLoStopEnabled(true);
                    axis0.SetLoStop(n_deg2rad(xmlReader->GetFloat("lostop0")));
                }
                if (xmlReader->HasAttr("histop0"))
                {
                    axis0.SetHiStopEnabled(true);
                    axis0.SetHiStop(n_deg2rad(xmlReader->GetFloat("histop0")));
                }
                axis0.SetVelocity(n_deg2rad(xmlReader->GetFloat("vel0")));
                axis0.SetFMax(xmlReader->GetFloat("fmax0"));
                axis0.SetFudgeFactor(xmlReader->GetFloat("fudge0"));
                axis0.SetBounce(xmlReader->GetFloat("bounce0"));
                axis0.SetCFM(xmlReader->GetFloat("cfm0"));
                axis0.SetStopERP(xmlReader->GetFloat("stopErp0"));
                axis0.SetStopCFM(xmlReader->GetFloat("stopCfm0"));

                JointAxis& axis1 = joint->AxisParams(1);
                axis1.SetAxis(xmlReader->GetFloat4("axis2"));
                if (xmlReader->HasAttr("lostop1"))
                {
                    axis1.SetLoStopEnabled(true);
                    axis1.SetLoStop(n_deg2rad(xmlReader->GetFloat("lostop1")));
                }
                if (xmlReader->HasAttr("histop1"))
                {
                    axis1.SetHiStopEnabled(true);
                    axis1.SetHiStop(n_deg2rad(xmlReader->GetFloat("histop1")));
                }
                axis1.SetVelocity(n_deg2rad(xmlReader->GetFloat("vel1")));
                axis1.SetFMax(xmlReader->GetFloat("fmax1"));
                axis1.SetFudgeFactor(xmlReader->GetFloat("fudge1"));
                axis1.SetBounce(xmlReader->GetFloat("bounce1"));
                axis1.SetCFM(xmlReader->GetFloat("cfm1"));
                axis1.SetStopERP(xmlReader->GetFloat("stopErp1"));
                axis1.SetStopCFM(xmlReader->GetFloat("stopCfm1"));

                joint->SetSuspensionERP(xmlReader->GetFloat("suspERP"));
                joint->SetSuspensionCFM(xmlReader->GetFloat("suspCFM"));

                composite->AddJoint(joint);
            }
            while (xmlReader->SetToNextChild("Hinge2"));

            // iterate shapes
            if (xmlReader->SetToFirstChild("MeshShape")) do
            {
                static const Math::matrix44 identity = Math::matrix44::identity();
                Util::String filename = xmlReader->GetString("file");
                MaterialType matType = MaterialTable::StringToMaterialType(xmlReader->GetString("mat").AsCharPtr());
                int meshGroupIndex = xmlReader->GetInt("meshGroup");
                Ptr<MeshShape> meshShape = Physics::PhysicsServer::Instance()->CreateMeshShape(identity, matType, filename, meshGroupIndex);
                composite->AddShape(meshShape.get());
            }
            while (xmlReader->SetToNextChild("MeshShape"));

            if (numBodies > 0)
            {
                composite->EndBodies();
            }
            if (numJoints > 0)
            {
                composite->EndJoints();
            }
            if (numShapes > 0)
            {
                composite->EndShapes();
            }

            // add new composite to result
            result.Append(composite);
        }
        while (xmlReader->SetToNextChild("Composite"));

        xmlReader->Close();
    }
    return result;
}

} // namespace Physics