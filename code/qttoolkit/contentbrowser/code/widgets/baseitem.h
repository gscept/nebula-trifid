#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::BaseItem
    
    Base item class for special QTreeWidgetItems
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QTreeWidgetItem>
#include <QWidget>
#include <QAction>
#include <QMenu>
#include "core/debug.h"
#include "util/string.h"
#include "io/ioserver.h"
#include "basehandler.h"

namespace Widgets
{
class BaseItem : public QTreeWidgetItem
{
public:

	enum ItemType
	{
		AnimationItemType,
		AudioItemType,
		AudioBankItemType,
		MeshItemType,
		ModelItemType,
		TextureItemType,
		LayoutItemType,
		FontItemType
	};

	/// constructor
	BaseItem();
	/// destructor
	virtual ~BaseItem();

	/// discard item
	virtual void Discard();

	/// overrides clone
	virtual BaseItem* Clone();

	/// set the dock widget to where the item should present its data
	void SetWidget(QWidget* widget);
	/// get pointer to dock widget
	QWidget* GetWidget() const;
	/// handles activate events (double-clicks)
	virtual void OnActivated();
	/// handles deactivate events (if this was clicked and some other item was clicked after)
	virtual bool OnDeactivated();
	/// handles click events (single-clicks)
	virtual void OnClicked();
	/// handles context menu events (right-clicks)
	virtual void OnRightClicked(const QPoint& pos);

	/// sets name of model item
	void SetName(const QString& name);
	/// gets name of model item
	const QString& GetName() const;

	/// gets the item type
	const ItemType& GetType() const;

	/// returns handler
	const Ptr<BaseHandler>& GetHandler() const;
	/// set pointer to handler
	void SetHandler(const Ptr<BaseHandler>& handler);

protected:
	ItemType type;
	Ptr<BaseHandler> itemHandler;
	QWidget* frame;
	QString name;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<BaseHandler>& 
BaseItem::GetHandler() const
{
	return this->itemHandler;
}


//------------------------------------------------------------------------------
/**
*/
inline void 
BaseItem::SetHandler( const Ptr<BaseHandler>& handler )
{
	this->itemHandler = handler;
}


//------------------------------------------------------------------------------
/**
*/
inline void
BaseItem::SetWidget( QWidget* widget )
{
	n_assert(widget);
	this->frame = widget;
}


//------------------------------------------------------------------------------
/**
*/
inline QWidget* 
BaseItem::GetWidget() const
{
	return this->frame;
}

//------------------------------------------------------------------------------
/**
*/
inline const BaseItem::ItemType& 
BaseItem::GetType() const
{
	return this->type;
}


//------------------------------------------------------------------------------
/**
*/
inline void 
BaseItem::SetName( const QString& name )
{
	n_assert(!name.isEmpty());
	this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const QString& 
BaseItem::GetName() const
{
	return this->name;
}

} // namespace Widgets
//------------------------------------------------------------------------------