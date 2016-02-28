#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::layoutItem
    
    QTreeWidgetItem responsible for handling clicking and managing layout items
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/baseitem.h"
#include "uihandler.h"
namespace Widgets
{
class LayoutItem : public BaseItem
{
public:
	/// constructor
	LayoutItem();
	/// destructor
	virtual ~LayoutItem();

	/// clones model item
	BaseItem* Clone();

	/// sets the ui
	void SetUi(Ui::UIInfoWidget* ui);
	/// returns pointer to ui
	Ui::UIInfoWidget* GetUi() const;

	/// sets up dock widget with layout information
	void OnActivated();
	/// handles events whenever the item is clicked
	void OnClicked();
	/// handles events whenever the item is right clicked
	void OnRightClicked(const QPoint& pos);
private:
	Ui::UIInfoWidget * ui;
};

//------------------------------------------------------------------------------
/**
*/
inline void
LayoutItem::SetUi(Ui::UIInfoWidget* ui)
{
	this->ui = ui;
}

//------------------------------------------------------------------------------
/**
*/
inline Ui::UIInfoWidget*
LayoutItem::GetUi() const
{
	return this->ui;
}

} // namespace Widgets
//------------------------------------------------------------------------------