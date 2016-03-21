#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::BankItem
    
    QTreeWidgetItem responsible for handling clicking and managing audio bank files
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/baseitem.h"
#include "audiohandler.h"
namespace Widgets
{
class BankItem : public BaseItem
{
public:
	/// constructor
	BankItem();
	/// destructor
	virtual ~BankItem();

	/// clones model item
	BaseItem* Clone();

	/// sets up dock widget with audio display information
	void OnActivated();
	/// handles events whenever the item is clicked
	void OnClicked();
	/// handles events whenever the item irs right clicked
	void OnRightClicked(const QPoint& pos);
private:
	bool loaded;

};



} // namespace Widgets
//------------------------------------------------------------------------------