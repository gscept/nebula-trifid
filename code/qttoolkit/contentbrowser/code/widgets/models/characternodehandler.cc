//------------------------------------------------------------------------------
//  characternodehandler.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characternodehandler.h"
#include "graphics/graphicsprotocol.h"
#include "contentbrowserapp.h"
#include "previewer/previewstate.h"
#include "messaging/messagecallbackhandler.h"

using namespace Graphics;
using namespace Util;
namespace Widgets
{
__ImplementClass(Widgets::CharacterNodeHandler, 'CHNH', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
CharacterNodeHandler::CharacterNodeHandler() :
    currentSeekInterval(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
CharacterNodeHandler::~CharacterNodeHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::Setup()
{
	// first get current model
	this->model = ContentBrowser::ContentBrowserApp::Instance()->GetPreviewState()->GetModel();

	// now fetch skins
	Ptr<FetchSkinList> skinMsg = FetchSkinList::Create();
    __Send(this->model, skinMsg);
	__SingleFireCallback(CharacterNodeHandler, OnFetchedSkinList, this, skinMsg.upcast<Messaging::Message>());
	
	// do the same or clips
	Ptr<FetchClips> clipMsg = FetchClips::Create();
	__Send(this->model, clipMsg);
	__SingleFireCallback(CharacterNodeHandler, OnFetchedClipList, this, clipMsg.upcast<Messaging::Message>());

	// connect all shortcuts for double clicking the lists
	connect(this->ui->availableSkins, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnSkinActivated()));
	connect(this->ui->visibleSkins, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnSkinDeactivated()));

	// connect actual buttons
	connect(this->ui->addClip, SIGNAL(clicked()), this, SLOT(OnAddClip()));
	connect(this->ui->removeClip, SIGNAL(clicked()), this, SLOT(OnRemoveClip()));
	connect(this->ui->showSkin, SIGNAL(clicked()), this, SLOT(OnShowSkin()));
	connect(this->ui->hideSkin, SIGNAL(clicked()), this, SLOT(OnHideSkin()));
	connect(this->ui->playClip, SIGNAL(clicked()), this, SLOT(OnPlayClip()));
	connect(this->ui->stopClip, SIGNAL(clicked()), this, SLOT(OnStopClip()));
	connect(this->ui->pauseClip, SIGNAL(clicked()), this, SLOT(OnPauseClip()));
	connect(this->ui->seekBar, SIGNAL(valueChanged(int)), this, SLOT(OnClipSeek(int)));

	// connect clip list double click
	connect(this->ui->clipsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnClipDoubleClicked(QListWidgetItem*)));

	// create track controller
	this->trackController = new TrackController(this->ui);

	// connect controller stuff
	connect(this->trackController, SIGNAL(WeightChanged(IndexT)), this, SLOT(OnWeightChanged(IndexT)));
    connect(this->trackController, SIGNAL(ClipRemoved(IndexT)), this, SLOT(OnRemoveClip()));

	// connect skeleton render bool
	connect(this->ui->renderSkeleton, SIGNAL(toggled(bool)), this, SLOT(OnRenderSkeleton(bool)));
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::Discard()
{
	this->model = 0;
	delete this->trackController;
	this->trackController = 0;
    this->currentDurations.Clear();


	// make sure this object doesn't get called if the handler has been destroyed
	__AbortSingleFireCallbacks(CharacterNodeHandler, this);
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnSkinActivated()
{
	// get current item
	QListWidgetItem* currentItem = this->ui->availableSkins->currentItem();

	Ptr<ShowSkin> showSkin = ShowSkin::Create();
	showSkin->SetSkin(currentItem->text().toUtf8().constData());
	this->model->HandleMessage(showSkin.upcast<Messaging::Message>());

	// now remove item from visible skins and add it to available
	this->ui->visibleSkins->addItem(currentItem->text());
	this->ui->availableSkins->takeItem(this->ui->availableSkins->row(currentItem));
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnSkinDeactivated()
{
	// get current item
	QListWidgetItem* currentItem = this->ui->visibleSkins->currentItem();

	Ptr<HideSkin> hideSkin = HideSkin::Create();
	hideSkin->SetSkin(currentItem->text().toUtf8().constData());
	this->model->HandleMessage(hideSkin.upcast<Messaging::Message>());

	// now remove item from visible skins and add it to available
	this->ui->availableSkins->addItem(currentItem->text());
	this->ui->visibleSkins->takeItem(this->ui->visibleSkins->row(currentItem));
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnAddClip()
{
	// get selected item and index
	QListWidgetItem* item = this->ui->clipsList->selectedItems()[0];
	this->trackController->AddClip(item->text().toUtf8().constData());

    // set current duration in slot to be the actual duration
    SizeT duration = this->durations[this->ui->clipsList->currentIndex().row()];
    this->currentDurations[this->ui->clipsList->currentIndex().row()] = duration;

    // calculate maximum and set the seek bar to show this amount of frames
    SizeT maximum = 0;
    IndexT i;
    for (i = 0; i < this->currentDurations.Size(); i++)
    {
        maximum = Math::n_max(maximum, this->currentDurations[i]);
    }
    this->ui->seekBar->setMaximum(maximum / 40);

    // automatically start clips
	this->OnPlayClip();
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnRemoveClip()
{
	// get selected index
	QModelIndex index = this->ui->trackView->currentIndex();
	this->trackController->RemoveClip(index.row());

    // set duration to be 0
    this->currentDurations[this->ui->clipsList->currentIndex().row()] = 0;

    // calculate maximum and set the seek bar to show this amount of frames
    SizeT maximum = 0;
    IndexT i;
    for (i = 0; i < this->currentDurations.Size(); i++)
    {
        maximum = Math::n_max(maximum, this->currentDurations[i]);
    }
    this->ui->seekBar->setMaximum(maximum / 40);

    // automatically start clips
	this->OnPlayClip();
}

//------------------------------------------------------------------------------
/**
	Plays clip from track controller, if we have conflicting weights on the same track, 
	the latter clip will take over.
*/
void 
CharacterNodeHandler::OnPlayClip()
{
	// stop all prior clips
	Ptr<AnimStopAllTracks> stopMsg = AnimStopAllTracks::Create();
	stopMsg->SetAllowFadeOut(false);
	this->model->HandleMessage(stopMsg.upcast<Messaging::Message>());

	// get data from controller
	const Array<String>& clips = this->trackController->GetClips();
	const Array<IndexT>& tracks = this->trackController->GetTracks();
	const Array<float>& weights = this->trackController->GetWeights();

	// reset seek bar
	this->ui->seekBar->setValue(0);

	// iterate through clips
	IndexT i;
	for (i = 0; i < clips.Size(); i++)
	{
		// create message
		Ptr<AnimPlayClip> msg = AnimPlayClip::Create();

		// create message
		msg->SetClipName(clips[i]);
		msg->SetTrackIndex(i);
		msg->SetBlendWeight(weights[i]);
		msg->SetLoopCount(0);

		// send to model
        this->model->HandleMessage(msg.upcast<Messaging::Message>());
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnPauseClip()
{
	// create message and send to model
	Ptr<AnimPauseAllTracks> msg = AnimPauseAllTracks::Create();
	this->model->HandleMessage(msg.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnStopClip()
{
	// create message and send to model
	Ptr<AnimStopAllTracks> msg = AnimStopAllTracks::Create();
	msg->SetAllowFadeOut(false);
	this->model->HandleMessage(msg.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnClipSeek( int time )
{
	Ptr<AnimSeek> msg = AnimSeek::Create();
	msg->SetTime(time * 40);
    this->ui->currentFrame->setValue(time);
	this->model->HandleMessage(msg.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnClipDoubleClicked( QListWidgetItem* item )
{
	// simply call add clip
	this->OnAddClip();
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnWeightChanged( int index )
{
	// get weight and track
	float weight = this->trackController->GetWeights()[index];

	// create message
	Ptr<AnimModifyBlendWeight> msg = AnimModifyBlendWeight::Create();
	msg->SetBlendWeight(weight);
	msg->SetTrackIndex(index);
	this->model->HandleMessage(msg.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnShowSkin()
{
	if (this->ui->availableSkins->currentItem() != NULL)
	{
		this->OnSkinActivated();
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnHideSkin()
{
	if (this->ui->visibleSkins->currentItem() != NULL)
	{
		this->OnSkinDeactivated();
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::ReshowSkins()
{
	SizeT numSkins = this->ui->visibleSkins->count();
	IndexT skinIndex;
	for (skinIndex = 0; skinIndex < numSkins; skinIndex++)
	{
		Ptr<ShowSkin> showSkin = ShowSkin::Create();
		QListWidgetItem* item = this->ui->visibleSkins->item(skinIndex);
		showSkin->SetSkin(item->text().toUtf8().constData());
		this->model->HandleMessage(showSkin.upcast<Messaging::Message>());
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnRenderSkeleton(bool b)
{
	Ptr<SetRenderSkeleton> skelRend = SetRenderSkeleton::Create();
	skelRend->SetRender(b);
	this->model->HandleMessage(skelRend.downcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnFetchedSkinList( const Ptr<Messaging::Message>& msg )
{
	const Ptr<FetchSkinList>& skinMsg = msg.downcast<FetchSkinList>();

	// then copy list to our skin list
	this->skins = skinMsg->GetSkins();

	if (this->skins.Size() > 0)
	{
		// first add the first skin to our list of visible skins
		this->ui->visibleSkins->addItem(skins[0].AsString().AsCharPtr());

		// go through skins and add to available list
		IndexT i;
		for (i = 1; i < skins.Size(); i++)
		{
			this->ui->availableSkins->addItem(this->skins[i].AsString().AsCharPtr());
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterNodeHandler::OnFetchedClipList( const Ptr<Messaging::Message>& msg )
{
	const Ptr<FetchClips>& clipMsg = msg.downcast<FetchClips>();

	// then copy list to our clips list
	this->clips = clipMsg->GetClips();
	this->durations = clipMsg->GetLenghts();

    // resize current clip array
    this->currentDurations.Resize(this->clips.Size());
    this->currentDurations.Fill(0);

	// go through clips and add them
	IndexT i;
	for (i = 0; i < this->clips.Size(); i++)
	{
		this->ui->clipsList->addItem(this->clips[i].AsString().AsCharPtr());
	}
}

} // namespace Widgets