//------------------------------------------------------------------------------
//  fbxreader.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include <QtGui/QDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QFileInfo>
#include <QDateTime>
#include "nfbxreader.h"
#include "progressreporter.h"

using namespace Util;
namespace ModelImporter
{

//------------------------------------------------------------------------------
/**
*/
NFbxReader::NFbxReader() : 
	hasVertexColors(false),
	numUvs(0),
	scene(0),
	isOpen(false)
{
	this->sdkManager = FbxManager::Create();
}

//------------------------------------------------------------------------------
/**
*/
NFbxReader::~NFbxReader()
{
	// empty
}


//------------------------------------------------------------------------------
/**
	Global function called from KFbxImporter when loading progress is returned
*/
bool
ProgressChanged(void* arguments, float progress, const char* status)
{
	ContentBrowser::ProgressReporter::ReportProgress(progress, status);
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
NFbxReader::Open(const IO::URI& path)
{
	Q_ASSERT(!this->isOpen);

	this->path = path;
	String file = this->path.GetHostAndLocalPath().ExtractFileName();
	String category = this->path.GetHostAndLocalPath().ExtractLastDirName();
	this->scene = FbxScene::Create(this->sdkManager, "Import scene");

	FbxImporter* importer = FbxImporter::Create(this->sdkManager, "Importer");
	FbxProgressCallback callback = &ProgressChanged;
	importer->SetProgressCallback(callback);
	bool importStatus = importer->Initialize(path.GetHostAndLocalPath().AsCharPtr());
	if (importStatus)
	{
		importStatus = importer->Import(this->scene);
		if (!importStatus)
		{
			String error;
			error.Format("The file: %s/%s could not be imported!", category.AsCharPtr(), file.AsCharPtr());
			int retVal = QMessageBox::critical(this->parentWidget, "Import error", error.AsCharPtr());
			emit Loaded(Failed, path);
		}
		else
		{
			emit Loaded(Success, path);
		}
	}

	importer->ContentUnload();
	importer->Destroy();
}


//------------------------------------------------------------------------------
/**
*/
void 
NFbxReader::Close()
{
	Q_ASSERT(this->isOpen);
	this->scene->ContentUnload();	
	this->scene->Destroy(true);
	this->scene = 0;
	this->numUvs = 0;
}


//------------------------------------------------------------------------------
/**
*/
bool 
NFbxReader::IsOpen() const
{
	return this->isOpen;
}


//------------------------------------------------------------------------------
/**
*/
void
NFbxReader::SetParent(QWidget* parent)
{
	this->parentWidget = parent;
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Util::String> 
NFbxReader::GetTakes()
{
	Array<String> takes;
	int animStackCount = scene->GetSrcObjectCount<FbxAnimStack>();
	for (int stackIndex = 0; stackIndex < animStackCount; stackIndex++)
	{
		FbxAnimStack* animStack = scene->GetSrcObject<FbxAnimStack>(stackIndex);
		int animLayerCount = animStack->GetSrcObjectCount<FbxAnimLayer>();
		FbxAnimLayer* animLayer = animStack->GetSrcObject<FbxAnimLayer>(0);
		int animCurveNodeCount = animLayer->GetSrcObjectCount<FbxAnimCurveNode>();
		if (animCurveNodeCount > 0)
		{
			String stackname = animStack->GetName();
			takes.Append(stackname);
		}			
	}
	return takes;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String 
NFbxReader::GetInfo()
{
	FbxDocumentInfo* info = this->scene->GetSceneInfo();
	String infoText;

	QFileInfo fileInfo(this->path.AsString().AsCharPtr());

	FbxSystemUnit systemUnit = this->scene->GetGlobalSettings().GetSystemUnit();
	float scale = (float)systemUnit.GetScaleFactor();
	FbxString scaleString = systemUnit.GetScaleFactorAsString();

	int major, minor, revision;
	this->sdkManager->GetFileFormatVersion(major, minor, revision);
	infoText.Append("<br>");
	infoText.Append("<b>Autodesk FBX</b><br>");
	infoText.Append("Version: " + String::FromInt(major) + "." + String::FromInt(minor) + " revision: " + String::FromInt(revision) + "<br>");
	infoText.Append("Created: " + String(fileInfo.created().toString().toLatin1().constData()) + "<br>");
	infoText.Append("Last modified: " + String(fileInfo.lastModified().toString().toLatin1().constData()) + "<br>");
	infoText.Append("<br>");
	infoText.Append("<b>File: " + this->path.GetHostAndLocalPath() + "</b><br>");
	infoText.Append("<hr>");		
	infoText.Append("<br>");
	infoText.Append("Native scale: " + String::FromFloat(scale) + " (" + String(scaleString) + ") <br>");
	infoText.Append("Meshes: <br>");
	infoText.Append("<br>");

	// split meshes based on material
	FbxGeometryConverter* converter = new FbxGeometryConverter(this->scene->GetFbxManager());
	converter->SplitMeshesPerMaterial(this->scene, true);

	int meshCount = scene->GetSrcObjectCount<FbxMesh>();
	for (int meshIndex = 0; meshIndex < meshCount; meshIndex++)
	{
		FbxMesh* fbxMesh = scene->GetSrcObject<FbxMesh>(meshIndex);
		
		String name = fbxMesh->GetNode()->GetName();
		int vertexCount = fbxMesh->GetControlPointsCount();
		int polyCount = fbxMesh->GetPolygonCount();
		int uvs = fbxMesh->GetUVLayerCount();
		int colors = fbxMesh->GetElementVertexColorCount();
		String material = fbxMesh->GetNode()->GetMaterial(0)->GetName();
		String isCCW = fbxMesh->CheckIfVertexNormalsCCW() ? "Yes" : "No";
		String isSkinned = fbxMesh->GetDeformerCount(FbxDeformer::eSkin) > 0 ? "Yes" : "No";

		infoText.Append("<b>Name: " + name + "</b><br>");
		infoText.Append("Vertex count: " + String::FromInt(vertexCount) + "<br>");
		infoText.Append("Polygon count: " + String::FromInt(polyCount) + "<br>");
		infoText.Append("UV channel count: " + String::FromInt(uvs) + "<br>");
		infoText.Append("Vertex color count: " + String::FromInt(colors) + "<br>");
		infoText.Append("Is CCW: " + isCCW + "<br>");
		infoText.Append("Is skinned: " + isSkinned + "<br>");
		infoText.Append("Material: " + material + "<br>");
		infoText.Append("<br>");

		// save number of uvs
		this->numUvs = uvs;
		this->hasVertexColors = colors > 0;
	}
	infoText.Append("<br>");
	infoText.Append("<b>Number of meshes: " + String::FromInt(meshCount) + "</b><br>");
	infoText.Append("<hr>");
	infoText.Append("<br>");

	int rootSkeletonCount = 0;
	int skeletonCount = scene->GetSrcObjectCount<FbxSkeleton>();
	if (skeletonCount > 0)
	{
		infoText.Append("Skeletons: <br>");
		for (int skeletonIndex = 0; skeletonIndex < skeletonCount; skeletonIndex++)
		{
			FbxSkeleton* skeleton = scene->GetSrcObject<FbxSkeleton>(skeletonIndex);
			// we only want to construct node trees if we have a root node
			if (skeleton->IsSkeletonRoot())
			{
				rootSkeletonCount++;
				String skeletonName = skeleton->GetNode()->GetName();
				infoText.Append("Skeleton: " + skeletonName + "<br>");
				infoText.Append("Joints: <br>");

				Array<String> joints;
				this->GetJointsRecursive(joints, skeleton->GetNode());
				for (int jointIndex = 0; jointIndex < joints.Size(); jointIndex++)
				{
					infoText.Append(joints[jointIndex]);					
					if (jointIndex < joints.Size() - 1)
					{
						infoText.Append(", ");
					}
				}
				infoText.Append("<br>");
				infoText.Append("<br>");
				infoText.Append("<b>Number of joints: " + String::FromInt(joints.Size()) + "</b><br>");
				infoText.Append("<br>");
				infoText.Append("<br>");
			}
		}
		infoText.Append("<b>Number of skeletons: " + String::FromInt(rootSkeletonCount) + "</b><br>");
	}

	infoText.Append("<br>");
	return infoText;
}

//------------------------------------------------------------------------------
/**
*/
void
NFbxReader::GetJointsRecursive(Util::Array<Util::String>& joints, FbxNode* parent)
{
	int childCount = parent->GetChildCount();
	for (int childIndex = 0; childIndex < childCount; childIndex++)
	{
		FbxNode* child = parent->GetChild(childIndex);

		if (child->GetSkeleton())
		{
			String jointName = child->GetName();
			joints.Append(jointName);
			GetJointsRecursive(joints, child);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
const bool
NFbxReader::HasPhysics() const
{
	int transformCount = scene->GetSrcObjectCount<FbxNode>();
	// gather all transforms
	for (int transformIndex = 0; transformIndex < transformCount; transformIndex++)
	{		
		FbxNode * node = scene->GetSrcObject<FbxNode>(transformIndex);
		Util::String name = node->GetName();
		if(name == "physics")
			return true;		
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
FbxScene* 
NFbxReader::GetScene()
{
	return this->scene;
}

} // namespace ModelImporter