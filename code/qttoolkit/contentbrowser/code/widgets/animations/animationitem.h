#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::AnimationItem
    
    QTreeWidgetItem responsible for handling click events for animation objects
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/baseitem.h"
#include "animationhandler.h"
namespace Widgets
{
class AnimationItem : public BaseItem
{
public:
	/// constructor
	AnimationItem();
	/// destructor
	virtual ~AnimationItem();

	/// clones model item
	BaseItem* Clone();

	/// sets up dock widget with animation display information
	void OnActivated();
	/// handles events whenever the item is clicked
	void OnClicked();
	/// handles events whenever the item is right clicked
	void OnRightClicked(const QPoint& pos);

};


} // namespace Widgets
//------------------------------------------------------------------------------