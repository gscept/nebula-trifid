//------------------------------------------------------------------------------
//  librarytreewidget.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "librarytreewidget.h"
#include "widgets/baseitem.h"

using namespace Widgets;
namespace ContentBrowser
{

//------------------------------------------------------------------------------
/**
*/
LibraryTreeWidget::LibraryTreeWidget(QWidget* parent) :
	QTreeWidget(parent)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
LibraryTreeWidget::~LibraryTreeWidget()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
QMimeData* 
LibraryTreeWidget::mimeData( const QList<QTreeWidgetItem*> items ) const
{
	// we always use single select, so the amount of items we get can be at most 1
	QTreeWidgetItem* item = items[0];

	// it has to be a base item
	BaseItem* baseItem = dynamic_cast<BaseItem*>(item);

	if (baseItem)
	{
		// create mimedata
		QMimeData* mimeData = new QMimeData;
		Util::String res = baseItem->GetName().toLatin1().constData();
		res.StripFileExtension();
		mimeData->setData("nebula/resourceid", res.AsCharPtr());
		return mimeData;
	}


	// if that fails, return 0
	return NULL;	
}

//------------------------------------------------------------------------------
/**
*/
QTreeWidgetItem* 
LibraryTreeWidget::FindItem( QTreeWidgetItem* topLevelItem, const QString& path )
{
    if (topLevelItem == NULL)   return topLevelItem;
    else                        return FindItemHelper(topLevelItem, path);
}

//------------------------------------------------------------------------------
/**
*/
QTreeWidgetItem* 
LibraryTreeWidget::FindItemHelper( QTreeWidgetItem* currentItem, const QString& fragment )
{
    QString piece = fragment.section("/", 0, 0);
    int slashIndex = fragment.indexOf("/");
    QTreeWidgetItem* nextItem = currentItem;

    IndexT i;
    for (i = 0; i < currentItem->childCount(); i++)
    {
        QTreeWidgetItem* child = currentItem->child(i);
        if (child->text(0) == piece) 
        {
            if (slashIndex == -1) return child;
            else
            {
                nextItem = FindItemHelper(child, fragment.section("/", 1));
                return nextItem; 
            }
        }
    }

    return NULL;    
}
} // namespace ContentBrowser