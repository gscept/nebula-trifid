//------------------------------------------------------------------------------
//  modelcontroller.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "modelcontroller.h"
#include "toolkitutil/modelutil/modeldatabase.h"
#include "fbx/nfbxreader.h"

#define MATERIALTAB 4

using namespace ToolkitUtil;
using namespace Util;
namespace ModelImporter
{

//------------------------------------------------------------------------------
/**
*/
ModelController::ModelController( Ui::ModelImporterWindow* ui ) : 
	currentOptions(0)
{
	this->ui = ui;
	this->SetControllersEnabled(false);

	connect(this->ui->removeRedundant, SIGNAL(toggled(bool)), this, SLOT(ToggleRedundantRemoval(bool)));
	connect(this->ui->calcNormals, SIGNAL(toggled(bool)), this, SLOT(ToggleCalcNormals(bool)));
	connect(this->ui->calcBinormAndTangents, SIGNAL(toggled(bool)), this, SLOT(ToggleCalcBinormalsAndTangents(bool)));
	connect(this->ui->flipUVs, SIGNAL(toggled(bool)), this, SLOT(ToggleFlipUVs(bool)));
	connect(this->ui->vertexColors, SIGNAL(toggled(bool)), this, SLOT(ToggleVertexColors(bool)));
    connect(this->ui->secondaryUVs, SIGNAL(toggled(bool)), this, SLOT(ToggleSecondaryUVs(bool)));

	connect(this->ui->staticImport, SIGNAL(toggled(bool)), this, SLOT(ToggleStaticExport(bool)));
	connect(this->ui->skeletalImport, SIGNAL(toggled(bool)), this, SLOT(ToggleSkeletalExport(bool)));
	
	connect(this->ui->scaleBox, SIGNAL(valueChanged(double)), this, SLOT(OnScaleChanged(double)));
	connect(this->ui->scalePresets, SIGNAL(activated(int)), this, SLOT(OnScalePresetSelected(int)));
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
	Put all controllers here.
*/
void 
ModelController::SetControllersEnabled( bool state )
{
	this->ui->staticImport->setEnabled(state);
	this->ui->skeletalImport->setEnabled(state);
	this->ui->removeRedundant->setEnabled(state);
	this->ui->calcNormals->setEnabled(state);
	this->ui->calcBinormAndTangents->setEnabled(state);
	this->ui->flipUVs->setEnabled(state);
	this->ui->vertexColors->setEnabled(state);
    this->ui->secondaryUVs->setEnabled(state);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelController::ReaderLoaded( int status, const IO::URI& path )
{
	if (status == NFbxReader::Success)
	{
		this->SetControllersEnabled(true);
		String fileExt = path.GetHostAndLocalPath().ExtractFileName().AsCharPtr();
		fileExt.StripFileExtension();
		String file = fileExt.AsCharPtr();
		String category = path.GetHostAndLocalPath().ExtractLastDirName().AsCharPtr();
		String model = category + "/" + file;

		this->currentOptions = ModelDatabase::Instance()->LookupAttributes(model);
		ToolkitUtil::ExportFlags exportFlags = this->currentOptions->GetExportFlags();
		ToolkitUtil::ExportMode exportMode = this->currentOptions->GetExportMode();
		float scale = this->currentOptions->GetScale();

		// disable signals
		this->blockSignals(true);

		// set scale
		this->ui->scaleBox->setValue(scale);

		// set export mode
		if (exportMode == ToolkitUtil::Static)
		{
			this->ui->staticImport->setChecked(true);
            this->currentOptions->SetExportMode(ToolkitUtil::Static);
		}
		else if (exportMode == ToolkitUtil::Skeletal)
		{
			this->ui->skeletalImport->setChecked(true);
            this->currentOptions->SetExportMode(ToolkitUtil::Skeletal);
		}

		// set options
		if (exportFlags & ToolkitUtil::FlipUVs)
		{
			this->ui->flipUVs->setChecked(true);
		}
		else
		{
			this->ui->flipUVs->setChecked(false);
		}

		if (exportFlags & ToolkitUtil::ImportColors)
		{
			this->ui->vertexColors->setChecked(true);
		}
		else
		{
			this->ui->vertexColors->setChecked(false);
		}

        if (exportFlags & ToolkitUtil::ImportSecondaryUVs)
        {
            this->ui->secondaryUVs->setChecked(true);
        }
        else
        {
            this->ui->secondaryUVs->setChecked(false);
        }

		if (exportFlags & ToolkitUtil::RemoveRedundant)
		{
			this->ui->removeRedundant->setChecked(true);
		}
		else
		{
			this->ui->removeRedundant->setChecked(false);
		}

		if (exportFlags & ToolkitUtil::CalcNormals)
		{
			this->ui->calcNormals->setChecked(true);
		}
		else
		{
			this->ui->calcNormals->setChecked(false);
		}

		if (exportFlags & ToolkitUtil::CalcBinormalsAndTangents)
		{
			this->ui->calcBinormAndTangents->setChecked(true);
		}
		else
		{
			this->ui->calcBinormAndTangents->setChecked(false);
		}

		// reenable signals
		this->blockSignals(false);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelController::ToggleRedundantRemoval( bool b )
{
	uint flags = (uint)this->currentOptions->GetExportFlags();
	if (b)
	{
		flags |= ToolkitUtil::RemoveRedundant;
	}
	else
	{
		flags &= ~ToolkitUtil::RemoveRedundant;
	}
	this->currentOptions->SetExportFlags((ToolkitUtil::ExportFlags)flags);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelController::ToggleCalcNormals( bool b )
{
	uint flags = (uint)this->currentOptions->GetExportFlags();
	if (b)
	{
		flags |= ToolkitUtil::CalcNormals;
	}
	else
	{
		flags &= ~ToolkitUtil::CalcNormals;
	}
	this->currentOptions->SetExportFlags((ToolkitUtil::ExportFlags)flags);
}

//------------------------------------------------------------------------------
/**
*/
void
ModelController::ToggleCalcBinormalsAndTangents(bool b)
{
	uint flags = (uint)this->currentOptions->GetExportFlags();
	if (b)
	{
		flags |= ToolkitUtil::CalcBinormalsAndTangents;
	}
	else
	{
		flags &= ~ToolkitUtil::CalcBinormalsAndTangents;
	}
	this->currentOptions->SetExportFlags((ToolkitUtil::ExportFlags)flags);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelController::ToggleFlipUVs( bool b )
{
	uint flags = (uint)this->currentOptions->GetExportFlags();
	if (b)
	{
		flags |= ToolkitUtil::FlipUVs;
	}
	else
	{
		flags &= ~ToolkitUtil::FlipUVs;
	}
	this->currentOptions->SetExportFlags((ToolkitUtil::ExportFlags)flags);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelController::ToggleVertexColors( bool b )
{
	uint flags = (uint)this->currentOptions->GetExportFlags();
	if (b)
	{
		flags |= ToolkitUtil::ImportColors;
	}
	else
	{
		flags &= ~ToolkitUtil::ImportColors;
	}
	this->currentOptions->SetExportFlags((ToolkitUtil::ExportFlags)flags);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelController::ToggleSecondaryUVs( bool b )
{
    uint flags = (uint)this->currentOptions->GetExportFlags();
    if (b)
    {
        flags |= ToolkitUtil::ImportSecondaryUVs;
    }
    else
    {
        flags &= ~ToolkitUtil::ImportSecondaryUVs;
    }
    this->currentOptions->SetExportFlags((ToolkitUtil::ExportFlags)flags);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelController::ToggleStaticExport( bool b )
{
	if (b)
	{
		this->currentOptions->SetExportMode(ToolkitUtil::Static);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelController::ToggleSkeletalExport( bool b )
{
	if (b)
	{
		this->currentOptions->SetExportMode(ToolkitUtil::Skeletal);
	}
}


//------------------------------------------------------------------------------
/**
*/
void 
ModelController::OnScaleChanged( double d )
{
	this->currentOptions->SetScale(d);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelController::OnScalePresetSelected( int i )
{
	this->ui->scaleBox->setValue(pow(10.0f, i) * 0.001f); 
}


}