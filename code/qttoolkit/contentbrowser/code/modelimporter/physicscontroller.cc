//------------------------------------------------------------------------------
//  modelcontroller.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physicscontroller.h"
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
PhysicsController::PhysicsController( Ui::ModelImporterWindow* ui ) : 
	currentOptions(0),hasPhysics(false)
{
	this->ui = ui;
	this->SetControllersEnabled(false);

	connect(this->ui->phUseProvidedPh, SIGNAL(toggled(bool)), this, SLOT(ToggleUseProvided(bool)));
	connect(this->ui->phCreateBBox, SIGNAL(toggled(bool)), this, SLOT(ToggleCreateBBox(bool)));
	connect(this->ui->phUseGraphics, SIGNAL(toggled(bool)), this, SLOT(ToggleUseGraphics(bool)));
	connect(this->ui->phUseConcave, SIGNAL(toggled(bool)), this, SLOT(ToggleConcave(bool)));

	connect(this->ui->phUseConvex, SIGNAL(toggled(bool)), this, SLOT(ToggleConvex(bool)));
	connect(this->ui->phSplit, SIGNAL(toggled(bool)), this, SLOT(ToggleHACD(bool)));

	connect(this->ui->phCreateConvex, SIGNAL(toggled(bool)), this, SLOT(ToggleCreateConvex(bool)));
	connect(this->ui->phUseStatic, SIGNAL(toggled(bool)),this,SLOT(ToggleStatic(bool)));

	
}

//------------------------------------------------------------------------------
/**
*/
PhysicsController::~PhysicsController()
{
	// empty
}

//------------------------------------------------------------------------------
/**
	Put all controllers here.
*/
void 
PhysicsController::SetControllersEnabled( bool state )
{

	this->ui->phUseProvidedPh->setEnabled(state & this->hasPhysics);
	this->ui->phCreateBBox->setEnabled(state);
	this->ui->phUseGraphics->setEnabled(state);
	this->ui->phUseConvex->setEnabled(state);
	this->ui->phUseConcave->setEnabled(state);
	this->ui->phSplit->setEnabled(state);
	this->ui->phCreateConvex->setEnabled(state);		
	this->ui->phUseStatic->setEnabled(state);	
}

//------------------------------------------------------------------------------
/**
*/
void 
PhysicsController::ReaderLoaded( int status, const IO::URI& path )
{
	if (status == NFbxReader::Success)
	{
		this->SetControllersEnabled(true);
		String fileExt = path.GetHostAndLocalPath().ExtractFileName().AsCharPtr();
		fileExt.StripFileExtension();
		String file = fileExt.AsCharPtr();
		String category = path.GetHostAndLocalPath().ExtractLastDirName().AsCharPtr();
		String model = category + "/" + file;

		this->currentOptions = ModelDatabase::Instance()->LookupPhysics(model);
		ToolkitUtil::PhysicsExportMode exportMode = this->currentOptions->GetExportMode();
		Physics::MeshTopologyType meshMode = this->currentOptions->GetMeshMode();
		

		if (exportMode == ToolkitUtil::UsePhysics)
		{
			this->ui->phUseProvidedPh->setChecked(true);
			this->ui->phUseConcave->setEnabled(true);
			this->ui->phUseConvex->setEnabled(true);
			this->ui->phCreateConvex->setEnabled(true);
			this->ui->phSplit->setEnabled(true);
			this->ui->phUseStatic->setEnabled(true);	
		}
		else if (exportMode == ToolkitUtil::UseBoundingBox)
		{
			this->ui->phCreateBBox->setChecked(true);

			this->ui->phUseConcave->setEnabled(false);
			this->ui->phUseConvex->setEnabled(false);
			this->ui->phCreateConvex->setEnabled(false);
			this->ui->phSplit->setEnabled(false);
			this->ui->phUseStatic->setEnabled(false);	

		}
		else if (exportMode == ToolkitUtil::UseGraphicsMesh)
		{
			this->ui->phUseGraphics->setChecked(true);
			this->ui->phUseConcave->setEnabled(true);
			this->ui->phUseConvex->setEnabled(true);
			this->ui->phCreateConvex->setEnabled(true);
			this->ui->phSplit->setEnabled(false);
			this->ui->phUseStatic->setEnabled(true);	
		}


		if (meshMode == Physics::MeshConcave)
		{
			this->ui->phUseConcave->setChecked(true);
		}
		else if (meshMode == Physics::MeshConvex)
		{
			this->ui->phUseConvex->setChecked(true);
		}
		else if (meshMode == Physics::MeshConvexHull)
		{
			this->ui->phCreateConvex->setChecked(true);
		}
		else if (meshMode == Physics::MeshConvexDecomposition)
		{
			this->ui->phSplit->setChecked(true);
		}
		else if (meshMode == Physics::MeshStatic)
		{
			this->ui->phUseStatic->setChecked(true);
		}				
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PhysicsController::ToggleUseProvided( bool b )
{
	this->currentOptions->SetExportMode(UsePhysics);
	this->ui->phUseConcave->setEnabled(b);
	this->ui->phUseConvex->setEnabled(b);
	this->ui->phCreateConvex->setEnabled(b);
	this->ui->phSplit->setEnabled(b);
	this->ui->phUseStatic->setEnabled(b);
}

void 
PhysicsController::ToggleCreateBBox( bool b )
{
	this->currentOptions->SetExportMode(UseBoundingBox);
	this->ui->phUseConcave->setEnabled(!b);
	this->ui->phUseConvex->setEnabled(!b);
	this->ui->phCreateConvex->setEnabled(!b);
	this->ui->phSplit->setEnabled(!b);
	this->ui->phUseStatic->setEnabled(!b);
}

void 
PhysicsController::ToggleUseGraphics( bool b )
{
	this->currentOptions->SetExportMode(UseGraphicsMesh);
	this->ui->phUseConcave->setEnabled(b);
	this->ui->phUseConvex->setEnabled(b);
	this->ui->phCreateConvex->setEnabled(b);
	this->ui->phSplit->setEnabled(!b);
	this->ui->phUseStatic->setEnabled(b);
}

void 
PhysicsController::ToggleConcave( bool b )
{
	this->currentOptions->SetMeshMode(Physics::MeshConcave);	
}

void 
PhysicsController::ToggleConvex( bool b )
{
	this->currentOptions->SetMeshMode(Physics::MeshConvex);	
}

void 
PhysicsController::ToggleCreateConvex( bool b )
{
	this->currentOptions->SetMeshMode(Physics::MeshConvexHull);	
}

void 
PhysicsController::ToggleHACD( bool b )
{
	this->currentOptions->SetMeshMode(Physics::MeshConvexDecomposition);	
}

void 
PhysicsController::ToggleStatic( bool b )
{
	this->currentOptions->SetMeshMode(Physics::MeshStatic);	
}

void 
PhysicsController::SetHasPhysics(bool enable)
{
	this->hasPhysics = enable;	
	this->ui->phUseProvidedPh->setEnabled(enable);
	if(enable)
	{		
		this->ToggleUseProvided(true);
	}
}

}