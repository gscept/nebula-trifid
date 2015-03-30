#pragma once
#include <QObject>
#include "ui_uidialog.h"
#include "util/dictionary.h"
#include "util/string.h"
#include "io/uri.h"

namespace LevelEditor2
{	

class UIDialogHandler : public QDialog
{
	Q_OBJECT

public:
    /// constructor
	UIDialogHandler(QWidget* parent = 0);
    /// destructor
	~UIDialogHandler();

    /// sets up the dialog
	void SetupDialog();
    

private slots:
    /// saves properties
	void SaveUIProperties();
	/// clean up dialog
	void CloseDialog();	
private:
	Ui::UIProperties ui;		
};
}