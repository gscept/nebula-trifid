#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaMeshExporter
    
    The core mesh data exporting class.
    
    (C) 2009 Radon Labs GmbH
*/
#include "mayaexporters/mayaexporterbase.h"
#include "mayashaders/mayashaderfilter.h"
#include "math/transform44.h"

#include <maya/MDagPath.h>
#include <maya/MIntArray.h>

//------------------------------------------------------------------------------
namespace Maya
{
class MayaMeshExporter : public MayaExporterBase
{
public:
    /// constructor
    MayaMeshExporter();
    /// set shader filter (filters the nodes which the mesh exporter should consider)
    void SetShaderFilter(MayaShaderFilter::Code shdFilter);
    /// get shader filter
    MayaShaderFilter::Code GetShaderFilter() const;

    /// perform exporting
    virtual MayaStatus::Code Export();

private:
    /// perform pre-export steps
    void BeforeExport();
    /// perform post-export steps
    void AfterExport();
    /// perform shape-node export
    bool ExportShapeNode(const MDagPath& dagPath);
    /// export shape node geometry data
    void ExportGeometry(const MDagPath& dagPath, const MIntArray& faceMaterialGroupIndices, const Math::transform44& relHierNodeTransform);
    /// dump geometry export errors (invalid faces)
    void DumpExportGeometryErrors(const MDagPath& dagPath, const MIntArray& zeroAreaFaceIndices, const MIntArray& invalidTriangulationFaceIndices, const MIntArray& uvsOutOfRangeFaceIndices);
    /// build a string of face indices, called from DumpExportGeometryErrors()
    Util::String BuildFaceIndexString(const MIntArray& intArray);
    
    MayaShaderFilter::Code shaderFilter;
};

//------------------------------------------------------------------------------
/**
*/
inline void
MayaMeshExporter::SetShaderFilter(MayaShaderFilter::Code c)
{
    this->shaderFilter = c;
}

//------------------------------------------------------------------------------
/**
*/
inline MayaShaderFilter::Code
MayaMeshExporter::GetShaderFilter() const
{
    return this->shaderFilter;
}

} // namespace Maya
//------------------------------------------------------------------------------
