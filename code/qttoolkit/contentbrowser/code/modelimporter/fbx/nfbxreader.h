#pragma once
//------------------------------------------------------------------------------
/**
    @class Importer::FBXReader
    
    Reads FBX data and calls back
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "fbxsdk.h"
#include <QString>
#include <QObject>
#include "io/uri.h"
namespace ModelImporter
{
class NFbxReader : public QObject
{
	Q_OBJECT

public:

	enum LoadStatus
	{
		Success,
		Failed
	};

	/// constructor
	NFbxReader();
	/// destructor
	~NFbxReader();

	/// opens a scene
	void Open(const IO::URI& path);
	/// closes reader (and scene)
	void Close();
	/// returns true if reader is open
	bool IsOpen() const;
	/// sets the parent widget
	void SetParent(QWidget* parent);

	/// gets list of takes from loaded FBX
	const Util::Array<Util::String> GetTakes();
	/// gets file information from loaded FBX
	const Util::String GetInfo();

	/// get number of uv channels from file
	const SizeT GetNumUvs() const;
	
	/// returns true if there exists any vertex colors
	const bool HasVertexColors() const;
	/// returns true if there are any joints
	const bool HasJoints() const;
	/// returns true if there is a physics node
	const bool HasPhysics() const;

	/// gets the scene pointer
	FbxScene* GetScene();
signals:
	/// emitted when scene is loaded
	void Loaded(int status, const IO::URI& path);
private:
	/// gets a list of joints from root node
	void GetJointsRecursive(Util::Array<Util::String>& joints, FbxNode* parent);

	bool hasVertexColors;
	SizeT numUvs;
	bool isOpen;
	IO::URI path;
	QWidget* parentWidget;
	FbxManager* sdkManager;
	FbxIOSettings* ioSettings;
	FbxScene* scene;
};


//------------------------------------------------------------------------------
/**
*/
inline const bool 
NFbxReader::HasJoints() const
{
	return this->scene->GetSrcObjectCount<FbxSkeleton>() > 0;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
NFbxReader::HasVertexColors() const
{
	return this->hasVertexColors;
}

//------------------------------------------------------------------------------
/**
*/
inline const SizeT 
NFbxReader::GetNumUvs() const
{
	return this->numUvs;
}

}