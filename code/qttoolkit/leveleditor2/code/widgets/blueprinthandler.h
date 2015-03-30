#pragma once
#include <QObject>
#include <QListWidget>
#include <QDialog>
#include <QDomDocument>
#include <QFile>
#include "ui_blueprints_dialog.h"
#include "entityhandler.h"

namespace LevelEditor2
{	
	class EntityDialog;

class BlueprintsHandler: public QDialog
{
	Q_OBJECT

public:
	BlueprintsHandler(QWidget* i_parent =0);
	~BlueprintsHandler(){}

	void SetupDialog();
	void SetupEntityDialog(QList<QString> i_lProperties);

	public slots:
		void NewEntity();
		void EditEntity();
		void DeleteEntity();
		/// Creates a duplicate of selected blueprint
		void DuplicateBlueprint();
		void BlueprintsChanged();
		

private:
	EntityHandler* m_pEntityDialog;

	Ui::BlueprintUI m_uiBlueprintsUI;

};
}