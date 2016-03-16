#pragma once
//------------------------------------------------------------------------------
/**
    @class Importer:PhysicsController
    
    Handles physics-specific options and GUI
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QObject>
#include "ui_modelimporterwindow.h"
#include "io/uri.h"
#include "modelutil/modelphysics.h"


namespace ModelImporter
{
class PhysicsController : public QObject
{
	Q_OBJECT
public:
	/// constructor
	PhysicsController(Ui::ModelImporterWindow* ui);
	/// destructor
	~PhysicsController();

	/// sets the current import options
	void SetCurrentOptions(const Ptr<ToolkitUtil::ModelPhysics>& options);
	/// gets the current import options
	const Ptr<ToolkitUtil::ModelPhysics>& GetCurrentOptions() const;

	/// sets the merge option
	void SetMeshFlags(Physics::MeshTopologyType flags);
	/// sets the redundant mesh removal option
	void SetPhysicsFlags(ToolkitUtil::PhysicsExportMode flags);

	void SetHasPhysics(bool enable);

public slots:
	/// called when a new file has been read
	void ReaderLoaded(int status, const IO::URI& path);

	/// 
	void ToggleUseProvided(bool b);
	/// called when 
	void ToggleCreateBBox(bool b);
	/// called when 
	void ToggleUseGraphics(bool b);
	
	/// called when 
	void ToggleConcave(bool b);
	/// called when 
	void ToggleConvex(bool b);

	/// called when 
	void ToggleCreateConvex(bool b);
	///
	void ToggleStatic(bool b);

	///
	void ToggleBox( bool);
	///
	void ToggleSphere( bool);
	///
	void ToggleCapsule( bool);
	
private:

	/// sets controllers to be enabled/disabled
	void SetControllersEnabled(bool state);

	Ui::ModelImporterWindow* ui;
	Ptr<ToolkitUtil::ModelPhysics> currentOptions;
	bool hasPhysics;
};


//------------------------------------------------------------------------------
/**
*/
inline void 
PhysicsController::SetCurrentOptions( const Ptr<ToolkitUtil::ModelPhysics>& options )
{
	this->currentOptions = options;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ToolkitUtil::ModelPhysics>&
PhysicsController::GetCurrentOptions() const
{
	return this->currentOptions;
}

}