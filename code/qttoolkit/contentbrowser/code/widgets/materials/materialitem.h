#pragma once
//------------------------------------------------------------------------------
/**
	@class Widgets::MaterialItem
	
	An item representing a material.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/baseitem.h"
#include "ui_materialeditorwidget.h"
namespace Widgets
{
class MaterialItem : public BaseItem
{
public:
	/// constructor
	MaterialItem();
	/// destructor
	virtual ~MaterialItem();

	/// clones model item
	BaseItem* Clone();

	/// sets the ui
	void SetUi(Ui::MaterialEditorWidget* ui);
	/// returns pointer to ui
	Ui::MaterialEditorWidget* GetUi() const;

	/// sets up dock widget with mesh display information
	void OnActivated();
	/// handles events whenever the item is clicked
	void OnClicked();
	/// handles events whenever an item is right clicked
	void OnRightClicked(const QPoint& pos);

private:
	Ui::MaterialEditorWidget* ui;
};

//------------------------------------------------------------------------------
/**
*/
void
MaterialItem::SetUi(Ui::MaterialEditorWidget* ui)
{
	this->ui = ui;
}

//------------------------------------------------------------------------------
/**
*/
Ui::MaterialEditorWidget*
MaterialItem::GetUi() const
{
	return this->ui;
}

} // namespace Widgets