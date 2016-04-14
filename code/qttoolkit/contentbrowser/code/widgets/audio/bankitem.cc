//------------------------------------------------------------------------------
//  bankitem.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "bankitem.h"
#include "contentbrowserapp.h"
#include "faudio/audiodevice.h"
#include "audioitem.h"

using namespace Util;
namespace Widgets
{
//------------------------------------------------------------------------------
/**
*/
BankItem::BankItem() :loaded(false)
{
	this->type = AudioBankItemType;
}

//------------------------------------------------------------------------------
/**
*/
BankItem::~BankItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
BankItem::OnActivated()
{
	BaseItem::OnActivated();
}

//------------------------------------------------------------------------------
/**
*/
void
BankItem::OnClicked()
{
	BaseItem::OnClicked();
	if (!this->loaded)
	{			
		if (!FAudio::AudioDevice::Instance()->BankLoaded(this->GetName().toLatin1().constData()))
		{
			FAudio::AudioDevice::Instance()->LoadBank(this->GetName().toLatin1().constData());
		}
		
		FMOD::Studio::Bank * bank = FAudio::AudioDevice::Instance()->GetBank(this->GetName().toLatin1().constData());
		if (bank)
		{
			this->loaded = true;
		}

		FMOD::Studio::System* system = FAudio::AudioDevice::Instance()->GetSystem();
		
		if (bank != NULL)
		{
			int count = 0; 
			bank->getEventCount(&count);
			if (count)
			{
				int countret;
				char buffer[255];
				FMOD::Studio::EventDescription ** events = new FMOD::Studio::EventDescription*[count];
				FMOD_RESULT res = bank->getEventList(events, count, &countret);
				for (int i = 0; i < countret; i++)
				{
					int ret;
					res = events[i]->getPath(buffer, 255, &ret);
					AudioItem * audioitem = new AudioItem;
					audioitem->SetHandler(this->GetHandler().upcast<BaseHandler>());
					audioitem->SetName(buffer);
					audioitem->SetWidget(this->GetWidget());
					audioitem->setText(0, buffer);										
					this->addChild(audioitem);
				}
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
BankItem::OnRightClicked(const QPoint& pos)
{
	// create menu
	QMenu menu(this->treeWidget());

	// create actions
	QAction* action1 = new QAction("Unload audio bank", NULL);

	// get window
	ContentBrowser::ContentBrowserWindow* window = ContentBrowser::ContentBrowserApp::Instance()->GetWindow();

	// setup menu
	menu.addAction(action1);

	// execute menu
	QAction* action = menu.exec(this->treeWidget()->mapToGlobal(pos));

	// get file
	String file = this->text(0).toUtf8().constData();
	FAudio::AudioDevice::Instance()->UnloadBank(file);
	QList<QTreeWidgetItem*> items = this->takeChildren();
	for (auto i = items.begin() ; i!= items.end(); i++)
	{		
		delete *i;
	}
	items.clear();
	this->loaded = false;	
}

//------------------------------------------------------------------------------
/**
*/
BaseItem* 
BankItem::Clone()
{
	BankItem* item = new BankItem;
	item->SetName(this->GetName());
	item->SetWidget(this->GetWidget());
	item->SetHandler(this->GetHandler());
	return item;
}

} // namespace Widgets