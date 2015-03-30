#pragma once
#include <QString>
#include <QList>
#include "take.h"

//------------------------------------------------------------------------------
/**
    @class Importer::ImportOptions
    
    Holds settings required to import a resource to Nebula
    
    (C) 2012 gscept
*/
namespace Importer
{
class ImportOptions
{


public:

	enum ImportMode
	{
		Static,
		Skeletal,

		NumImportModes
	};

	enum MeshFlag
	{
		None = 0,
		Merge = 1 << 0,
		RemoveRedudant = 1 << 1,

		NumMeshFlags = (1 << 2) - 1
	};

	/// constructor
	ImportOptions();
	/// destructor
	~ImportOptions();

	/// set the import mode
	void SetExportMode(const ImportMode& mode);
	/// gets the import mode
	const ImportMode& GetExportMode() const;

	/// set mesh flags (flags can be combined)
	void SetExportFlags(const MeshFlag& meshFlags);
	/// gets the mesh flags
	const MeshFlag& GetExportFlags() const;

	/// adds a take pointer to the list of takes
	void AddTake(Take* take);
	/// returns take for specific index
	Take* GetTake(uint index);
	/// returns reference to array of takes
	const QList<Take*>& GetTakes() const;

	/// sets the name of the model to which this import option will be related
	void SetName(const QString& name);
	/// returns the name of the associated model
	const QString& GetName() const;

private:
	friend class ImportDatabase;

	QString name;
	MeshFlag importFlags;
	ImportMode importMode;
	QList<Take*> takes;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
ImportOptions::SetExportMode( const ImportOptions::ImportMode& mode )
{
	this->importMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline const ImportOptions::ImportMode& 
ImportOptions::GetExportMode() const
{
	return this->importMode;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ImportOptions::SetExportFlags( const ImportOptions::MeshFlag& meshFlags )
{
	this->importFlags = meshFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline const ImportOptions::MeshFlag& 
ImportOptions::GetExportFlags() const
{
	return this->importFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ImportOptions::SetName( const QString& name )
{
	this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const QString& 
ImportOptions::GetName() const
{
	return this->name;
}

}