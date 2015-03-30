#pragma once
#include <QObject>
#include <QListWidget>
#include <QDialog>
#include <QDomDocument>
#include <QFile>
#include "ui_entity_dialog.h"
#include "entityhandler.h"

namespace LevelEditor2
{	

class EntityHandler : public QDialog
{
	Q_OBJECT

public:
    /// constructor
	EntityHandler(QWidget* parent =0);
    /// destructor
	~EntityHandler();

    /// sets up the dialog which modifies blueprints
	void SetupDialog();
    /// set which blueprint should be modified
	void SetSelectedEntity(QString i_sEntityName);

public slots:
    /// attaches a property to the current blueprint
	void Attach();
    /// detaches a property from the current blueprint
	void Detach();
    /// moves selected property up a level
    void Up();
    /// moves a selected property down a level
    void Down();

    /// saves blueprint
	void SaveEntity();

signals:
    /// signaled when a blueprint has changed
	void BlueprintsChanged();



private:
	Ui::EntityUI ui;
	QString selectedProperty;	
};
}