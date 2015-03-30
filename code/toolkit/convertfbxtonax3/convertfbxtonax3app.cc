//------------------------------------------------------------------------------
//  convertfbxtonax3app.cc
//  (C) 2011 LTU, Luleå University of Technology
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "convertfbxtonax3app.h"
#include "toolkitutil\animutil\animbuildersaver.h"
#include "toolkitutil\meshutil\meshbuildersaver.h"
#include "modelbuildersaver.h"

namespace Toolkit
{
using namespace ToolkitUtil;
using namespace Util;
using namespace IO;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
void
ConvertFbxToNax3App::Run()
{
	reader = FBX_Reader::Create();
	// parse command line args
	if (!this->ParseCmdLineArgs())
	{
		return;
	}

	// setup the project info object
	if (!this->SetupProjectInfo())
	{
		return;
	}

	if (!this->category.IsValid())
	{
		this->category = "characters";
	}
	// Convert skins to nvx2
	reader->InitiateFBXIO(SkinReadState);
	String skinDir(this->projectInfo.GetPathAttr("SrcDir"));
	skinDir.Append("/gfxlib/test");
	printf("-Converting skins-\n");
	this->ConvertSkinsRecursive(skinDir);

	
	// Convert animations to nax3
	reader->InitiateFBXIO(AnimReadState);
	String animDir(this->projectInfo.GetPathAttr("AnimSrcDir"));
	printf("-Converting animations-\n");
	this->ConvertAnimationsRecursive(animDir);

	// Create n3 model file
	String modelPath("mdl:characters");
	ModelBuilderSaver::SaveN3(modelPath, this->modelBuilder, this->platform);

	return;
}

Util::String ConvertFbxToNax3App::BuildAnimDstPath( const Util::String& categoryName, const Util::String& animFileName)
{
	String nax3FileName = animFileName;
	nax3FileName.StripFileExtension();
	nax3FileName.Append(".nax3");
	String destDir = this->projectInfo.GetAttr("AnimDstDir");
	String cat = categoryName;
	String filename = nax3FileName;

	String path = destDir + "/" + cat + "/" + filename;
	return path;
}
Util::String ConvertFbxToNax3App::BuildMeshDstPath( const Util::String& categoryName, const Util::String& meshFileName)
{
	String nvx2FileName = meshFileName;
	nvx2FileName.StripFileExtension();
	nvx2FileName.Append(".nvx2");
	String destDir = this->projectInfo.GetAttr("DstDir");
	destDir.Append("/meshes");
	String cat = categoryName;
	String filename = nvx2FileName;

	String path = destDir + "/" + cat + "/" + filename;
	return path;
}
//------------------------------------------------------------------------------
/**
    Return all files inside this directory that need a conversion.
*/
Array<String>
ConvertFbxToNax3App::GetFileListFromDirectory(const String& directory)
{
    Array<String> res;
    Array<String> files = IoServer::Instance()->ListFiles(directory, "*.fbx");
    IndexT fileIndex;
    // check each file in this directory if it needs to be converted
    for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
    {
        String srcPath;
        srcPath.Format("%s/%s", directory.AsCharPtr(), files[fileIndex].AsCharPtr());
        if (this->NeedsConversion(srcPath))
        {
            res.Append(srcPath);
        }
    }
    return res;
}
//------------------------------------------------------------------------------
/**
    Perform a file time check to decide whether a fbx file must be
    converted. Build dstPath from project settings and file name of srcPath.
*/
bool
ConvertFbxToNax3App::NeedsConversion(const String& srcPath)
{
    // cut source dir part from srcPath
    String resolvedSrc = AssignRegistry::Instance()->ResolveAssignsInString(srcPath);
    String subPath = resolvedSrc.ExtractToEnd(
        AssignRegistry::Instance()->ResolveAssignsInString(
            this->projectInfo.GetPathAttr("AnimSrcDir")
            )
            .Length()
        );
    subPath.Trim("/");
    String dstPath;
    dstPath.Format("%s/%s",
        this->projectInfo.GetPathAttr("AnimDstDir").AsCharPtr(),
        subPath.AsCharPtr()
    );
    // be sure to remove all file extensions
    while ("" != dstPath.GetFileExtension())
    {
        dstPath.StripFileExtension();
    }
    dstPath.Format("%s.%s", dstPath.AsCharPtr(), "nax3");
    return this->NeedsConversion(srcPath, dstPath);
}

//------------------------------------------------------------------------------
/**
    Perform a file time check to decide whether a texture must be
    converted. Use an explicit dstPath
*/
bool
ConvertFbxToNax3App::NeedsConversion(const String& srcPath, const String& dstPath)
{
    // file time check overriden?
    if (this->forceArg)
    {
        return true;
    }
    // otherwise check file times of src and dst file
    IoServer* ioServer = IoServer::Instance();
    if (ioServer->FileExists(dstPath))
    {
        FileTime srcFileTime = ioServer->GetFileWriteTime(srcPath);
        FileTime dstFileTime = ioServer->GetFileWriteTime(dstPath);
        if (dstFileTime > srcFileTime)
        {
            // dst file newer then src file, don't need to convert
            return false;
        }
    }
    // fallthrough: dst file doesn't exist, or it is older than the src file
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
ConvertFbxToNax3App::ParseCmdLineArgs()
{
    if (ToolkitApp::ParseCmdLineArgs())
    {
        this->forceArg = this->args.GetBoolFlag("-force");
        this->category = this->args.GetString("-cat", "");
        this->animFileName = this->args.GetString("-anim", "");
		this->reader->SetAnimDrivenMotionFlag(this->args.GetBoolFlag("-animdrivenmotion"));
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
ConvertFbxToNax3App::SetupProjectInfo()
{
    if (ToolkitApp::SetupProjectInfo())
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ConvertFbxToNax3App::ShowHelp()
{
    n_printf("Nebula3 animation processing tool.\n"
             "(C) Radon Labs GmbH 2009.\n"
             "-help       -- display this help\n"
             "-platform   -- select platform (win32, xbox360, wii, ps3)\n"
             "-waitforkey -- wait for key when complete\n"
             "-force      -- force export (don't check time stamps)\n"
             "-cat        -- select specific category\n"
             "-anim       -- select specific animation (also needs -cat)\n");
}

void ConvertFbxToNax3App::ConvertAnimationsRecursive(String animDir)
{
	Array<String> _filelist;
	Array<String> dirs = IoServer::Instance()->ListDirectories(animDir, "*");
	for (int i = 0; i < dirs.Size(); i++)
	{
		String directory;
		// Skip .svn folders
		if(dirs[i] == ".svn")
			continue;
		directory.Format("%s/%s", animDir.AsCharPtr(), dirs[i].AsCharPtr());
		_filelist.AppendArray(this->GetFileListFromDirectory(directory));
		// We found fbx files, convert them!
		bool writeFile = false;
		if(_filelist.Size() > 0)
		{
			printf("Converting ");
			printf(dirs[i].AsCharPtr());
			printf("\n");
		}
		for (int j = 0; j < _filelist.Size(); j++)
		{
			printf("processing: ");
			printf(_filelist[j].ExtractFileName().AsCharPtr());
			if(!reader->ReadAnimation(URI(_filelist[j]).AsString().ExtractToEnd(8).AsCharPtr(), this->animBuilder, this->modelBuilder.GetModelByName(dirs[i])))
			{
				printf(" - failed!");
				writeFile = false;
				break;
			}
			else 
			{
				printf(" - success!");
				writeFile = true;
			}
			printf("\n");
		}
		if(writeFile)
		{
			String fileName = dirs[i];
			fileName.Append("_animations");
			String path = this->BuildAnimDstPath(this->category, fileName);
			AnimBuilderSaver::SaveNax3(path, this->animBuilder, this->platform);
		}

		this->animBuilder.Clear();
		_filelist.Clear();

		this->ConvertAnimationsRecursive(directory);
	}
}

void ConvertFbxToNax3App::ConvertSkinsRecursive( String skinDir )
{
	printf("Converting skins\n");
	Array<String> _filelist;
	Array<String> dirs = IoServer::Instance()->ListDirectories(skinDir, "*");
	for (int i = 0; i < dirs.Size(); i++)
	{
		String directory;
		directory.Format("%s/%s", skinDir.AsCharPtr(), dirs[i].AsCharPtr());
		// Skip .svn folders
		if(dirs[i] == ".svn")
		{
			continue;
		}
		else
		{
			_filelist.AppendArray(this->GetFileListFromDirectory(directory));
			int numFiles = _filelist.Size();
			ModelBuilder::Model newModel;
			if (numFiles > 0)
			{
				this->modelBuilder.AddModel(skinDir.ExtractFileName(), newModel);
			}
			// We found fbx files, convert them!
			for (int j = 0; j < numFiles; j++)
			{
				printf("processing: ");
				printf(_filelist[j].ExtractFileName().AsCharPtr());
				//ToolkitUtil::ModelBuilder::Model* m = this->modelBuilder.GetModelByName(skinDir.ExtractFileName());

				if(!reader->ReadSkin(URI(_filelist[j]).AsString().ExtractToEnd(8).AsCharPtr(), this->meshBuilder, &newModel))
				{
					printf(" - failed!");
					break;
				}
				else 
				{
					printf(" - success!");
					String fileName = skinDir.ExtractFileName();
					fileName.Append("/skins/");
					fileName.Append(_filelist[j].ExtractFileName());
					fileName.StripFileExtension();
					fileName.Append("_sk_0");
					skinDir.ExtractLastDirName();
					String path = this->BuildMeshDstPath(this->category, fileName);
					MeshBuilder dstMesh;
					this->PartitionMesh(this->meshBuilder, dstMesh, 72);
					bool res = MeshBuilderSaver::SaveNvx2(path, dstMesh, this->platform);

					// Extract partition info
					this->ExtractPartitioningData(newModel);

					this->groupMappingArray.Clear();
					this->partitionArray.Clear();
					this->meshBuilder.Clear();
				}
				printf("\n");
			}
			_filelist.Clear();
		}
		this->ConvertSkinsRecursive(directory);
	}
}

bool
ConvertFbxToNax3App::PartitionMesh(MeshBuilder& srcMesh, MeshBuilder& dstMesh, int maxJointPaletteSize)
{
    this->partitionArray.Clear();

    // for each triangle...
    int triIndex;
    int numTriangles = srcMesh.GetNumTriangles();
    for (triIndex = 0; triIndex < numTriangles; triIndex++)
    {
        const MeshBuilderTriangle& triangle = srcMesh.TriangleAt(triIndex);

        // try to add the triangle to an existing partition
        bool triangleAdded = false;
        int numPartitions = this->partitionArray.Size();
        int partitionIndex;
        for (partitionIndex = 0; partitionIndex < numPartitions; partitionIndex++)
        {
            if (this->partitionArray[partitionIndex].GetGroupId() == triangle.GetGroupId())
            {
                if (this->partitionArray[partitionIndex].CheckAddTriangle(triIndex))
                {
                    triangleAdded = true;
                    break;
                }
            }
        }
        if (!triangleAdded)
        {
            // triangle didn't fit into any existing partition, create a new partition
            Partition newPartition(&srcMesh, maxJointPaletteSize, triangle.GetGroupId());
            triangleAdded = newPartition.CheckAddTriangle(triIndex);
            n_assert(triangleAdded);
            this->partitionArray.Append(newPartition);
        }
    }

    // update the triangle group ids
    this->BuildDestMesh(srcMesh, dstMesh);

    return true;
}

//------------------------------------------------------------------------------
/**
    Private helper method which builds the actual destination mesh after
    partitioning has happened.
*/
void
ConvertFbxToNax3App::BuildDestMesh(MeshBuilder& srcMesh, MeshBuilder& dstMesh)
{
	dstMesh.Clear();
    //dstMesh.vertexArray.Clear();
    //dstMesh.triangleArray.Clear();

    int partitionIndex;
    int numPartitions = this->partitionArray.Size();
    for (partitionIndex = 0; partitionIndex < numPartitions; partitionIndex++)
    {
        const Partition& partition = this->partitionArray[partitionIndex];
        
        // record the original group id in the groupMapArray
        this->groupMappingArray.Append(partition.GetGroupId());

        // transfer the partition vertices and triangles
        int i;
        const Array<int>& triArray = partition.GetTriangleIndices();
        for (i = 0; i < triArray.Size(); i++)
        {
            MeshBuilderTriangle tri = srcMesh.TriangleAt(triArray[i]);

            // transfer the 3 vertices of the triangle
            int origVertexIndex[3];
            tri.GetVertexIndices(origVertexIndex[0], origVertexIndex[1], origVertexIndex[2]);
            int triPoint;
            int newVertexIndex[3];
            for (triPoint = 0; triPoint < 3; triPoint++)
            {
                const MeshBuilderVertex& v = srcMesh.VertexAt(origVertexIndex[triPoint]);

                // convert the global joint indices to partition-local joint indices
				const float4& globalJointIndices = v.GetComponent(MeshBuilderVertex::JIndicesIndex);
                float4 localJointIndices;
                localJointIndices.x() = float(partition.GlobalToLocalJointIndex(int(globalJointIndices.x())));
                localJointIndices.y() = float(partition.GlobalToLocalJointIndex(int(globalJointIndices.y())));
                localJointIndices.z() = float(partition.GlobalToLocalJointIndex(int(globalJointIndices.z())));
                localJointIndices.w() = float(partition.GlobalToLocalJointIndex(int(globalJointIndices.w())));

                // add the vertex to the destination mesh and correct the joint indices
                newVertexIndex[triPoint] = dstMesh.GetNumVertices();
                dstMesh.AddVertex(v);
				dstMesh.VertexAt(newVertexIndex[triPoint]).SetComponent(MeshBuilderVertex::JIndicesIndex, localJointIndices);
            }

            // update the triangle and add to the dest mesh's triangle array
            tri.SetGroupId(partitionIndex);
            tri.SetVertexIndices(newVertexIndex[0], newVertexIndex[1], newVertexIndex[2]);
            dstMesh.AddTriangle(tri);
        }
    }

    // finally, do a cleanup on the dest mesh to remove any redundant vertices
    dstMesh.Cleanup(0);
}

} // namespace Toolkit