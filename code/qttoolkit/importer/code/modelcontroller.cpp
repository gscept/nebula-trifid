#include "modelcontroller.h"
#include "importdatabase.h"
#include "fbx\fbxreader.h"

namespace Importer
{


//------------------------------------------------------------------------------
/**
*/
ModelController::ModelController( Ui::ImporterQtClass* ui ) : 
	currentOptions(0)
{
	this->ui = ui;
	this->SetControllersEnabled(false);
}

//------------------------------------------------------------------------------
/**
*/

ModelController::~ModelController()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelController::SetControllersEnabled( bool state )
{
	ui->staticImport->setEnabled(state);
	ui->skeletalImport->setEnabled(state);
	ui->merge->setEnabled(state);
	ui->removeRedudant->setEnabled(state);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelController::ReaderLoaded( int status, const QString& file, const QString& category )
{
	if (status == FbxReader::Success)
	{
		this->SetControllersEnabled(true);
		QString model = category + "/" + file;
		if (ImportDatabase::Instance()->HasImportOptions(model))
		{
			ImportOptions* options = ImportDatabase::Instance()->GetImportOptions(model);
			ImportOptions::MeshFlag meshFlag = options->GetExportFlags();
			ImportOptions::ImportMode mode = options->GetExportMode();
			if (mode == ImportOptions::Static)
			{
				this->ui->staticImport->setChecked(true);
			}
			else if (mode == ImportOptions::Skeletal)
			{
				this->ui->skeletalImport->setChecked(true);
			}

			if (meshFlag & ImportOptions::Merge)
			{
				this->ui->merge->setChecked(true);
			}
			else
			{
				this->ui->merge->setChecked(false);
			}

			if (meshFlag & ImportOptions::RemoveRedudant)
			{
				this->ui->removeRedudant->setChecked(true);
			}
			else
			{
				this->ui->removeRedudant->setChecked(false);
			}

			this->currentOptions = options;
		}
		else
		{
			ImportOptions* options = new ImportOptions;
			ImportDatabase::Instance()->AddImportOptions(model, options);

			this->ui->staticImport->setChecked(true);
			this->ui->merge->setChecked(false);
			this->ui->removeRedudant->setChecked(false);

			this->currentOptions = options;
		}
	}
}
}