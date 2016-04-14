#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::LevelPropertiesHandler
    
    Qt dialong for setting levelproperties
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include <QObject>
#include <QDialog>
#include <QListWidget>
#include "ui_levelproperties.h"

//------------------------------------------------------------------------------
namespace LevelEditor2
{

class LevelPropertiesHandler : public QDialog
{
	Q_OBJECT

public:
	/// constructor
	LevelPropertiesHandler(QWidget *parent = 0);
	/// destructor
	~LevelPropertiesHandler();

	/// update valules from level singleton
	void UpdateValues();
	public slots:

	/// called when startup level
	void OnStartupChanged();


protected:
	Ui_levelprops ui;
};
}