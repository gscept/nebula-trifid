#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::CreateEntityDialogHandler
    
    Qt dialong for the create different sorts of entities.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include <QObject>
#include <QDialog>
#include <QListWidget>
#include "ui_chooseentitydialog.h"
#include "entityguid.h"

//------------------------------------------------------------------------------
namespace LevelEditor2
{

class CreateEntityDialogHandler : public QDialog
{
	Q_OBJECT

public:
	/// constructor
	CreateEntityDialogHandler(QWidget *parent = 0);
	/// destructor
	~CreateEntityDialogHandler();

public slots:

	/// called when clicking Create -> Add Environment Entity in the menu
	void NewEnvironmentEntity();
	/// called when clicking Create -> Add Light Entity in the menu
	void NewLightEntity();
	/// called when clicking Create -> Add Game Entity in the menu
	void NewGameEntity();
	/// the category selection has changed
	void CategoryChosen(QListWidgetItem * item);
	/// the import-button has been clicked
	void ImportChosenEntity();

	/// called when right key is pressed
	void RightKeyPressed();
	/// called when left key is pressed
	void LeftKeyPressed();
	/// called when the placement option is changed
	void PlacementChanged(int);

private:

	EntityType entityType;
	Ui::ChooseEntityDialog ui;
};

} // namespace LevelEditor2