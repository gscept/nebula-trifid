//------------------------------------------------------------------------------
//  audioitem.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "audioitem.h"
#include "contentbrowserapp.h"

using namespace Util;
namespace Widgets
{
//------------------------------------------------------------------------------
/**
*/
AudioItem::AudioItem()
{
	this->type = AudioItemType;
}

//------------------------------------------------------------------------------
/**
*/
AudioItem::~AudioItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
AudioItem::OnActivated()
{
	BaseItem::OnActivated();
	// cast base handler to model handler
	Ptr<AudioHandler> handler = this->itemHandler.downcast<AudioHandler>();	

	// resolve path
	QString file = this->text(0);
	QTreeWidgetItem* parentItem = this->parent();
	QString category = parentItem->text(0);

	handler->SetName(file.toUtf8().constData());	

	// tell handler to load audio info
	handler->LoadAudioInfo();	
}

//------------------------------------------------------------------------------
/**
*/
void
AudioItem::OnClicked()
{
	BaseItem::OnClicked();
}

//------------------------------------------------------------------------------
/**
*/
void 
AudioItem::OnRightClicked( const QPoint& pos )
{
	// create menu
	QMenu menu(this->treeWidget());

	// create actions
	QAction* action1 = new QAction("Delete audio", NULL);

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
		resource.Format("audio:%s/%s", category.AsCharPtr(), file.AsCharPtr());

		// delete audio
		ioServer->DeleteFile(resource);

		// remove audio
		window->RemoveAudioItem(this);
	}
}

//------------------------------------------------------------------------------
/**
*/
BaseItem* 
AudioItem::Clone()
{
	AudioItem* item = new AudioItem;
	item->SetName(this->GetName());
	item->SetWidget(this->GetWidget());
	item->SetHandler(this->GetHandler());
	return item;
}

} // namespace Widgets