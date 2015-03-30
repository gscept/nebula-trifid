#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::LibraryTreeWidget
    
    Overrides the library tree with special features for drag and drop
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QTreeWidget>
#include <QMimeData>
namespace ContentBrowser
{
class LibraryTreeWidget : public QTreeWidget
{
public:
	/// constructor
	LibraryTreeWidget(QWidget* parent);
	/// destructor
	virtual ~LibraryTreeWidget();

	/// start dragging
	QMimeData* mimeData(const QList<QTreeWidgetItem*> items) const;

    /// searches for an item recursively using a path with levels separated by '/' starting at topLevelItem, returns 0 if item is not found
    QTreeWidgetItem* FindItem(QTreeWidgetItem* topLevelItem, const QString& path);

private:

    /// internal find item helper
    QTreeWidgetItem* FindItemHelper(QTreeWidgetItem* currentItem, const QString& fragment);
}; 
} // namespace ContentBrowser
//------------------------------------------------------------------------------