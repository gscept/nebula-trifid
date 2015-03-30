//------------------------------------------------------------------------------
//  fbx_reader.cc
//  LTU Luleå University of Technology
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fbx_reader.h"
#include "toolkitutil\animutil\animbuildercurve.h"
#include "math/float4.h"
#include <string>
#include "util/stringatom.h"
#include "fbxfilesdk/kfbxmath/kfbxvector2.h"


using namespace Util;
using namespace Math;
using namespace ToolkitUtil;
using namespace CoreAnimation;

#define KEYDURATION 40

namespace Toolkit
{
__ImplementClass(FBX_Reader, 'COLR', Core::RefCounted);

void FBX_Reader::InitiateFBXIO(ReadState iReadState)
{
	if (!isInitiated)
	{
		// Create the FBX SDK manager
		this->lSdkManager = KFbxSdkManager::Create();
	}
	switch (iReadState)
	{
	case AnimReadState:
		{
			// Create an IOSettings object. IOSROOT is defined in kfbxiosettingspath.h.
			KFbxIOSettings * ios = KFbxIOSettings::Create(lSdkManager, IOSROOT );
			lSdkManager->SetIOSettings(ios);

			// Import options determine what kind of data is to be imported.
			// True is the default, but here we’ll set some to true explicitly, and others to false.
			(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_MATERIAL,        false);
			(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE,         false);
			(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_LINK,            false);
			(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE,           false);
			(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO,            false);
			(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION,       true);
			(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, false);

			
			jointCount = 0;
			keyCount = 0;
			keyStride = 0;
			scaleFactor = 1;
		}
		break;
	case SkinReadState:
		{
			// Create an IOSettings object. IOSROOT is defined in kfbxiosettingspath.h.
			KFbxIOSettings * ios = KFbxIOSettings::Create(lSdkManager, IOSROOT );
			lSdkManager->SetIOSettings(ios);

			// Import options determine what kind of data is to be imported.
			// True is the default, but here we’ll set some to true explicitly, and others to false.
			(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_MATERIAL,        true);
			(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE,         true);
			(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_LINK,            true);
			(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE,           true);
			(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO,            true);
			(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION,       true);
			(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

			scaleFactor = 1;
		}
		break;
	}
	isInitiated = true;
}

void FBX_Reader::AddCurves( KFbxNode* iNode, KFbxAnimLayer* iLayer, SizeT iNumKeys)
 {
	KFbxAnimCurve* animCurveX;
	KFbxAnimCurve* animCurveY;
	KFbxAnimCurve* animCurveZ;
	bool translationExists, rotationExists, scalingExists;
	translationExists = rotationExists = scalingExists = false;
	AnimBuilderCurve translationCurve;
	AnimBuilderCurve rotationCurve;
	AnimBuilderCurve scalingCurve;
	KFbxXMatrix matrix;
	
	animCurveX = iNode->LclTranslation.GetCurve<KFbxAnimCurve>(iLayer, KFCURVENODE_T_X);
	animCurveY = iNode->LclTranslation.GetCurve<KFbxAnimCurve>(iLayer, KFCURVENODE_T_Y);
	animCurveZ = iNode->LclTranslation.GetCurve<KFbxAnimCurve>(iLayer, KFCURVENODE_T_Z);

	if(animCurveX && animCurveY && animCurveZ)
	{
		translationExists = true;
		this->animPreLooping = (animCurveX->GetPreExtrapolation() == KFbxAnimCurveBase::eREPETITION);
		this->animPostLooping = (animCurveX->GetPostExtrapolation() == KFbxAnimCurveBase::eREPETITION);
		translationCurve.SetCurveType(CurveType::Translation);
		translationCurve.ResizeKeyArray(iNumKeys);
		for(int i = 0; i < iNumKeys; i++ )
		{
			float4 key = float4(animCurveX->KeyGetValue(i)/this->scaleFactor, animCurveY->KeyGetValue(i)/this->scaleFactor, animCurveZ->KeyGetValue(i)/this->scaleFactor, 0.0f);
			translationCurve.SetKey(i, key);
			float test = key.y();
			keyCount++;
		}
		translationCurve.SetFirstKeyIndex(keyCount);
		translationCurve.SetStatic(false);
		translationCurve.SetActive(true);
		this->keyStride++;
		
	}

	animCurveX = iNode->LclRotation.GetCurve<KFbxAnimCurve>(iLayer, KFCURVENODE_R_X);
	animCurveY = iNode->LclRotation.GetCurve<KFbxAnimCurve>(iLayer, KFCURVENODE_R_Y);
	animCurveZ = iNode->LclRotation.GetCurve<KFbxAnimCurve>(iLayer, KFCURVENODE_R_Z);

	if(animCurveX && animCurveY && animCurveZ)
	{
		rotationExists = true;
		this->animPreLooping = (animCurveX->GetPreExtrapolation() == KFbxAnimCurveBase::eREPETITION);
		this->animPostLooping = (animCurveX->GetPostExtrapolation() == KFbxAnimCurveBase::eREPETITION);
		rotationCurve.SetCurveType(CurveType::Rotation);
		rotationCurve.ResizeKeyArray(iNumKeys);
		for(int i = 0; i < iNumKeys; i++ )
		{
			KFbxVector4 rotvec = KFbxVector4(animCurveX->KeyGetValue(i),animCurveY->KeyGetValue(i), animCurveZ->KeyGetValue(i),0.0f);
			matrix.SetR(rotvec);
			KFbxQuaternion quat = matrix.GetQ();
			float4 key = float4((float)quat[0],(float)quat[1],(float)quat[2],(float)quat[3]);
 			rotationCurve.SetKey(i, key);
			keyCount++;
		}
		rotationCurve.SetFirstKeyIndex(keyCount);
		rotationCurve.SetStatic(false);
		rotationCurve.SetActive(true);
		this->keyStride++;
	}

	animCurveX = iNode->LclScaling.GetCurve<KFbxAnimCurve>(iLayer, KFCURVENODE_S_X);
	animCurveY = iNode->LclScaling.GetCurve<KFbxAnimCurve>(iLayer, KFCURVENODE_S_Y);
	animCurveZ = iNode->LclScaling.GetCurve<KFbxAnimCurve>(iLayer, KFCURVENODE_S_Z);

	if(animCurveX && animCurveY && animCurveZ)
	{
		scalingExists = true;
		this->animPreLooping = (animCurveX->GetPreExtrapolation() == KFbxAnimCurveBase::eREPETITION);
		this->animPostLooping = (animCurveX->GetPostExtrapolation() == KFbxAnimCurveBase::eREPETITION);
		scalingCurve.SetCurveType(CurveType::Scale);
		scalingCurve.ResizeKeyArray(iNumKeys);
		for(int i = 0; i < iNumKeys; i++ )
		{
			float4 key = float4(animCurveX->KeyGetValue(i), animCurveY->KeyGetValue(i), animCurveZ->KeyGetValue(i), 0.0f);
			scalingCurve.SetKey(i, key);
			keyCount++;
		}
		scalingCurve.SetFirstKeyIndex(keyCount);
		scalingCurve.SetStatic(false);
		scalingCurve.SetActive(true);
		this->keyStride++;
	}
	if(!translationExists)
	{
		translationCurve.SetCurveType(CurveType::Translation);
		translationCurve.SetFirstKeyIndex(0);
		translationCurve.SetStatic(true);
		translationCurve.SetActive(true);
		translationCurve.SetStaticKey(Math::float4((float)iNode->LclTranslation.Get()[0]/this->scaleFactor,(float)iNode->LclTranslation.Get()[1]/this->scaleFactor,(float)iNode->LclTranslation.Get()[2]/this->scaleFactor,0.0f));
	}
	if(!rotationExists)
	{
		rotationCurve.SetCurveType(CurveType::Rotation);
		rotationCurve.SetFirstKeyIndex(0);
		rotationCurve.SetStatic(true);
		rotationCurve.SetActive(true);
		KFbxVector4 rotvec = KFbxVector4((float)iNode->LclRotation.Get()[0],(float)iNode->LclRotation.Get()[1], (float)iNode->LclRotation.Get()[2],0.0f);
		matrix.SetR(rotvec);
		KFbxQuaternion quat = matrix.GetQ();
		float4 key = float4((float)quat[0],(float)quat[1],(float)quat[2],(float)quat[3]);
		rotationCurve.SetStaticKey(key);
	}
	if(!scalingExists)
	{
		scalingCurve.SetCurveType(CurveType::Scale);
		scalingCurve.SetFirstKeyIndex(0);
		scalingCurve.SetStatic(true);
		scalingCurve.SetActive(true);
		scalingCurve.SetStaticKey(Math::float4((float)iNode->LclScaling.Get()[0],(float)iNode->LclScaling.Get()[1],(float)iNode->LclScaling.Get()[2],0.0f));
		
	}
	this->animCurves.Append(translationCurve);
	this->animCurves.Append(rotationCurve);
	this->animCurves.Append(scalingCurve);
}

void FBX_Reader::ReadCurvesRecursive( KFbxNode* iNode, KFbxAnimLayer* iLayer, SizeT iNumKeys, int parentIndex)
{
	this->AddCurves(iNode, iLayer, iNumKeys);
	ModelBuilder::Joint newJoint;
	newJoint.jointName = String(iNode->GetName());
	newJoint.jointIndex = jointCount++;
	newJoint.parentJointIndex = parentIndex;
	newJoint.poseTranslation = Math::float4((float)iNode->LclTranslation.Get()[0]/this->scaleFactor,(float)iNode->LclTranslation.Get()[1]/this->scaleFactor,(float)iNode->LclTranslation.Get()[2]/this->scaleFactor,0.0f);
	
	KFbxXMatrix matrix;
	KFbxVector4 rotvec = KFbxVector4((float)iNode->LclRotation.Get()[0],(float)iNode->LclRotation.Get()[1], (float)iNode->LclRotation.Get()[2],0.0f);
	matrix.SetR(rotvec);
	KFbxQuaternion quat = matrix.GetQ();
	newJoint.poseRotation = float4((float)quat[0],(float)quat[1],(float)quat[2],(float)quat[3]);

	newJoint.poseScale = Math::float4((float)iNode->LclScaling.Get()[0],(float)iNode->LclScaling.Get()[1],(float)iNode->LclScaling.Get()[2],0.0f);

	int childCount = iNode->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		this->ReadCurvesRecursive(iNode->GetChild(i), iLayer, iNumKeys, parentIndex++);
	}
}

bool FBX_Reader::ReadAnimation(const char* lFilename, ToolkitUtil::AnimBuilder& animBuilder, ModelBuilder::Model* model)
{
	KFbxScene* lScene = this->ImportScene(lFilename);

	if (!lScene)
	{
		return false;
	}
	this->model = model;
	// Convert Unit System to what is used in this example, if needed
	KFbxSystemUnit SceneSystemUnit = lScene->GetGlobalSettings().GetSystemUnit();
	this->scaleFactor = (float)SceneSystemUnit.GetScaleFactor();
	if( this->scaleFactor != 1.0f )
	{
		KFbxSystemUnit OurSystemUnit(1.0);
		OurSystemUnit.ConvertScene(lScene);
	}

	KFbxNode* rootNode = lScene->GetRootNode();
	KFbxAnimStack* animStack;
	KFbxAnimLayer* animLayer;
	
	String name;
	SizeT numKeys;
	SizeT numCurves;

	// The skeleton must always be named skeleton
	KFbxNode* skeletonNode = lScene->FindNodeByName("skeleton");
	lScene->GetRootNode()->ResetPivotSetAndConvertAnimation(25.0);
	int countAnimStacks = lScene->GetSrcObjectCount(FBX_TYPE(KFbxAnimStack));
	for (int i = 0; i < countAnimStacks; i++)
	{
		animStack = lScene->GetSrcObject(FBX_TYPE(KFbxAnimStack), i);
		numKeys = 1+(SizeT)(animStack->LocalStop.Get().GetMilliSeconds()-animStack->LocalStart.Get().GetMilliSeconds())/KEYDURATION;
		// Get the first anim layer of the stack
		animLayer = animStack->GetSrcObject(FBX_TYPE(KFbxAnimLayer), 0);
		
		int parentIndex = -1;
		int childCount = skeletonNode->GetChildCount();
		for (int i = 0; i < childCount; i++)
		{
			this->ReadCurvesRecursive(skeletonNode->GetChild(i), animLayer, numKeys, parentIndex);
		}

		numCurves = this->animCurves.Size();
		if (numCurves == 0)
		{
			continue;
		}

		// Build the clip, empty the curve list
		AnimBuilderClip clip;
		name = String(lFilename);
		name = name.ExtractFileName();
		name.StripFileExtension();
		clip.SetName(Util::StringAtom(name));
		clip.ReserveCurves(numCurves);
		for (IndexT i = 0; i < numCurves; i++)
		{
			AnimBuilderCurve curve = this->animCurves[i];
			clip.AddCurve(curve);
		}
		clip.SetNumKeys(numKeys);
		clip.SetKeyStride(this->keyStride);
		clip.SetKeyDuration(KEYDURATION);
		clip.SetStartKeyIndex(0);
		clip.SetPreInfinityType(animPreLooping ? InfinityType::Cycle : InfinityType::Constant);
		clip.SetPostInfinityType(animPostLooping ? InfinityType::Cycle : InfinityType::Constant);
		animBuilder.AddClip(clip);

		this->animCurves.Clear();
	}
	animBuilder.FixAnimCurveFirstKeyIndices();
	animBuilder.FixInactiveCurveStaticKeyValues();
	if(animDrivenMotionFlag)
	{
		animBuilder.BuildVelocityCurves();
		animBuilder.TrimEnd(1);
	}
	return true;
}
void FBX_Reader::CreateJointMapRecursiveBreadthFirst(Util::Queue<KFbxNode*>& nodeQueue, int& iIndex )
{
	if(nodeQueue.IsEmpty())
		return;
	KFbxNode* node;
	node = nodeQueue.Dequeue();
	int childCount = node->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		nodeQueue.Enqueue(node->GetChild(i));
		this->jointIndexMap.Add(String(node->GetChild(i)->GetName()), iIndex++);
	}
	this->CreateJointMapRecursiveBreadthFirst(nodeQueue, iIndex);
}
void FBX_Reader::CreateJointMapRecursiveDepthFirst(KFbxNode* iNode, int& iIndex)
{
	this->jointIndexMap.Add(String(iNode->GetName()), iIndex++);
	int childCount = iNode->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		this->CreateJointMapRecursiveDepthFirst(iNode->GetChild(i), iIndex);
	}	
}

bool FBX_Reader::ReadSkin( const char* lFilename, ToolkitUtil::MeshBuilder& meshBuilder, ModelBuilder::Model* model )
{
	KFbxScene* lScene = this->ImportScene(lFilename);
	int numQuadsOrLarger = 0;

	if (!lScene)
	{
		return false;
	}
	this->model = model;
	
	KFbxNode* skeletonNode = lScene->FindNodeByName("skeleton");
	int mapindex = 0;
	/*Util::Queue<KFbxNode*> q;
	q.Enqueue(skeletonNode);
	this->jointIndexMap.Add(String(skeletonNode->GetName()), mapindex++);
	CreateJointMapRecursiveBreadthFirst(q, mapindex);*/
	CreateJointMapRecursiveDepthFirst(skeletonNode, mapindex);



	// Convert Unit System to what is used in this example, if needed
	KFbxSystemUnit SceneSystemUnit = lScene->GetGlobalSettings().GetSystemUnit();
	this->scaleFactor = (float)SceneSystemUnit.GetScaleFactor();
	if( this->scaleFactor != 1.0f )
	{
		KFbxSystemUnit OurSystemUnit(1.0);
		OurSystemUnit.ConvertScene(lScene);
	}

	KFbxNode* rootNode = lScene->GetRootNode();
	rootNode->ResetPivotSetAndConvertAnimation(25.0);
	int countMeshNodes = lScene->GetSrcObjectCount(FBX_TYPE(KFbxMesh));
	for (int meshIndex = 0; meshIndex < countMeshNodes; meshIndex++)
	{
		KFbxMesh* mesh = lScene->GetSrcObject(FBX_TYPE(KFbxMesh), meshIndex);
		//KFbxMesh* mesh = lScene->FindSrcObject(FBX_TYPE(KFbxMesh),"skins");
		KFbxNode* pNode = mesh->GetNode();
		KFbxNode* parent = pNode->GetParent();
		String parentname(parent->GetName());
		if (parentname != "model")
		{
			continue;
		}
		ReadMaterial(mesh);
		
		int lTotalClusterCount = 0;
		int lSkinCount= 0;
		int lVertexCount = mesh->GetControlPointsCount();
		int triangleCount = mesh->GetPolygonCount();

		// No vertex to draw.
		if (lVertexCount == 0)
		{
			return false;
		}
		for (int vertexIndex = 0; vertexIndex < lVertexCount; vertexIndex++)
		{
			KFbxVector4 v = mesh->GetControlPointAt(vertexIndex);
			MeshBuilderVertex vertex;
			vertex.SetComponent(MeshBuilderVertex::CoordIndex, float4((float)v[0]/scaleFactor,(float)v[1]/scaleFactor,(float)v[2]/scaleFactor,1.0f));
			meshBuilder.AddVertex(vertex);
		}
		int vertexId = 0;
		for (int polygonIndex = 0; polygonIndex < triangleCount; polygonIndex++)
		{
			int polygonSize = mesh->GetPolygonSize(polygonIndex);
			MeshBuilderTriangle triangle;
			int groupIndex = mesh->GetPolygonGroup(polygonIndex);
			groupIndex = groupIndex < 0 ? 0 : groupIndex;
			triangle.SetGroupId(groupIndex);

			for(int polygonVertexIndex = 0; polygonVertexIndex < polygonSize; polygonVertexIndex++)
			{
				if (polygonSize > 3)
				{
					numQuadsOrLarger++;
					continue;
				}
				int polygonVertex = mesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
				triangle.SetVertexIndex(polygonVertexIndex, polygonVertex);
				MeshBuilderVertex& vertex = meshBuilder.VertexAt(polygonVertex);
				for (int l = 0; l < mesh->GetElementVertexColorCount(); l++)
				{
					KFbxGeometryElementVertexColor* leVtxc = mesh->GetElementVertexColor( l);
					switch (leVtxc->GetMappingMode())
					{
					case KFbxGeometryElement::eBY_CONTROL_POINT:
						switch (leVtxc->GetReferenceMode())
						{
						case KFbxGeometryElement::eDIRECT:
							{
								KFbxColor color = leVtxc->GetDirectArray().GetAt(polygonVertex);
								vertex.SetComponent(MeshBuilderVertex::ColorIndex, float4((float)color[0],(float)color[1],(float)color[2],(float)color[3]));
							}	
							break;
						case KFbxGeometryElement::eINDEX_TO_DIRECT:
							{
								KFbxColor color = leVtxc->GetDirectArray().GetAt(leVtxc->GetIndexArray().GetAt(polygonVertex));
								vertex.SetComponent(MeshBuilderVertex::ColorIndex, float4((float)color[0],(float)color[1],(float)color[2],(float)color[3]));
							}
							break;
						default:
							break; // other reference modes not shown here!
						}
						break;

					case KFbxGeometryElement::eBY_POLYGON_VERTEX:
						{
							switch (leVtxc->GetReferenceMode())
							{
							case KFbxGeometryElement::eDIRECT:
								{
									KFbxColor color = leVtxc->GetDirectArray().GetAt(vertexId);
									vertex.SetComponent(MeshBuilderVertex::ColorIndex, float4((float)color[0],(float)color[1],(float)color[2],(float)color[3]));
								}
								break;
							case KFbxGeometryElement::eINDEX_TO_DIRECT:
								{
									KFbxColor color = leVtxc->GetDirectArray().GetAt(leVtxc->GetIndexArray().GetAt(vertexId));
									vertex.SetComponent(MeshBuilderVertex::ColorIndex, float4((float)color[0],(float)color[1],(float)color[2],(float)color[3]));
								}
								break;
							default:
								break; // other reference modes not shown here!
							}
						}
						break;

					case KFbxGeometryElement::eBY_POLYGON: // doesn't make much sense for UVs
					case KFbxGeometryElement::eALL_SAME:   // doesn't make much sense for UVs
					case KFbxGeometryElement::eNONE:       // doesn't make much sense for UVs
						break;
					}
				}
				for (int l = 0; l < mesh->GetElementUVCount(); l++)
				{
					if(l>3)
						break;
					KFbxGeometryElementUV* leUV = mesh->GetElementUV(l);
					switch (leUV->GetMappingMode())
					{
					case KFbxGeometryElement::eBY_CONTROL_POINT:
						switch (leUV->GetReferenceMode())
						{
						case KFbxGeometryElement::eDIRECT:
							{
								KFbxVector2 uv = leUV->GetDirectArray().GetAt(polygonVertex);
								vertex.SetComponent((MeshBuilderVertex::ComponentIndex)(MeshBuilderVertex::Uv0Index+l), float4((float)uv[0],(float)uv[1],0.0f,0.0f));
							}
							break;
						case KFbxGeometryElement::eINDEX_TO_DIRECT:
							{
								KFbxVector2 uv = leUV->GetDirectArray().GetAt(leUV->GetIndexArray().GetAt(polygonVertex));
								vertex.SetComponent((MeshBuilderVertex::ComponentIndex)(MeshBuilderVertex::Uv0Index+l), float4((float)uv[0],(float)uv[1],0.0f,0.0f));
							}
							break;
						default:
							break; // other reference modes not shown here!
						}
						break;

					case KFbxGeometryElement::eBY_POLYGON_VERTEX:
						{
							int lTextureUVIndex = mesh->GetTextureUVIndex(polygonIndex, polygonVertexIndex);
							switch (leUV->GetReferenceMode())
							{
							case KFbxGeometryElement::eDIRECT:
							case KFbxGeometryElement::eINDEX_TO_DIRECT:
								{
									KFbxVector2 uv = leUV->GetDirectArray().GetAt(lTextureUVIndex);
									//FIXME: (+l?)
									vertex.SetComponent((MeshBuilderVertex::ComponentIndex)(MeshBuilderVertex::Uv0Index+l), float4((float)uv[0],(float)uv[1],0.0f,0.0f));
								}
								break;
							default:
								break; // other reference modes not shown here!
							}
						}
						break;

					case KFbxGeometryElement::eBY_POLYGON: // doesn't make much sense for UVs
					case KFbxGeometryElement::eALL_SAME:   // doesn't make much sense for UVs
					case KFbxGeometryElement::eNONE:       // doesn't make much sense for UVs
						break;
					}
				}
				for(int l = 0; l < mesh->GetElementNormalCount(); l++)
				{
					KFbxGeometryElementNormal* leNormal = mesh->GetElementNormal(l);
					if(leNormal->GetMappingMode() == KFbxGeometryElement::eBY_POLYGON_VERTEX)
					{
						switch (leNormal->GetReferenceMode())
						{
						case KFbxGeometryElement::eDIRECT:
							{
								KFbxVector4 normal = leNormal->GetDirectArray().GetAt(vertexId);
								vertex.SetComponent(MeshBuilderVertex::NormalIndex, float4((float)normal[0],(float)normal[1],(float)normal[2],0.0f));
							}
							break;
						case KFbxGeometryElement::eINDEX_TO_DIRECT:
							{
								KFbxVector4 normal = leNormal->GetDirectArray().GetAt(leNormal->GetIndexArray().GetAt(vertexId));
								vertex.SetComponent(MeshBuilderVertex::NormalIndex, float4((float)normal[0],(float)normal[1],(float)normal[2],0.0f));
							}
							break;
						default:
							break; // other reference modes not shown here!
						}
					}

				}
				if(mesh->GetElementTangentCount() > 0)
				{
					KFbxGeometryElementTangent* leTangent = mesh->GetElementTangent(0);
					if(leTangent->GetMappingMode() == KFbxGeometryElement::eBY_POLYGON_VERTEX)
					{
						switch (leTangent->GetReferenceMode())
						{
						case KFbxGeometryElement::eDIRECT:
							{
								KFbxVector4 tangent = leTangent->GetDirectArray().GetAt(vertexId);
								vertex.SetComponent(MeshBuilderVertex::TangentIndex, float4((float)tangent[0],(float)tangent[1],(float)tangent[2],0.0f));
							}
							break;
						case KFbxGeometryElement::eINDEX_TO_DIRECT:
							{
								KFbxVector4 tangent = leTangent->GetDirectArray().GetAt(leTangent->GetIndexArray().GetAt(vertexId));
								vertex.SetComponent(MeshBuilderVertex::TangentIndex, float4((float)tangent[0],(float)tangent[1],(float)tangent[2],0.0f));
							}
							break;
						default:
							break; // other reference modes not shown here!
						}
					}

				}
				if(mesh->GetElementBinormalCount() > 0)
				{
					KFbxGeometryElementBinormal* leBinormal = mesh->GetElementBinormal(0);
					if(leBinormal->GetMappingMode() == KFbxGeometryElement::eBY_POLYGON_VERTEX)
					{
						switch (leBinormal->GetReferenceMode())
						{
						case KFbxGeometryElement::eDIRECT:
							{
								KFbxVector4 tangent = leBinormal->GetDirectArray().GetAt(vertexId);
								vertex.SetComponent(MeshBuilderVertex::BinormalIndex, float4((float)tangent[0],(float)tangent[1],(float)tangent[2],0.0f));
							}
							break;
						case KFbxGeometryElement::eINDEX_TO_DIRECT:
							{
								KFbxVector4 tangent = leBinormal->GetDirectArray().GetAt(leBinormal->GetIndexArray().GetAt(vertexId));
								vertex.SetComponent(MeshBuilderVertex::BinormalIndex, float4((float)tangent[0],(float)tangent[1],(float)tangent[2],0.0f));
							}
							break;
						default:
							break; // other reference modes not shown here!
						}
					}
				}
				vertexId++;
			}
			meshBuilder.AddTriangle(triangle);
		}


		lSkinCount = mesh->GetDeformerCount(KFbxDeformer::eSKIN);
		vertexJointWeights = new float[lVertexCount*9];
		

		for(int k=0; k<lVertexCount*9;k++)
		{
			vertexJointWeights[k] = 0.0f;
		}
		for( int skinIndex = 0; skinIndex < lSkinCount; skinIndex++)
		{
			// Set up joint hierarchy
			KFbxSkin * skin = (KFbxSkin *)mesh->GetDeformer(skinIndex, KFbxDeformer::eSKIN);
			int lClusterCount = skin->GetClusterCount();
			/*int jointIndex = 0;
			Array<String> jointNames;
			jointIndexMap.Clear();
			for( int clusterIndex=0; clusterIndex < lClusterCount; clusterIndex++)
			{
				KFbxCluster* cluster = skin->GetCluster(clusterIndex);
				KFbxNode* jointNode = cluster->GetLink();
				if (!jointNode)
					continue;
				int lVertexIndexCount = cluster->GetControlPointIndicesCount();
				if (lVertexIndexCount > 0)
				{
					jointNames.Append(String(jointNode->GetName()));
				}
			}*/

			//this->CreateJointMapRecursiveDepthFirst(skeletonNode,jointIndex,jointNames);
			/////////////////////////
			//KFbxSkin * skin = (KFbxSkin *)mesh->GetDeformer(skinIndex, KFbxDeformer::eSKIN);
			//int lClusterCount = skin->GetClusterCount();
			for( int clusterIndex=0; clusterIndex < lClusterCount; clusterIndex++)
			{
				KFbxCluster* cluster = skin->GetCluster(clusterIndex);
				KFbxNode* jointNode = cluster->GetLink();
				std::string jointName = jointNode->GetName();
				if (!jointNode)
					continue;

				int lVertexIndexCount = cluster->GetControlPointIndicesCount();
				for (int k = 0; k < lVertexIndexCount; k++) 
				{            
					int lIndex = cluster->GetControlPointIndices()[k];
					double lWeight = cluster->GetControlPointWeights()[k];

					// Sometimes, the mesh can have less points than at the time of the skinning
					// because a smooth operator was active when skinning but has been deactivated during export.
					if (lIndex >= lVertexCount)
						continue;

					MeshBuilderVertex& vertex = meshBuilder.VertexAt(lIndex);

					/*if(lWeight == 0.0)
					{
						continue;
					}*/

					float* vertexData = vertexJointWeights+(lIndex*9);
					float weightCount = vertexData[0];
					if (weightCount > 3)
					{
						continue;
					}
					vertexData[(int)weightCount+1] = (float)lWeight;
					vertexData[5+(int)weightCount] = (float)clusterIndex;//(float)this->jointIndexMap[String(jointNode->GetName())];
					vertexData[0]++;
					/*
					float4 weights(vertex.GetComponent(MeshBuilderVertex::WeightsIndex));
					float4 jointIndices(vertex.GetComponent(MeshBuilderVertex::JIndicesIndex));
					float w[4] = {weights.x(),weights.y(),weights.z(),weights.w()};
					float j[4] = {jointIndices.x(),jointIndices.y(),jointIndices.z(),jointIndices.w()};
					for (int weightIndex = 0; weightIndex < 4; weightIndex++)
					{
						if (w[weightIndex] == 0.0f)
						{
							w[weightIndex] = (float)lWeight;
							vertex.SetComponent(MeshBuilderVertex::WeightsIndex, float4(w[0], w[1], w[2], w[3]));
							String dinmamma = String(jointNode->GetName());
							j[weightIndex] = (float)this->jointIndexMap[dinmamma];//(float)clusterIndex;
							vertex.SetComponent(MeshBuilderVertex::JIndicesIndex, float4(j[0], j[1], j[2], j[3]));
							break;
						}
					}*/
				}//For each vertex 

			}//For each cluster
			lTotalClusterCount += lClusterCount;
			for(int k = 0; k<lVertexCount; k++)
			{
				MeshBuilderVertex& vertex = meshBuilder.VertexAt(k);
				float* weights = vertexJointWeights+(k*9)+1;
				float* joints = vertexJointWeights+(k*9)+5;
				vertex.SetComponent(MeshBuilderVertex::WeightsIndex, float4(weights[0], weights[1], weights[2], weights[3]));
				vertex.SetComponent(MeshBuilderVertex::JIndicesIndex, float4(joints[0], joints[1], joints[2], joints[3]));
			}
			
		}//For each skin

	}
	jointIndexMap.Clear();
	return true;

}

void FBX_Reader::ReadMaterial(KFbxGeometry* pGeometry)
{
	int lMaterialIndex;
	KFbxProperty lProperty;
	if(pGeometry->GetNode()==NULL)
		return;
	int lNbMat = pGeometry->GetNode()->GetSrcObjectCount(KFbxSurfaceMaterial::ClassId);
	for (lMaterialIndex = 0; lMaterialIndex < lNbMat; lMaterialIndex++)
	{
		KFbxSurfaceMaterial *lMaterial = (KFbxSurfaceMaterial *)pGeometry->GetNode()->GetSrcObject(KFbxSurfaceMaterial::ClassId, lMaterialIndex);
		bool lDisplayHeader = true;
		ModelBuilder::Skin newSkin;

		//go through all the possible textures
		if(lMaterial)
		{
			String(lMaterial->GetName());
			int lCount = 0;
			KFbxProperty lProperty = lMaterial->GetFirstProperty();
			while (lProperty.IsValid())
			{
				if (lProperty.GetFlag(KFbxProperty::eUSER))
				{
					lCount++;
					EFbxType type = lProperty.GetPropertyDataType().GetType();
					switch(type)
					{
					case eINTEGER1:
						{
							newSkin.shaderInts.Add(String(lProperty.GetName().Buffer()), KFbxGet<int>(lProperty));
							break;
						}
					case eFLOAT1:
						{
							newSkin.shaderFloats.Add(String(lProperty.GetName().Buffer()), KFbxGet<float>(lProperty));
							break;
						}
					case eDOUBLE1:
						{
							newSkin.shaderFloats.Add(String(lProperty.GetName().Buffer()), (float)(KFbxGet<double>(lProperty)));
							break;
						}
					case eSTRING:
						{
							KString name = lProperty.GetName();
							if(name.Compare("ShaderFileName"))
							{
								newSkin.shaderFileName = String(KFbxGet<KString>(lProperty).Buffer());
							}
							else if(name.Compare("ShaderTechnique"))
							{
								newSkin.shaderTechnique = String(KFbxGet<KString>(lProperty).Buffer());
							}
							else
							{
								newSkin.shaderTextures.Add(String(lProperty.GetName().Buffer()), String(KFbxGet<KString>(lProperty).Buffer()));
							}
							break;
						}
					case eBOOL1:
						{
							newSkin.shaderBools.Add(String(lProperty.GetName().Buffer()), KFbxGet<bool>(lProperty));
							break;
						}
					case eENUM:
						{
							newSkin.shaderInts.Add(String(lProperty.GetName().Buffer()), KFbxGet<int>(lProperty));
							break;
						}
					default:
						break;
					}
				}

				lProperty = lMaterial->GetNextProperty(lProperty);
			}

		}//end if(lMaterial)

	}// end for lMaterialIndex     




	//int lMaterialCount = 0;
	//KFbxNode* lNode = NULL;
	//if(pGeometry){
	//	lNode = pGeometry->GetNode();
	//	if(lNode)
	//		lMaterialCount = lNode->GetMaterialCount();    
	//}

	//if (lMaterialCount > 0)
	//{
	//	KFbxPropertyDouble3 lKFbxDouble3;
	//	KFbxPropertyDouble1 lKFbxDouble1;
	//	KFbxColor theColor;

	//	for (int lCount = 0; lCount < lMaterialCount; lCount ++)
	//	{
	//		KFbxSurfaceMaterial *lMaterial = lNode->GetMaterial(lCount);

	//		//Get the implementation to see if it's a hardware shader.
	//		const KFbxImplementation* lImplementation = GetImplementation(lMaterial, "ImplementationHLSL");
	//		KString lImplemenationType = "HLSL";
	//		if(!lImplementation)
	//		{
	//			lImplementation = GetImplementation(lMaterial, "ImplementationCGFX");
	//			lImplemenationType = "CGFX";
	//		}
	//		if(lImplementation)
	//		{
	//			//Now we have a hardware shader, let's read it
	//			printf("            Hardware Shader Type: %s\n", lImplemenationType.Buffer());
	//			KFbxBindingTable const* lRootTable = lImplementation->GetRootTable();
	//			fbxString lFileName = lRootTable->DescAbsoluteURL.Get();
	//			fbxString lTechniqueName = lRootTable->DescTAG.Get();


	//			KFbxBindingTable const* lTable = lImplementation->GetRootTable();
	//			size_t lEntryNum = lTable->GetEntryCount();

	//			for(int i=0;i <(int)lEntryNum; ++i)
	//			{
	//				const KFbxBindingTableEntry& lEntry = lTable->GetEntry(i);
	//				const char* lEntrySrcType = lEntry.GetEntryType(true); 
	//				KFbxProperty lFbxProp;


	//				KString lTest = lEntry.GetSource();
	//				printf("            Entry: %s\n", lTest.Buffer());


	//				if ( strcmp( KFbxPropertyEntryView::sEntryType, lEntrySrcType ) == 0 )
	//				{   
	//					lFbxProp = lMaterial->FindPropertyHierarchical(lEntry.GetSource()); 
	//					if(!lFbxProp.IsValid())
	//					{
	//						lFbxProp = lMaterial->RootProperty.FindHierarchical(lEntry.GetSource());
	//					}


	//				}
	//				else if( strcmp( KFbxConstantEntryView::sEntryType, lEntrySrcType ) == 0 )
	//				{
	//					lFbxProp = lImplementation->GetConstants().FindHierarchical(lEntry.GetSource());
	//				}
	//				if(lFbxProp.IsValid())
	//				{
	//					if( lFbxProp.GetSrcObjectCount( FBX_TYPE(KFbxTexture) ) > 0 )
	//					{
	//						//do what you want with the textures
	//						for(int j=0; j<lFbxProp.GetSrcObjectCount(FBX_TYPE(KFbxFileTexture)); ++j)
	//						{
	//							KFbxFileTexture *lTex = lFbxProp.GetSrcObject(FBX_TYPE(KFbxFileTexture),j);
	//							printf("           File Texture: %s\n", lTex->GetFileName());
	//						}
	//						for(int j=0; j<lFbxProp.GetSrcObjectCount(FBX_TYPE(KFbxLayeredTexture)); ++j)
	//						{
	//							KFbxLayeredTexture *lTex = lFbxProp.GetSrcObject(FBX_TYPE(KFbxLayeredTexture),j);
	//							printf("        Layered Texture: %s\n", lTex->GetName());
	//						}
	//						for(int j=0; j<lFbxProp.GetSrcObjectCount(FBX_TYPE(KFbxProceduralTexture)); ++j)
	//						{
	//							KFbxProceduralTexture *lTex = lFbxProp.GetSrcObject(FBX_TYPE(KFbxProceduralTexture),j);
	//							printf("     Procedural Texture: %s\n", lTex->GetName());
	//						}
	//					}
	//					else
	//					{
	//						KFbxDataType lFbxType = lFbxProp.GetPropertyDataType();
	//						KString blah = lFbxType.GetName();
	//						if(DTBool == lFbxType)
	//						{
	//						}
	//						else if ( DTInteger == lFbxType ||  DTEnum  == lFbxType )
	//						{
	//						}
	//						else if ( DTFloat == lFbxType)
	//						{

	//						}
	//						else if ( DTDouble == lFbxType)
	//						{
	//						}
	//						else if ( DTString == lFbxType
	//							||  DTUrl  == lFbxType
	//							||  DTXRefUrl  == lFbxType )
	//						{
	//						}
	//						else if ( DTDouble2 == lFbxType)
	//						{
	//							fbxDouble2 lDouble2=KFbxGet <fbxDouble2> (lFbxProp);
	//							KFbxVector2 lVect;
	//							lVect[0] = lDouble2[0];
	//							lVect[1] = lDouble2[1];

	//						}
	//						else if ( DTVector3D == lFbxType||DTDouble3 == lFbxType || DTColor3 == lFbxType)
	//						{
	//							fbxDouble3 lDouble3 = KFbxGet <fbxDouble3> (lFbxProp);


	//							KFbxVector4 lVect;
	//							lVect[0] = lDouble3[0];
	//							lVect[1] = lDouble3[1];
	//							lVect[2] = lDouble3[2];
	//						}

	//						else if ( DTVector4D == lFbxType || DTDouble4 == lFbxType || DTColor4 == lFbxType)
	//						{
	//							fbxDouble4 lDouble4 = KFbxGet <fbxDouble4> (lFbxProp);
	//							KFbxVector4 lVect;
	//							lVect[0] = lDouble4[0];
	//							lVect[1] = lDouble4[1];
	//							lVect[2] = lDouble4[2];
	//							lVect[3] = lDouble4[3];
	//						}
	//						else if ( DTDouble44 == lFbxType)
	//						{
	//							fbxDouble44 lDouble44 = KFbxGet <fbxDouble44> (lFbxProp);
	//							for(int j=0; j<4; ++j)
	//							{

	//								KFbxVector4 lVect;
	//								lVect[0] = lDouble44[j][0];
	//								lVect[1] = lDouble44[j][1];
	//								lVect[2] = lDouble44[j][2];
	//								lVect[3] = lDouble44[j][3];
	//							}

	//						}
	//					}

	//				}   
	//			}
	//		}
	//		else if (lMaterial->GetClassId().Is(KFbxSurfacePhong::ClassId))
	//		{
	//			// We found a Phong material.  Display its properties.

	//			// Display the Ambient Color
	//			lKFbxDouble3 =((KFbxSurfacePhong *) lMaterial)->Ambient;
	//			theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

	//			// Display the Diffuse Color
	//			lKFbxDouble3 =((KFbxSurfacePhong *) lMaterial)->Diffuse;
	//			theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

	//			// Display the Specular Color (unique to Phong materials)
	//			lKFbxDouble3 =((KFbxSurfacePhong *) lMaterial)->Specular;
	//			theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

	//			// Display the Emissive Color
	//			lKFbxDouble3 =((KFbxSurfacePhong *) lMaterial)->Emissive;
	//			theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

	//			//Opacity is Transparency factor now
	//			lKFbxDouble1 =((KFbxSurfacePhong *) lMaterial)->TransparencyFactor;
	//			// Display the Shininess
	//			lKFbxDouble1 =((KFbxSurfacePhong *) lMaterial)->Shininess;

	//			// Display the Reflectivity
	//			lKFbxDouble1 =((KFbxSurfacePhong *) lMaterial)->ReflectionFactor;
	//		}
	//		else if(lMaterial->GetClassId().Is(KFbxSurfaceLambert::ClassId) )
	//		{
	//			// We found a Lambert material. Display its properties.
	//			// Display the Ambient Color
	//			lKFbxDouble3=((KFbxSurfaceLambert *)lMaterial)->Ambient;
	//			theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

	//			// Display the Diffuse Color
	//			lKFbxDouble3 =((KFbxSurfaceLambert *)lMaterial)->Diffuse;
	//			theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

	//			// Display the Emissive
	//			lKFbxDouble3 =((KFbxSurfaceLambert *)lMaterial)->Emissive;
	//			theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

	//			// Display the Opacity
	//			lKFbxDouble1 =((KFbxSurfaceLambert *)lMaterial)->TransparencyFactor;
	//		}
	//		else
	//		{
	//			//DisplayString("Unknown type of Material");
	//		}

	//		KFbxPropertyString lString;
	//		lString = lMaterial->ShadingModel;
	//	}
	//}
}

KFbxScene* FBX_Reader::ImportScene(const char* iFilename)
{
	n_assert2(isInitiated, "Toolkit::FBX_Reader::ImportScene, the FBX reader has not been initiated.");

	// Create an importer.
	KFbxImporter* lImporter = KFbxImporter::Create(lSdkManager, "");

	// Initialize the importer.
	bool lImportStatus = lImporter->Initialize(iFilename, -1, lSdkManager->GetIOSettings());

	// Create a new scene so it can be populated by the imported file.
	KFbxScene* lScene = KFbxScene::Create(lSdkManager,"myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

	// The file has been imported; we can get rid of the importer.
	lImporter->Destroy();

	return lScene;
}

} // namespace Toolkit