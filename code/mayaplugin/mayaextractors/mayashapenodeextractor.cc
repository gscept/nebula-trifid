//------------------------------------------------------------------------------
//  mayashapenodeextractor.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayashapenodeextractor.h"
#include "mayacore/mayavariableregistry.h"
#include "mayaexporters/mayameshexporter.h"

namespace Maya
{
__ImplementClass(Maya::MayaShapeNodeExtractor, 'MSPX', Maya::MayaExtractorBase);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
MayaStatus::Code
MayaShapeNodeExtractor::Extract(const Ptr<MayaScene>& mayaScene)
{
    MayaVariableRegistry* varRegistry = MayaVariableRegistry::Instance();

    // build target directory and file names for mesh exporter
    String dstDir = "mesh_dst:" + varRegistry->Get<String>(MayaVariable::CategoryName);
    String dstFile = varRegistry->Get<String>(MayaVariable::ObjectName) + "_s";

    // setup a mesh exporter
    MayaMeshExporter meshExporter;
    meshExporter.SetShaderFilter(MayaShaderFilter::Standard);
    meshExporter.SetTargetDirectory(dstDir);
    meshExporter.SetTargetFile(dstFile);
    meshExporter.SetRootNode("|model");
    MayaStatus::Code exportResult = meshExporter.Export();
    if (MayaStatus::Success != exportResult)
    {
        return exportResult;
    }

    // FIXME: add one shape nodes to MayaScene for each material group of the mesh export
    return MayaStatus::Success;
}

} // namespace Maya