#pragma once
//------------------------------------------------------------------------------
/**
    @class Toolkit::FBX_Reader
    
    Reader class for fbx files.
    
    LTU Luleå University of Technology
*/
//------------------------------------------------------------------------------
#include "core\refcounted.h"
#include <fbxsdk.h>
#include <fbxfilesdk/kfbxio/kfbxiosettings.h>
#include "toolkitutil/animutil/animbuilder.h"
#include <set>
#include "toolkitutil/meshutil/meshbuilder.h"
#include "util/queue.h"
#include "modelbuilder.h"


namespace Toolkit
{
	enum ReadState 
	{
		AnimReadState,
		SkinReadState,
		MeshReadState
	};
class FBX_Reader : public Core::RefCounted
{
	__DeclareClass(FBX_Reader);
public:
    /// constructor
	FBX_Reader(){isInitiated = false;}
    /// destructor
	~FBX_Reader(){lSdkManager->Destroy();}
	/// Initiate the fbx i/o interface
	void InitiateFBXIO(ReadState iReadState);
	/// read from fbx file
	bool ReadAnimation(const char* lFilename, ToolkitUtil::AnimBuilder& animBuilder, ToolkitUtil::ModelBuilder::Model* model);
	/// read from fbx file
	bool ReadSkin(const char* lFilename, ToolkitUtil::MeshBuilder& meshBuilder, ToolkitUtil::ModelBuilder::Model* model);

	void SetAnimDrivenMotionFlag(bool b){animDrivenMotionFlag = b;}
private:
	/// 
	void AddCurves(KFbxNode* iNode, KFbxAnimLayer* iLayer, SizeT iNumKeys);
	/// 
	void ReadCurvesRecursive(KFbxNode* iNode, KFbxAnimLayer* iLayer, SizeT iNumKeys, int parentIndex);
	KFbxScene* ImportScene(const char* lFilename);
	void CreateJointMapRecursiveBreadthFirst(Util::Queue<KFbxNode*>& nodeQueue, int& iIndex );
	void CreateJointMapRecursiveDepthFirst(KFbxNode* iNode, int& iIndex);
	void ReadMaterial(KFbxGeometry* pGeometry);

	bool isInitiated;
	KFbxSdkManager* lSdkManager;
	Util::Array<ToolkitUtil::AnimBuilderCurve> animCurves;
	bool animPostLooping;
	bool animPreLooping;
	bool animDrivenMotionFlag;
	int jointCount;
	int keyCount;
	int keyStride;
	float scaleFactor;
	Util::Dictionary<Util::String,int> jointIndexMap;
	float* vertexJointWeights;
	ToolkitUtil::ModelBuilder::Model* model;
};
} // namespace Toolkit
//------------------------------------------------------------------------------