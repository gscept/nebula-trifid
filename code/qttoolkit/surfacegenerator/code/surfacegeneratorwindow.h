#pragma once
//------------------------------------------------------------------------------
/**
	Window used for the surface generator.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QWidget>
#include <QDialog>
#include "ui_makesurfacewidget.h"
#include "ui_surfacegeneratorwidget.h"
#include "n3util/n3modeldata.h"
namespace ToolkitUtil
{
class SurfaceGeneratorWindow : public QWidget
{
	Q_OBJECT
public:
	/// constructor
	SurfaceGeneratorWindow();
	/// destructor
	~SurfaceGeneratorWindow();

private slots:
	/// handle scan button getting clicked
	void OnScanClicked();
	/// handle the create button getting clicked
	void OnCreateClicked();
private:

	/// helper function to create surface file
	bool CreateSurface(const ToolkitUtil::State& state, const Util::String& name);

	Ui::MakeSurfaceWidget makeUi;
	Ui::SurfaceGeneratorWidget ui;
	QDialog makeDialog;
};
} // namespace ToolkitUtil