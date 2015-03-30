#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QFileInfo>
#include <QDateTime>
#include "fbxreader.h"
#include "tools/pathresolver.h"
#include "tools/progressnotifier.h"

using namespace QtTools;

namespace Importer
{

//------------------------------------------------------------------------------
/**
*/
FbxReader::FbxReader() : 
	scene(0),
	isOpen(false)
{
	this->sdkManager = KFbxSdkManager::Create();
}

//------------------------------------------------------------------------------
/**
*/
FbxReader::~FbxReader()
{
	// empty
}


//------------------------------------------------------------------------------
/**
	Global function called from KFbxImporter when loading progress ia returned
*/
bool
ProgressChanged(float progress, KString status)
{
	ProgressNotifier::Instance()->Update(progress, status.Buffer());
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
FbxReader::Open( const QString& file, const QString& category )
{
	Q_ASSERT(!this->isOpen);
	this->file = file;
	this->category = category;

	const QString& path = PathResolver::Instance()->ResolvePath("workgfx:" + category + "/" + file + ".fbx");

	this->scene = KFbxScene::Create(this->sdkManager, "Import scene");

	KFbxImporter* importer = KFbxImporter::Create(this->sdkManager, "Importer");
	KFbxProgressCallback callback = &ProgressChanged;
	importer->SetProgressCallback(callback);
	bool importStatus = importer->Initialize(path.toAscii());
	if (importStatus)
	{
		importStatus = importer->Import(this->scene);
		if (!importStatus)
		{
			int retVal = QMessageBox::critical(this->parentWidget, "Import error", "The file: " + category + "/" + file + ".fbx could not be imported");
			emit Loaded(Failed, file, category);
		}
		else
		{
			emit Loaded(Success, file, category);
		}
	}

	importer->ContentUnload();
	importer->Destroy();
}


//------------------------------------------------------------------------------
/**
*/
void 
FbxReader::Close()
{
	Q_ASSERT(this->isOpen);
	this->scene->ContentUnload();	
	this->scene->Destroy(true);
	this->scene = 0;
}


//------------------------------------------------------------------------------
/**
*/
bool 
FbxReader::IsOpen() const
{
	return this->isOpen;
}


//------------------------------------------------------------------------------
/**
*/
void 
FbxReader::SetParent( QWidget* parent )
{
	this->parentWidget = parent;
}

//------------------------------------------------------------------------------
/**
*/
const QList<QString> 
FbxReader::GetTakes()
{
	QList<QString> takes;
	int animStackCount = scene->GetSrcObjectCount(FBX_TYPE(KFbxAnimStack));
	for (int stackIndex = 0; stackIndex < animStackCount; stackIndex++)
	{
		KFbxAnimStack* animStack = scene->GetSrcObject(FBX_TYPE(KFbxAnimStack), stackIndex);
		int animLayerCount = animStack->GetSrcObjectCount(FBX_TYPE(KFbxAnimLayer));
		KFbxAnimLayer* animLayer = animStack->GetSrcObject(FBX_TYPE(KFbxAnimLayer), 0);
		int animCurveNodeCount = animLayer->GetSrcObjectCount(FBX_TYPE(KFbxAnimCurveNode));
		if (animCurveNodeCount > 0)
		{
			QString stackname = animStack->GetName();
			takes.append(stackname);
		}			
	}
	return takes;
}

//------------------------------------------------------------------------------
/**
*/
const QString 
FbxReader::GetInfo()
{
	KFbxDocumentInfo* info = this->scene->GetSceneInfo();
	QString infoText;

	QFileInfo fileInfo(PathResolver::Instance()->ResolvePath("workgfx:" + category + "/" + file + ".fbx"));
	int major, minor, revision;
	this->sdkManager->GetFileFormatVersion(major, minor, revision);
	infoText.append("<br>");
	infoText.append("<b>Autodesk FBX</b><br>");
	infoText.append("Version: " + QString::number(major) + "." + QString::number(minor) + " revision: " + QString::number(revision) + "<br>");
	infoText.append("Created: " + fileInfo.created().toString() + "<br>");
	infoText.append("Last modified: " + fileInfo.lastModified().toString() + "<br>");
	infoText.append("<br>");
	infoText.append("<b>File: " + this->category + "/" + this->file + "</b><br>");
	infoText.append("<hr>");		
	infoText.append("<br>");
	infoText.append("Meshes: <br>");
	infoText.append("<br>");

	int meshCount = scene->GetSrcObjectCount(FBX_TYPE(KFbxMesh));
	for (int meshIndex = 0; meshIndex < meshCount; meshIndex++)
	{
		KFbxMesh* fbxMesh = scene->GetSrcObject(FBX_TYPE(KFbxMesh), meshIndex);
		
		QString name = fbxMesh->GetNode()->GetName();
		int vertexCount = fbxMesh->GetControlPointsCount();
		int polyCount = fbxMesh->GetPolygonCount();
		QString isCCW = fbxMesh->CheckIfVertexNormalsCCW() ? "Yes" : "No";
		QString isSkinned = fbxMesh->GetDeformerCount(KFbxDeformer::eSKIN) > 0 ? "Yes" : "No";

		infoText.append("<b>Name: " + name + "</b><br>");
		infoText.append("Vertex count: " + QString::number(vertexCount) + "<br>");
		infoText.append("Polygon count: " + QString::number(polyCount) + "<br>");
		infoText.append("Is CCW: " + isCCW + "<br>");
		infoText.append("Is skinned: " + isSkinned + "<br>");
		infoText.append("<br>");
	}
	infoText.append("<br>");
	infoText.append("<b>Number of meshes: " + QString::number(meshCount) + "</b><br>");
	infoText.append("<hr>");
	infoText.append("<br>");

	int rootSkeletonCount = 0;
	int skeletonCount = scene->GetSrcObjectCount(FBX_TYPE(KFbxSkeleton));
	if (skeletonCount > 0)
	{
		infoText.append("Skeletons: <br>");
		for (int skeletonIndex = 0; skeletonIndex < skeletonCount; skeletonIndex++)
		{
			KFbxSkeleton* skeleton = scene->GetSrcObject(FBX_TYPE(KFbxSkeleton), skeletonIndex);
			// we only want to construct node trees if we have a root node
			if (skeleton->IsSkeletonRoot())
			{
				rootSkeletonCount++;
				QString skeletonName = skeleton->GetNode()->GetName();
				infoText.append("Skeleton: " + skeletonName + "<br>");
				infoText.append("Joints: <br>");

				QList<QString> joints;
				this->GetJointsRecursive(joints, skeleton->GetNode());
				for (int jointIndex = 0; jointIndex < joints.size(); jointIndex++)
				{
					infoText.append(joints[jointIndex]);					
					if (jointIndex < joints.size() - 1)
					{
						infoText.append(", ");
					}
				}
				infoText.append("<br>");
				infoText.append("<br>");
				infoText.append("<b>Number of joints: " + QString::number(joints.size()) + "</b><br>");
				infoText.append("<br>");
				infoText.append("<br>");
			}
		}
		infoText.append("<b>Number of skeletons: " + QString::number(rootSkeletonCount) + "</b><br>");
	}

	infoText.append("<br>");
	return infoText;
}

//------------------------------------------------------------------------------
/**
*/
void 
FbxReader::GetJointsRecursive( QList<QString>& joints, KFbxNode* parent )
{
	int childCount = parent->GetChildCount();
	for (int childIndex = 0; childIndex < childCount; childIndex++)
	{
		KFbxNode* child = parent->GetChild(childIndex);

		if (child->GetSkeleton())
		{
			QString jointName = child->GetName();
			joints.append(jointName);
			GetJointsRecursive(joints, child);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
KFbxScene* 
FbxReader::GetScene()
{
	return this->scene;
}
}