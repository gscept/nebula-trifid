#pragma once
#include <QItemDelegate>
#include <QWidget>
#include <QStyleOptionViewItem>
#include "clip.h"
namespace Importer
{
class ClipController;
class ClipDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	/// constructor
	ClipDelegate(ClipController* controller);
	/// destructor
	~ClipDelegate();

	/// creates an editor
	QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	/// set initial data
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	/// sets the data when the editor is closed
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	/// updates the editor size
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	/// hints how big and wide the rows and columns should be
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	/// sets the current clip
	void SetCurrentClip(Clip* clip);
private:
	Clip* currentClip;
	ClipController* controller;
};
}