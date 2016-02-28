#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::ModelItem
    
    QTreeWidgetItem responsible for handling events sent to model items
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/baseitem.h"
#include "modelhandler.h"
#include "ui_modelinfowidget.h"
namespace Widgets
{
class ModelItem : public BaseItem
{
public:
	/// constructor
	ModelItem();
	/// destructor
	virtual ~ModelItem();

	/// clones model item
	BaseItem* Clone();

	/// sets the ui
	void SetUi(Ui::ModelInfoWidget* ui);
	/// returns pointer to ui
	Ui::ModelInfoWidget* GetUi() const;

	/// sets up dock widget with model display information
	void OnActivated();
	/// discards handler and all its contents
	bool OnDeactivated();
	/// handles events whenever the item is clicked
	void OnClicked();
	/// handles events whenever the item is right clicked
	void OnRightClicked(const QPoint& pos);

private:

	Ui::ModelInfoWidget* ui;
	
};


//------------------------------------------------------------------------------
/**
*/
inline void
ModelItem::SetUi(Ui::ModelInfoWidget* ui)
{
	this->ui = ui;
}

//------------------------------------------------------------------------------
/**
*/
inline Ui::ModelInfoWidget* 
ModelItem::GetUi() const
{
	return this->ui;
}

} // namespace Widgets
//------------------------------------------------------------------------------