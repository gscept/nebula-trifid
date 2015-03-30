#pragma once
#include <QObject>
#include "ui_audiodialog.h"
#include "util/dictionary.h"
#include "util/string.h"
#include "io/uri.h"

namespace LevelEditor2
{	

class AudioDialogHandler : public QDialog
{
	Q_OBJECT

public:
    /// constructor
	AudioDialogHandler(QWidget* parent = 0);
    /// destructor
	~AudioDialogHandler();

    /// sets up the dialog
	void SetupDialog();
    

private slots:
    /// saves properties
	void SaveAudioProperties();
	/// clean up dialog
	void CloseDialog();
	/// will load a bank file from the list
	void LoadBank(int state);
private:
	Ui::AudioProperties ui;		
};
}