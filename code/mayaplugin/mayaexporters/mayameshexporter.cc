//------------------------------------------------------------------------------
//  mayaexporterbase.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayameshexporter.h"
#include "mayacore/mayamaterialgroupregistry.h"
#include "mayautil/mayanodeutil.h"
#include "mayautil/mayatransformutil.h"
#include "mayamain/mayalogger.h"

#include <maya/MItDag.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPointArray.h>

namespace Maya
{
using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
MayaMeshExporter::MayaMeshExporter() :
    shaderFilter(MayaShaderFilter::Standard)    
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Perform pre-export initializations.
*/
void
MayaMeshExporter::BeforeExport()
{
    // reset the material group registry
    MayaMaterialGroupRegistry::Instance()->Reset();
}

//------------------------------------------------------------------------------
/**
    Perform post-export stuff.
*/
void
MayaMeshExporter::AfterExport()
{
    // FIXME
}

//------------------------------------------------------------------------------
/**
*/
MayaStatus::Code
MayaMeshExporter::Export()
{
    MStatus status;
    SizeT numValidShapes = 0;

    // walk dag for shape nodes
    MItDag dagIter(MItDag::kDepthFirst, MFn::kMesh);
    if (this->IsRootNodeValid())
    {
        dagIter.reset(this->GetRootNodeDagPath(), MItDag::kDepthFirst, MFn::kMesh);
    }
    this->BeforeExport();
    for (; !dagIter.isDone(); dagIter.next())
    {
        MDagPath dagPath;
        status = dagIter.getPath(dagPath);
        n_assert(MS::kSuccess == status);
        n_assert(dagPath.apiType() == MFn::kMesh);
        
        // NOTE: check MayaNodeUtil::IsExportNode() source for special
        // cases to ignore visibility of node
        if (MayaNodeUtil::IsExportNode(dagPath))
        {
            if (this->ExportShapeNode(dagPath))
            {
                numValidShapes++;
            }
        }
    }
    this->AfterExport();

    // no valid shapes in scene?
    if (0 == numValidShapes)
    {
        return MayaStatus::NothingTodo;
    }
    else
    {
        return MayaStatus::Success;
    }
}

//------------------------------------------------------------------------------
/**
    Performs geometry export on a single shape node with one or more
    shaders attached.
*/
bool
MayaMeshExporter::ExportShapeNode(const MDagPath& shapeDagPath)
{
    MStatus status;

    // Get MFnMesh interface, this may fail if the Maya shape contains 
    // invalid data. This is a rare error situation, in which we
    // should silently ignore the mesh (and not crash, which would
    // be the other result).
    MFnMesh fnMesh(shapeDagPath, &status);
    if (MS::kSuccess != status)
    {
        return false;
    }

    // if the shape is instanced we need to determine which
    // instance this path refers to
    int instanceNum = 0;
    if (shapeDagPath.isInstanced())
    {
        instanceNum = shapeDagPath.instanceNumber();
    }

    // get shader array and face assignment for this shape
    MObjectArray shapeShaderArray;
    MIntArray shapeFaceShaderIndices;
    status = fnMesh.getConnectedShaders(instanceNum, shapeShaderArray, shapeFaceShaderIndices);
    if (MS::kSuccess != status)
    {
        // no shaders assigned to this mesh
        return false;
    }

    // need to find the hierarchy node this shape is a child of, this is necessary
    // for material-group setup (shapes which have the same shader but belong
    // to a different hierarchy node still need to be in different material groups)
    MDagPath hierNodeDagPath;
    if (!MayaNodeUtil::LookupParentHierarchyNode(shapeDagPath, hierNodeDagPath))
    {
        // there must be SOME hierarchy node somewhere (usually the
        // top-level "model" group node)
        return false;
    }

    // include the shape-shader-array into the global scene-shader-array
    // and convert the shader indices into scene-shader-indices
    MIntArray faceMaterialGroupIndices;
    if (MayaMaterialGroupRegistry::Instance()->RegisterShapeShaders(shapeShaderArray, 
        shapeFaceShaderIndices, 
        this->shaderFilter,
        hierNodeDagPath.node(), 
        faceMaterialGroupIndices))
    {
        // need to extract vertices in hierarchy node space
        MDagPath transformNodeDagPath = shapeDagPath;
        status = transformNodeDagPath.pop();
        n_assert(MS::kSuccess == status);
        transform44 relHierNodeTransform = MayaTransformUtil::GetHierarchyNodeRelativeTransform(transformNodeDagPath);
        
        // finally extract the geometry data from the shape
        this->ExportGeometry(shapeDagPath, faceMaterialGroupIndices, relHierNodeTransform);

        // all ok
        return true;
    }
    else
    {
        // no shader of the requested shader filter assigned, nothing to do
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Export the geometry for a single Maya shape node. This is the core
    method of the geometry export.
*/
void
MayaMeshExporter::ExportGeometry(const MDagPath& dagPath, const MIntArray& faceMaterialGroupIndices, const transform44& relHierNodeTransform)
{
    MStatus status;
    MFnMesh fnMesh(dagPath, &status);
    n_assert(MS::kSuccess == status);
    MayaLogger* logger = MayaLogger::Instance();
    
    MIntArray zeroAreaFaceIndices;
    MIntArray invalidTriangulationFaceIndices;
    MIntArray uvsOutOfRangeFaceIndices;

    // iterate over faces
    MItMeshPolygon polyIter(dagPath, MObject::kNullObj, &status);
    n_assert(MS::kSuccess == status);
    for (; !polyIter.isDone(); polyIter.next())
    {
        // get face index of current polygon
        int faceIndex = polyIter.index();

        // check whether the current polygon is valid
        if (-1 == faceMaterialGroupIndices[faceIndex])
        {
            logger->Print("Face %d has no valid shader assigned or doesn't match shader filter!\n", faceIndex);
            continue;
        }
        if (!polyIter.hasValidTriangulation())
        {
            invalidTriangulationFaceIndices.append(faceIndex);
            continue;
        }
        if (polyIter.zeroArea())
        {
            zeroAreaFaceIndices.append(faceIndex);
            continue;
        }

        // now iterate over face triangles
        int numFaceTriangles = 0;
        status = polyIter.numTriangles(numFaceTriangles);
        n_assert(MS::kSuccess == status);
        
        // logger->Print("Face %d has %d triangles.\n", faceIndex, numFaceTriangles);
        int faceTriangleIndex;
        for (faceTriangleIndex = 0; faceTriangleIndex < numFaceTriangles; faceTriangleIndex++)
        {
            MPointArray trianglePoints;
            MIntArray triangleVertexIndices;
            status = polyIter.getTriangle(faceTriangleIndex, trianglePoints, triangleVertexIndices, MSpace::kObject);
            n_assert(MS::kSuccess == status);
            n_assert(3 == trianglePoints.length());
            n_assert(3 == triangleVertexIndices.length());

            /*
            logger->Print("Face %d, tri %d: (%d,%d,%d) = (%f,%f,%f),(%f,%f,%f),(%f,%f,%f)\n",
                faceIndex, faceTriangleIndex, 
                triangleVertexIndices[0], triangleVertexIndices[1], triangleVertexIndices[2],
                trianglePoints[0].x, trianglePoints[0].y, trianglePoints[0].z,
                trianglePoints[1].x, trianglePoints[1].y, trianglePoints[1].z,
                trianglePoints[2].x, trianglePoints[2].y, trianglePoints[2].z);
            */
        }
    }

    // dump export errors
    this->DumpExportGeometryErrors(dagPath, 
                                   zeroAreaFaceIndices, 
                                   invalidTriangulationFaceIndices,
                                   uvsOutOfRangeFaceIndices);
}

//------------------------------------------------------------------------------
/**
    Called by ExportGeometry() after export do display any errors 
    (invalid faces).
*/
void
MayaMeshExporter::DumpExportGeometryErrors(const MDagPath& dagPath,
                                           const MIntArray& zeroAreaFaceIndices,
                                           const MIntArray& invalidTriangulationFaceIndices,
                                           const MIntArray& uvsOutOfRangeFaceIndices)
{
    MayaLogger* logger = MayaLogger::Instance();

    // display polygon errors
    if ((zeroAreaFaceIndices.length() > 0) ||
        (invalidTriangulationFaceIndices.length() > 0) ||
        (uvsOutOfRangeFaceIndices.length() > 0))
    {
        logger->Warning("Errors in shape node '%s':", dagPath.fullPathName().asChar());
        if (zeroAreaFaceIndices.length() > 0)
        {
            String indexString = this->BuildFaceIndexString(zeroAreaFaceIndices);
            logger->Warning("    %d faces have ZERO AREA (face indices: %s...)\n", zeroAreaFaceIndices.length(), indexString.AsCharPtr());
        }
        if (invalidTriangulationFaceIndices.length() > 0)
        {
            String indexString = this->BuildFaceIndexString(invalidTriangulationFaceIndices);
            logger->Warning("    %d faces have INVALID TRIANGULATION:\n", invalidTriangulationFaceIndices.length(), indexString.AsCharPtr());

        }
        if (uvsOutOfRangeFaceIndices.length() > 0)
        {
            String indexString = this->BuildFaceIndexString(uvsOutOfRangeFaceIndices);
            logger->Warning("    %d faces have OUT OF RANGE UVS:\n", uvsOutOfRangeFaceIndices.length(), indexString.AsCharPtr());
        }
    }
}

//------------------------------------------------------------------------------
/**
    Build a string with at most N entries from the MIntArray.
*/
String
MayaMeshExporter::BuildFaceIndexString(const MIntArray& intArray)
{
    String result;
    unsigned int i;
    for (i = 0; (i < 25) && (i < intArray.length()); i++)
    {
        result.Append(String::FromInt(intArray[i]));
        result.Append(", ");
    }
    return result;
}

} // namespace Maya