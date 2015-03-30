#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::TrackDelegate
    
    Implements a table view delegate.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QItemDelegate>
namespace Widgets
{
class TrackController;
class TrackDelegate : public QItemDelegate
{
public:
	/// constructor
	TrackDelegate(TrackController* controller);
	/// destructor
	virtual ~TrackDelegate();

	/// creates an editor
	QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	/// handle event
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);
	/// set initial data
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	/// sets the data when the editor is closed
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	/// hints how big and wide the rows and columns should be
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
	TrackController* controller;
}; 
} // namespace Widgets
//------------------------------------------------------------------------------