#pragma once
//------------------------------------------------------------------------------
/**
    @class Toolkit::ConvertFbxToNax3App
    
    Application class for 
*/
#include "toolkitutil/toolkitapp.h"
#include "fbx_reader.h"
#include "nax3_writer.h"
#include "toolkitutil/meshutil/meshbuilder.h"
#include "modelbuilder.h"

using namespace ToolkitUtil;

//------------------------------------------------------------------------------
namespace Toolkit
{
class ConvertFbxToNax3App : public ToolkitUtil::ToolkitApp
{
public:
    /// run the application
    virtual void Run();

private:

	/// return a list of files in this directory
	Util::Array<Util::String> GetFileListFromDirectory(const Util::String& directory);
	bool NeedsConversion(const Util::String& srcPath);
	bool NeedsConversion(const Util::String& srcPath, const Util::String& dstPath);
	/// parse command line arguments
	virtual bool ParseCmdLineArgs();
	/// setup project info object
	virtual bool SetupProjectInfo();
	/// print help text
	virtual void ShowHelp();
	/// build path to Nax3 dest file
	Util::String BuildAnimDstPath(const Util::String& categoryName, const Util::String& animFileName);
	Util::String BuildMeshDstPath( const Util::String& categoryName, const Util::String& meshFileName);
	void ConvertAnimationsRecursive(Util::String animDir);
	void ConvertSkinsRecursive(Util::String skinDir);

	bool PartitionMesh(ToolkitUtil::MeshBuilder& srcMesh, ToolkitUtil::MeshBuilder& dstMesh, int maxJointPaletteSize);
	void BuildDestMesh(ToolkitUtil::MeshBuilder& srcMesh, ToolkitUtil::MeshBuilder& dstMesh);
	void ExtractPartitioningData(ToolkitUtil::ModelBuilder::Model& model);
	/// get a joint palette of a partition
	const Util::Array<int>& GetJointPalette(int partitionIndex) const;
	
	Util::String category;
	Util::String animFileName;
	bool forceArg;
	FBX_Reader* reader;
	Nax3_Writer* writer;
	ToolkitUtil::AnimBuilder animBuilder;
	ToolkitUtil::MeshBuilder meshBuilder;
	ToolkitUtil::ModelBuilder modelBuilder;
	Util::Array<String> meshesToCreate;
	/// a partition object
	class Partition
	{
	public:
		/// default constructor
		Partition();
		/// constructor
		Partition(ToolkitUtil::MeshBuilder* mBuilder, int maxJoints, int groupId);
		/// check if a triangle can be added, and if yes, add the triangle
		bool CheckAddTriangle(int triangleIndex);
		/// get joint palette of the partition
		const Util::Array<int>& GetJointPalette() const;
		/// get the triangle indices of the partition
		const Util::Array<int>& GetTriangleIndices() const;
		/// get the original group index the partition belongs to
		int GetGroupId() const;
		/// convert a global joint index into a partition-local joint index
		int GlobalToLocalJointIndex(int globalJointIndex) const;

	private:
		/// add a unique index to an int array
		void AddUniqueJointIndex(Util::Array<int>& intArray, int index);
		/// get all joint indices of a triangle
		void GetTriangleJoints(const ToolkitUtil::MeshBuilderTriangle& tri, Util::Array<int>& triJoints);
		/// get all triangle joint indices not currently in joint palette
		void GetJointIndexDifferenceSet(const Util::Array<int>& triJoints, Util::Array<int>& diffSet);

		enum
		{
			MaxJointsInSkeleton = 1024,
		};

		ToolkitUtil::MeshBuilder* sourceMesh;
		int maxJointPaletteSize;
		int groupId;
		bool jointMask[MaxJointsInSkeleton];
		Util::Array<int> jointPalette;
		Util::Array<int> triangleArray;
	};
	Util::Array<Partition> partitionArray;
	Util::Array<int> groupMappingArray;
}; 

//------------------------------------------------------------------------------
/**
*/
inline
ConvertFbxToNax3App::Partition::Partition() :
    sourceMesh(0),
    maxJointPaletteSize(0),
    groupId(0),
    jointPalette(32, 32),
    triangleArray(2048, 2048)
{
    memset(this->jointMask, 0, sizeof(this->jointMask));
}

//------------------------------------------------------------------------------
/**
*/
inline
ConvertFbxToNax3App::Partition::Partition(MeshBuilder* mBuilder, int maxJoints, int gId) :
    sourceMesh(mBuilder),
    maxJointPaletteSize(maxJoints),
    groupId(gId),
    jointPalette(32, 32),
    triangleArray(2048, 2048)
{
    n_assert(maxJoints <= MaxJointsInSkeleton);
    memset(this->jointMask, 0, sizeof(this->jointMask));
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ConvertFbxToNax3App::Partition::GetGroupId() const
{
    return this->groupId;
}

//------------------------------------------------------------------------------
/**
    Add a unique index to an int array.
*/
inline
void
ConvertFbxToNax3App::Partition::AddUniqueJointIndex(Util::Array<int>& intArray, int index)
{
    int num = intArray.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        if (intArray[i] == index)
        {
            return;
        }
    }
    // fallthrough: new index
    intArray.Append(index);
}

//------------------------------------------------------------------------------
/**
    Fills the provided int array with the unique joint indices in the 
    triangle.
*/
inline
void
ConvertFbxToNax3App::Partition::GetTriangleJoints(const MeshBuilderTriangle& tri, Util::Array<int>& triJoints)
{
    n_assert(this->sourceMesh);

    int index[3];
    tri.GetVertexIndices(index[0], index[1], index[2]);
    int i;
    for (i = 0; i < 3; i++)
    {
        MeshBuilderVertex& v = this->sourceMesh->VertexAt(index[i]);
		const Math::float4& weights = v.GetComponent(MeshBuilderVertex::WeightsIndex);
        const Math::float4& indices = v.GetComponent(MeshBuilderVertex::JIndicesIndex);
        if (weights.x() > 0.0f)
        {
            this->AddUniqueJointIndex(triJoints, int(indices.x()));
        }
        if (weights.y() > 0.0f)
        {
            this->AddUniqueJointIndex(triJoints, int(indices.y()));
        }
        if (weights.z() > 0.0f)
        {
            this->AddUniqueJointIndex(triJoints, int(indices.z()));
        }
        if (weights.w() > 0.0f)
        {
            this->AddUniqueJointIndex(triJoints, int(indices.w()));
        }
    }
}

//------------------------------------------------------------------------------
/**
    Fills the provided diffSet int array with all joint indices in the triJoints
    array which are not currently in the joint palette.
*/
inline
void
ConvertFbxToNax3App::Partition::GetJointIndexDifferenceSet(const Util::Array<int>& triJoints, Util::Array<int>& diffSet)
{
    int num = triJoints.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        if (!this->jointMask[triJoints[i]])
        {
            diffSet.Append(triJoints[i]);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ConvertFbxToNax3App::Partition::CheckAddTriangle(int triangleIndex)
{
    n_assert(this->sourceMesh);
    const MeshBuilderTriangle& tri = this->sourceMesh->TriangleAt(triangleIndex);
    
    // get the unique joints in the triangle
    Util::Array<int> triJoints;
    this->GetTriangleJoints(tri, triJoints);

    // get the difference set between the triangle joint indices and the indices in the joint palette
    Util::Array<int> diffSet;
    this->GetJointIndexDifferenceSet(triJoints, diffSet);

    // if the resulting number of joints would be within the max joint palette size,
    // add the triangle to the partition and update the joint palette
    if ((this->jointPalette.Size() + diffSet.Size()) <= this->maxJointPaletteSize)
    {
        this->triangleArray.Append(triangleIndex);
        int i;
        for (i = 0; i < diffSet.Size(); i++)
        {
            this->jointPalette.Append(diffSet[i]);
            this->jointMask[diffSet[i]] = true;
        }
        return true;
    }

    // fallthrough: could not add the triangle to this partition, because there
    // the partition is or would be full
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::Array<int>&
ConvertFbxToNax3App::Partition::GetJointPalette() const
{
    return this->jointPalette;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::Array<int>&
ConvertFbxToNax3App::Partition::GetTriangleIndices() const
{
    return this->triangleArray;
}

//------------------------------------------------------------------------------
/**
    Converts a global joint palette index into a partition-local joint index.
    If the global joint index cannot be found in the local index, a 0 index
    will be returned.
*/
inline
int
ConvertFbxToNax3App::Partition::GlobalToLocalJointIndex(int globalJointIndex) const
{
    int num = this->jointPalette.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        if (globalJointIndex == this->jointPalette[i])
        {
            return i;
        }
    }
    // fallthrough: globalJointIndex not in partition
    return 0;
}

inline
const Util::Array<int>& 
ConvertFbxToNax3App::GetJointPalette(int partitionIndex) const
{
	return this->partitionArray[partitionIndex].GetJointPalette();
}

//------------------------------------------------------------------------------
/**
	 Set the skin partitioner, this configures the fragments and joint
	 palettes in the skin shape node.
*/
inline
void
ConvertFbxToNax3App::ExtractPartitioningData(ToolkitUtil::ModelBuilder::Model& model)
{
	 //nSkinShapeNode* skinShapeNode=this->AsSkinShapeNode(kernelServer->GetCwd());
	 int numPartitions = this->partitionArray.Size();
	 //const nArray<int>& groupMappingArray=skinPartitioner.GetGroupMappingArray();
	 
	 // first find out, how many skin partitions originated 
	 // from this shape's shader group
	 int partitionIndex;
	 int numFragments=0;
	 for (int shaderGroupIndex = 0; shaderGroupIndex < model.skins.Size(); shaderGroupIndex++)
	 {
		 for (partitionIndex=0; partitionIndex < numPartitions; partitionIndex++)
		 {
			  if (groupMappingArray[partitionIndex] == shaderGroupIndex)
			  {
					numFragments++;
			  }
		 }

		 // add the fragment definition to the skin shape object
		 //skinShapeNode->BeginFragments(numFragments);
		 model.skins[shaderGroupIndex].numFragments = numFragments;
		 int fragIndex=0;
		 for (partitionIndex=0; partitionIndex < numPartitions; partitionIndex++)
		 {
			  if (groupMappingArray[partitionIndex] == shaderGroupIndex)
			  {
				  //skinShapeNode->SetFragGroupIndex(fragIndex, partitionIndex);
				  model.skins[shaderGroupIndex].skinFragmentPrimGroupIndices.Append(partitionIndex);
				  const Util::Array<int>& jointPalette = this->GetJointPalette(partitionIndex);
				  //skinShapeNode->BeginJointPalette(fragIndex, jointPalette.Size());
				  /*int palIndex;
				  for (palIndex=0; palIndex < jointPalette.Size(); palIndex++)
				  {
					  skinShapeNode->SetJointIndex(fragIndex, palIndex, jointPalette[palIndex]);

				  }
				  skinShapeNode->EndJointPalette(fragIndex);*/
				  model.skins[shaderGroupIndex].skinFragmentJointPalettes.Append(jointPalette);
				  fragIndex++;
			  }
		 }
		 //skinShapeNode->EndFragments();
	 }
}

} // namespace Toolkit

//------------------------------------------------------------------------------