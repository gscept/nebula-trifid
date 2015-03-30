#pragma once
#include <QObject>
#include <QItemSelection>
#include "ui_importerqt.h"
#include "clipmodel.h"
#include "clipdelegate.h"
#include "clip.h"
#include "fbx\fbxreader.h"
#include "importoptions.h"

namespace Importer
{

class ClipController : public QObject
{
	Q_OBJECT
public:
	/// constructor
	ClipController(Ui::ImporterQtClass* ui);
	/// destructor
	~ClipController();

	/// handles selection change (in case one decides to use tab)
	void HandleSelection(const QModelIndex& index);
	/// sets the fbx reader
	void SetReader(FbxReader* reader);

	/// emits the changed signal
	void EmitChanged();

private slots:
	/// adds a new clip
	void AddClip();
	/// removes the selected clip
	void RemoveClip();
	/// take selected callback
	void TakeSelected(QListWidgetItem* item);

public slots:
	/// sets the current clip
	void SetCurrentClip(const QModelIndex& index);
	/// reader has loaded
	void ReaderLoaded(int status, const QString& file, const QString& category);
	
signals:
	/// called whenever something changed
	void Changed(bool state);


private:

	/// refreshes the clip gui
	void Refresh();
	/// sets controllers to state
	void SetControllersEnabled(bool state);

	FbxReader* reader;
	Ui::ImporterQtClass* ui;
	ClipModel* model;
	ClipDelegate* delegate;
	Take* currentTake;
	ImportOptions* currentBatch;
	QList<Clip*> clips;
	Clip* currentClip;
};


//------------------------------------------------------------------------------
/**
*/

}