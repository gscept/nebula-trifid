#pragma once
//------------------------------------------------------------------------------
/**
    @class Importer::ModelController
    
    Handles model-specific options and GUI
    
    (C) 2012 gscept
*/
#include <QObject>
#include "importoptions.h"
#include "ui_importerqt.h"

namespace Importer
{
class ModelController : public QObject
{
	Q_OBJECT
public:
	/// constructor
	ModelController(Ui::ImporterQtClass* ui);
	/// destructor
	~ModelController();

	/// sets the current import options
	void SetCurrentOptions(ImportOptions* options);
	/// gets the current import options
	ImportOptions* GetCurrentOptions() const;

public slots:
	/// called when a new file has been read
	void ReaderLoaded(int status, const QString& file, const QString& category);
private:

	/// sets controllers to be enabled/disabled
	void SetControllersEnabled(bool state);

	Ui::ImporterQtClass* ui;
	ImportOptions* currentOptions;
};


//------------------------------------------------------------------------------
/**
*/
inline void 
ModelController::SetCurrentOptions( ImportOptions* options )
{
	this->currentOptions = options;
}

//------------------------------------------------------------------------------
/**
*/
inline ImportOptions* 
ModelController::GetCurrentOptions() const
{
	return this->currentOptions;
}

}