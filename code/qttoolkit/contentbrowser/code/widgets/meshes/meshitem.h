#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::MeshItem
    
    QTreeWidgetItem responsible for clicking mesh items
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/baseitem.h"
namespace Widgets
{
class MeshItem : public BaseItem
{
public:
	/// constructor
	MeshItem();
	/// destructor
	virtual ~MeshItem();

	/// clones model item
	BaseItem* Clone();

	/// sets up dock widget with mesh display information
	void OnActivated();
	/// handles events whenever the item is clicked
	void OnClicked();
	/// handles events whenever an item is right clicked
	void OnRightClicked(const QPoint& pos);
};


} // namespace Widgets
//------------------------------------------------------------------------------