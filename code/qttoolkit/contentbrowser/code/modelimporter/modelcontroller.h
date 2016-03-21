#pragma once
//------------------------------------------------------------------------------
/**
    @class Importer::ModelController
    
    Handles model-specific options and GUI
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QObject>
#include "ui_modelimporterwindow.h"
#include "io/uri.h"
#include "toolkitutil/modelutil/modelattributes.h"

namespace ModelImporter
{
class ModelController : public QObject
{
	Q_OBJECT
public:
	/// constructor
	ModelController(Ui::ModelImporterWindow* ui);
	/// destructor
	~ModelController();

	/// sets the current import options
	void SetCurrentOptions(const Ptr<ToolkitUtil::ModelAttributes>& options);
	/// gets the current import options
	const Ptr<ToolkitUtil::ModelAttributes>& GetCurrentOptions() const;

	/// sets the merge option
	void SetMerge(bool b);
	/// sets the redundant mesh removal option
	void SetRemoveRedundants(bool b);

public slots:
	/// called when a new file has been read
	void ReaderLoaded(int status, const IO::URI& path);

	/// called when the redudant vertices removal box is checked
	void ToggleRedundantRemoval(bool b);
	/// called when the calculate normal box is checked
	void ToggleCalcNormals(bool b);
	/// called when the calculate binormal and tangets box is checked
	void ToggleCalcBinormalsAndTangents(bool b);
	/// called when the flip uv box is checked
	void ToggleFlipUVs(bool b);
	/// called when the vertex color box is checked
	void ToggleVertexColors(bool b);
    /// called when the lightmapped radio button is checked
    void ToggleSecondaryUVs(bool b);

	/// called when the static radio button is checked
	void ToggleStaticExport(bool b);
	/// called when the skeletal radio button is checked
	void ToggleSkeletalExport(bool b);


	/// called when the scale has changed
	void OnScaleChanged(double d);
	/// called when a scale preset has been set
	void OnScalePresetSelected(int i);
private:

	/// sets controllers to be enabled/disabled
	void SetControllersEnabled(bool state);

	Ui::ModelImporterWindow* ui;
	Ptr<ToolkitUtil::ModelAttributes> currentOptions;
};


//------------------------------------------------------------------------------
/**
*/
inline void 
ModelController::SetCurrentOptions( const Ptr<ToolkitUtil::ModelAttributes>& options )
{
	this->currentOptions = options;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ToolkitUtil::ModelAttributes>&
ModelController::GetCurrentOptions() const
{
	return this->currentOptions;
}

}