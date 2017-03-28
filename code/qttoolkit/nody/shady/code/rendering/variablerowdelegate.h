#pragma once
//------------------------------------------------------------------------------
/**
	Implements a row delegate for editing variables in the material editor.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QStyledItemDelegate>
namespace Shady
{
class MaterialWindow;
class VariableRowDelegate : public QStyledItemDelegate
{
public:
	/// constructor
	VariableRowDelegate();
	/// destructor
	virtual ~VariableRowDelegate();

	/// apply data when editing has finished
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	/// setup editor when we start editing
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	/// create widget
	QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
	friend class MaterialWindow;
	MaterialWindow* window;
};
} // namespace Shady