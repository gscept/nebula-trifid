#pragma once
//------------------------------------------------------------------------------
/**
	Implements a table delegate for rows handling passes.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QStyledItemDelegate>
namespace Shady
{
class MaterialWindow;
class PassRowDelegate : public QStyledItemDelegate
{
public:
	/// constructor
	PassRowDelegate();
	/// destructor
	virtual ~PassRowDelegate();

	/// apply data when editing has finished
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	/// setup editor when we start editing
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	/// create widget
	QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;


private:
	/// handle browsing a frame shader pass
	void OnBrowseFrameShaders(const QModelIndex& index) const;
	
	friend class MaterialWindow;
	MaterialWindow* window;
};
} // namespace Shady