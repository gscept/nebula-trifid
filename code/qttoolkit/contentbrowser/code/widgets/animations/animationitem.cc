//------------------------------------------------------------------------------
//  animationitem.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "animationitem.h"
#include "contentbrowserapp.h"

using namespace Util;
namespace Widgets
{

//------------------------------------------------------------------------------
/**
*/
AnimationItem::AnimationItem()
{
	this->type = AnimationItemType;
}

//------------------------------------------------------------------------------
/**
*/
AnimationItem::~AnimationItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationItem::OnActivated()
{
	BaseItem::OnActivated();
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationItem::OnClicked()
{
	BaseItem::OnClicked();
}

//------------------------------------------------------------------------------
/**
*/
void 
AnimationItem::OnRightClicked( const QPoint& pos )
{
	// create menu
	QMenu menu(this->treeWidget());

	// create actions
	QAction* action1 = new QAction("Delete animation", NULL);

	// get window
	ContentBrowser::ContentBrowserWindow* window = ContentBrowser::ContentBrowserApp::Instance()->GetWindow();

	// setup menu
	menu.addAction(action1);

	// execute menu
	QAction* action = menu.exec(this->treeWidget()->mapToGlobal(pos));

	// get file
	String file = this->text(0).toUtf8().constData();

	// get category
	String category = this->parent()->text(0).toUtf8().constData();

	// get pointer to io server
	const Ptr<IO::IoServer>& ioServer = IO::IoServer::Instance();

	if (action == action1)
	{
		String resource;
		resource.Format("ani:%s/%s.nax3", category.AsCharPtr(), file.AsCharPtr());

		// delete animation
		ioServer->DeleteFile(resource);

		// remove animation
		window->RemoveAnimationItem(this);
	}
}

//------------------------------------------------------------------------------
/**
*/
BaseItem* 
AnimationItem::Clone()
{
	AnimationItem* item = new AnimationItem;
	item->SetName(this->GetName());
	item->SetWidget(this->GetWidget());
	item->SetHandler(this->GetHandler());
	return item;
}

} // namespace Widgets