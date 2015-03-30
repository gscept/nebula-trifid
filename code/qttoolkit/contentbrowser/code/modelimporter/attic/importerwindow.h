#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::ImporterWindow
    
    Handles importer callbacks
    
    (C) 2012 gscept
*/

//------------------------------------------------------------------------------
#include <QtGui/QDialog>
#include "toolkit\toolkitutil\fbx\fbxexporter.h"
#include "ui_importerwindow.h"

namespace ContentBrowser
{
class ImporterWindow : public QMainWindow
{
	Q_OBJECT
public:
	/// constructor
	ImporterWindow();
	/// destructor
	virtual ~ImporterWindow();

private:
	Ui::ImporterWindow ui;
	Ptr<ToolkitUtil::FBXExporter> fbxExporter; 
}; 
} // namespace ContentBrowser
//------------------------------------------------------------------------------