#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::FontItem
    
    QTreeWidgetItem responsible for handling clicking and managing font items
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/baseitem.h"
#include "uihandler.h"
namespace Widgets
{
class FontItem : public BaseItem
{
public:
	/// constructor
	FontItem();
	/// destructor
	virtual ~FontItem();

	/// clones model item
	BaseItem* Clone();

	/// handles events whenever the item is clicked
	void OnClicked();
	/// handles events whenever the item is right clicked
	void OnRightClicked(const QPoint& pos);
};


} // namespace Widgets
//------------------------------------------------------------------------------