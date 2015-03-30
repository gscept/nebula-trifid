#pragma once
//------------------------------------------------------------------------------
/**
    @class Importer::ImportDatabase
    
    Holds dictionary of import options, is also responsible for loading and saving them
    
    (C) 2012 gscept
*/
#include <QMap>
#include <QDomDocument>
#include <QObject>
#include "importoptions.h"
namespace Importer
{
class ImportDatabase : public QObject
{
	Q_OBJECT
public:

	/// returns pointer to singleton instance
	static ImportDatabase* Instance();

	/// loads importer options from batch-attributes file
	void Load(const QString& file);
	/// saves importer options to batch-attributes file
	void Save(const QString& file);

	/// returns importer option for file
	ImportOptions* GetImportOptions(const QString& name);
	/// returns true if import options exist
	bool HasImportOptions(const QString& name);
	/// adds import options to import database
	void AddImportOptions(const QString& name, ImportOptions* options);

public slots:
	/// saves importer database
	void SaveToDefault();
	/// loads impoter database
	void LoadFromDefault();
private:
	/// constructor
	ImportDatabase();
	/// destructor
	~ImportDatabase();

	static ImportDatabase* instance;

	QMap<QString, ImportOptions*> importOptions;
};

//------------------------------------------------------------------------------
/**
*/
inline ImportOptions* 
ImportDatabase::GetImportOptions( const QString& name )
{
	return this->importOptions[name];
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
ImportDatabase::HasImportOptions( const QString& name )
{
	return this->importOptions.contains(name);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ImportDatabase::AddImportOptions( const QString& name, ImportOptions* options )
{
	this->importOptions[name] = options;
}
}