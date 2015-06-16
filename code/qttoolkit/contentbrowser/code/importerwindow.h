#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::ImporterWindow
    
    Base class for importer windows, implements basic stuff.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QDialog>
#include "ui_saveresourcedialog.h"
#include "util/string.h"
namespace ContentBrowser
{
class ImporterWindow : public QDialog
{
	Q_OBJECT
public:
	/// constructor
	ImporterWindow();
	/// destructor
	virtual ~ImporterWindow();

private slots:
	/// called when a new category should be created
	void OnNewCategory();

signals:
	/// called whenever an importer is done
	void ImportDone(const Util::String& name);

protected:
	/// sets up categories for move dialog
	void SetupCategories(const QString& category);

	Ui::SaveResourceDialog dialogUi;
	QDialog dialog;
	Util::String resourceFolder;
}; 
} // namespace ContentBrowser
//------------------------------------------------------------------------------