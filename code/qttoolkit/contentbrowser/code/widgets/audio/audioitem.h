#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::AudioItem
    
    QTreeWidgetItem responsible for handling clicking and managing audio items
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/baseitem.h"
#include "audiohandler.h"
namespace Widgets
{
class AudioItem : public BaseItem
{
public:
	/// constructor
	AudioItem();
	/// destructor
	virtual ~AudioItem();

	/// clones model item
	BaseItem* Clone();

	/// sets up dock widget with audio display information
	void OnActivated();
	/// handles events whenever the item is clicked
	void OnClicked();
	/// handles events whenever the item irs right clicked
	void OnRightClicked(const QPoint& pos);


};



} // namespace Widgets
//------------------------------------------------------------------------------