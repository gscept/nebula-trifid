//------------------------------------------------------------------------------
//  meshitem.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "meshitem.h"
#include "contentbrowserapp.h"

using namespace Util;
namespace Widgets
{
//------------------------------------------------------------------------------
/**
*/
MeshItem::MeshItem()
{
	// create handler
	this->itemHandler = MeshHandler::Create();
	this->type = MeshItemType;	
}

//------------------------------------------------------------------------------
/**
*/
MeshItem::~MeshItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
MeshItem::OnActivated()
{
	BaseItem::OnActivated();
}

//------------------------------------------------------------------------------
/**
*/
void
MeshItem::OnClicked()
{
	BaseItem::OnClicked();
}

//------------------------------------------------------------------------------
/**
*/
void 
MeshItem::OnRightClicked( const QPoint& pos )
{
	// create menu
	QMenu menu(this->treeWidget());

	// create actions
	QAction* action1 = new QAction("Delete mesh", NULL);

	// get window
	ContentBrowser::ContentBrowserWindow* window = ContentBrowser::ContentBrowserApp::Instance()->GetWindow();

	// setup menu
	menu.addAction(action1);

	// execute menu
	QAction* action = menu.exec(this->treeWidget()->mapToGlobal(pos));

	// get file
	String file = this->text(0).toUtf8().constData();
	file.StripFileExtension();

	// get category
	String category = this->parent()->text(0).toUtf8().constData();

	// get pointer to io server
	const Ptr<IO::IoServer>& ioServer = IO::IoServer::Instance();

	if (action == action1)
	{
		String resource;
		resource.Format("msh:%s/%s.nvx2", category.AsCharPtr(), file.AsCharPtr());

		// delete mesh
		ioServer->DeleteFile(resource);

		// remove mesh
		window->RemoveMeshItem(this);
	}
}

//------------------------------------------------------------------------------
/**
*/
BaseItem* 
MeshItem::Clone()
{
	MeshItem* item = new MeshItem;
	item->SetName(this->GetName());
	item->SetWidget(this->GetWidget());
	item->SetHandler(this->GetHandler());
	return item;
}



} // namespace Widgets