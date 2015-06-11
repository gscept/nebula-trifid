//------------------------------------------------------------------------------
//  assetexporter.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "assetexporter.h"
#include "io/ioserver.h"
#include "io/assignregistry.h"

using namespace Util;
using namespace IO;
namespace ToolkitUtil
{
__ImplementClass(ToolkitUtil::AssetExporter, 'ASEX', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
AssetExporter::AssetExporter() :
    mode(All)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AssetExporter::~AssetExporter()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
AssetExporter::Open()
{
    ExporterBase::Open();
    this->fbxExporter = ToolkitUtil::NFbxExporter::Create();
    this->fbxExporter->Open();
    this->modelBuilder = ToolkitUtil::ModelBuilder::Create();
    this->textureExporter.SetTexAttrTablePath("src:assets/");
    this->textureExporter.Setup(this->logger);
}

//------------------------------------------------------------------------------
/**
*/
void
AssetExporter::Close()
{
    this->fbxExporter->Close();
    this->fbxExporter = 0;
    this->modelBuilder = 0;
    this->textureExporter.Discard();
    ExporterBase::Close();
}


//------------------------------------------------------------------------------
/**
*/
void
AssetExporter::ExportSystem()
{
	String origSrc = AssignRegistry::Instance()->GetAssign("src");
	AssignRegistry::Instance()->SetAssign(Assign("src", "toolkit"));
	this->ExportDir("system");
	this->ExportDir("lighting");
	AssignRegistry::Instance()->SetAssign(Assign("src", origSrc));
}

//------------------------------------------------------------------------------
/**
*/
void
AssetExporter::ExportDir(const Util::String& category)
{
    n_printf("Exporting asset directory '%s'\n", category.AsCharPtr());

    String assetPath = String::Sprintf("src:assets/%s/", category.AsCharPtr());
    IndexT fileIndex;

    if (this->mode & ExportModes::FBX)
    {
        // export FBX sources
        Array<String> files = IoServer::Instance()->ListFiles(assetPath, "*.fbx");
        this->fbxExporter->SetForce((this->mode & ExportModes::ForceFBX) != 0);
        this->fbxExporter->SetCategory(category);
        for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
        {
            this->fbxExporter->SetFile(files[fileIndex]);
            this->fbxExporter->ExportFile(assetPath + files[fileIndex]);
        }
    }    

    if (this->mode & ExportModes::Models)
    {
        // export models
        Array<String> files = IoServer::Instance()->ListFiles(assetPath, "*.constants");
        for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
        {
            String modelName = files[fileIndex];
            modelName.StripFileExtension();
            modelName = category + "/" + modelName;
            Ptr<ModelConstants> constants = ModelDatabase::Instance()->LookupConstants(modelName);
            Ptr<ModelAttributes> attributes = ModelDatabase::Instance()->LookupAttributes(modelName);
            Ptr<ModelPhysics> physics = ModelDatabase::Instance()->LookupPhysics(modelName);
            this->modelBuilder->SetConstants(constants);
            this->modelBuilder->SetAttributes(attributes);
            this->modelBuilder->SetPhysics(physics);

            // save models and physics
            String modelPath = String::Sprintf("mdl:%s.n3", modelName.AsCharPtr());
            this->modelBuilder->SaveN3(modelPath, this->platform);
            String physicsPath = String::Sprintf("phys:%s.np3", modelName.AsCharPtr());
            this->modelBuilder->SaveN3(physicsPath, this->platform);
        }
    }

    if (this->mode & ExportModes::Textures)
    {
        // export textures
        Array<String> files = IoServer::Instance()->ListFiles(assetPath, "*.tga");
        files.AppendArray(IoServer::Instance()->ListFiles(assetPath, "*.bmp"));
        files.AppendArray(IoServer::Instance()->ListFiles(assetPath, "*.dds"));
        files.AppendArray(IoServer::Instance()->ListFiles(assetPath, "*.psd"));
        files.AppendArray(IoServer::Instance()->ListFiles(assetPath, "*.png"));
        files.AppendArray(IoServer::Instance()->ListFiles(assetPath, "*.jpg"));
        this->textureExporter.SetForceFlag((this->mode & ExportModes::ForceTextures) != 0);
        for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
        {
            this->textureExporter.SetDstDir("tex:");
            this->textureExporter.ConvertTexture(assetPath + files[fileIndex], "temp:textureconverter");
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AssetExporter::ExportAll()
{
    IndexT folderIndex;
    Array<String> folders = IoServer::Instance()->ListDirectories("src:assets/", "*");
    for (folderIndex = 0; folderIndex < folders.Size(); folderIndex++)
    {
        this->ExportDir(folders[folderIndex]);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AssetExporter::ExportList(const Util::Array<Util::String>& files)
{
    for (Array<String>::Iterator iter = files.Begin(); iter != files.End(); iter++)
    {

    }
}

} // namespace ToolkitUtil