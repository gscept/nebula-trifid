//------------------------------------------------------------------------------
//  clipcontroller.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "clipcontroller.h"
#include <QtGui/QHeaderView>
#include <QTableView>
#include "toolkitutil/modelutil/modeldatabase.h"

using namespace ToolkitUtil;
using namespace Util;
namespace ModelImporter
{
//------------------------------------------------------------------------------
/**
*/ 
ClipController::ClipController( Ui::ModelImporterWindow* ui ) : 
	attributes(0),
	currentClip(0),
    currentTake(0),
    currentEvent(0)
{
	this->ui = ui;

	this->SetControllersEnabled(false);

    // clip stuff
    QObject::connect(this->ui->newClip, SIGNAL(clicked()), this, SLOT(OnNewClip()));
	QObject::connect(this->ui->removeClip, SIGNAL(clicked()), this, SLOT(OnRemoveClip()));
    QObject::connect(this->ui->clips, SIGNAL(currentIndexChanged(int)), this, SLOT(OnClipSelected()));
    QObject::connect(this->ui->clipName, SIGNAL(textChanged(const QString&)), this, SLOT(OnClipNameChanged()));
    QObject::connect(this->ui->take, SIGNAL(currentIndexChanged(int)), this, SLOT(OnClipTakeChanged()));
    QObject::connect(this->ui->startTime, SIGNAL(valueChanged(int)), this, SLOT(OnClipStartChanged()));
    QObject::connect(this->ui->stopTime, SIGNAL(valueChanged(int)), this, SLOT(OnClipStopChanged()));
    QObject::connect(this->ui->preInfinity, SIGNAL(currentIndexChanged(int)), this, SLOT(OnClipPreInfinityChanged()));
    QObject::connect(this->ui->postInfinity, SIGNAL(currentIndexChanged(int)), this, SLOT(OnClipPostInfinityChanged()));

    // event stuff
    QObject::connect(this->ui->newEvent, SIGNAL(clicked()), this, SLOT(OnNewEvent()));
    QObject::connect(this->ui->removeEvent, SIGNAL(clicked()), this, SLOT(OnRemoveEvent()));
    QObject::connect(this->ui->events, SIGNAL(currentIndexChanged(int)), this, SLOT(OnEventSelected()));
    QObject::connect(this->ui->eventName, SIGNAL(textChanged(const QString&)), this, SLOT(OnEventNameChanged()));
    QObject::connect(this->ui->eventCategory, SIGNAL(textChanged(const QString&)), this, SLOT(OnEventCategoryChanged()));
    QObject::connect(this->ui->marker, SIGNAL(valueChanged(int)), this, SLOT(OnEventMarkerChanged()));
    QObject::connect(this->ui->markerType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnEventMarkerTypeChanged()));
}

//------------------------------------------------------------------------------
/**
*/ 
ClipController::~ClipController()
{
	this->ui = 0;
    this->currentClip = 0;
    this->currentEvent = 0;
    this->currentTake = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnNewClip()
{
	Ptr<Clip> newClip = Clip::Create();
	newClip->SetName("New clip");
	newClip->SetStart(0);
	newClip->SetEnd(0);
	newClip->SetPreInfinity(Clip::Constant);
	newClip->SetPostInfinity(Clip::Constant);
    newClip->SetTake(this->attributes->GetTakes()[0]);
    newClip->GetTake()->AddClip(newClip);
    this->clips.Append(newClip);

    // add to UI
    this->ui->clips->addItem(newClip->GetName().AsCharPtr());
	this->ui->clips->setCurrentIndex(this->clips.Size() - 1);
	this->SetClipsEmpty(false);
    this->SetEventsEnabled(true);

	emit Changed(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnRemoveClip()
{
    n_assert(this->currentClip);
    this->currentClip->GetTake()->RemoveClip(this->currentClip);
    IndexT clipIndex = this->clips.FindIndex(this->currentClip);
    this->ui->clips->blockSignals(true);
    this->ui->clips->removeItem(clipIndex);    
    this->ui->clips->blockSignals(false);
    this->clips.EraseIndex(clipIndex);
    if (this->clips.Size() == 0)
    {
        this->SetClipsEmpty(true);
        this->SetEventsEnabled(false);
        this->ui->newClip->setEnabled(true);
    }
    else
    {
        this->OnClipSelected();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnClipSelected()
{
    IndexT index = this->ui->clips->currentIndex();   
    const Ptr<Clip>& clip = this->clips[index];
    this->currentClip = clip;

    // block all signals
    this->ui->clipName->blockSignals(true);
    this->ui->startTime->blockSignals(true);
    this->ui->stopTime->blockSignals(true);
    this->ui->preInfinity->blockSignals(true);
    this->ui->postInfinity->blockSignals(true);
    this->ui->take->blockSignals(true);

    // setup UI
    this->ui->clipName->setText(clip->GetName().AsCharPtr());
    this->ui->startTime->setValue(clip->GetStart());
    this->ui->stopTime->setValue(clip->GetEnd());
    this->ui->preInfinity->setCurrentIndex(clip->GetPreInfinity());
    this->ui->postInfinity->setCurrentIndex(clip->GetPostInfinity());
    this->ui->take->setCurrentIndex(this->ui->take->findText(clip->GetTake()->GetName().AsCharPtr()));

    // unblock them
    this->ui->clipName->blockSignals(false);
    this->ui->startTime->blockSignals(false);
    this->ui->stopTime->blockSignals(false);
    this->ui->preInfinity->blockSignals(false);
    this->ui->postInfinity->blockSignals(false);
    this->ui->take->blockSignals(true);

    // go through and setup events
    this->ui->events->blockSignals(true);
    this->ui->events->clear();
    this->ui->events->blockSignals(false);
    this->events.Clear();

    if (clip->GetNumEvents() > 0)
    {
        const Util::Array<Ptr<ClipEvent>>& events = clip->GetEvents();
        IndexT i;
        this->ui->events->blockSignals(true);
        for (i = 0; i < events.Size(); i++)
        {
            const Ptr<ClipEvent>& event = events[i];
            this->events.Append(event);
            this->ui->events->addItem(event->GetName().AsCharPtr());
        }
        this->ui->events->blockSignals(false);

        // set events to not be empty
        this->SetEventsEmpty(false);

        // invoke signal for event
        this->OnEventSelected();
    }    
    else
    {
        // disable events, but keep the new event button open     
        this->SetEventsEmpty(true);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnClipNameChanged()
{
    n_assert(this->currentClip.isvalid());
    this->currentClip->SetName(this->ui->clipName->text().toUtf8().constData());
    IndexT clipIndex = this->clips.FindIndex(this->currentClip);
    this->ui->clips->setItemText(clipIndex, this->currentClip->GetName().AsCharPtr());
    emit Changed(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnClipTakeChanged()
{
    n_assert(this->currentClip.isvalid());
    QString name = this->ui->clips->currentText();
    const Ptr<Take>& take = this->attributes->GetTake(name.toUtf8().constData());
    this->currentClip->GetTake()->RemoveClip(this->currentClip);
    this->currentClip->SetTake(take);
    take->AddClip(this->currentClip);
    emit Changed(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnClipStartChanged()
{
    n_assert(this->currentClip.isvalid());
    this->currentClip->SetStart(this->ui->startTime->value());
    emit Changed(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnClipStopChanged()
{
    n_assert(this->currentClip.isvalid());
    this->currentClip->SetEnd(this->ui->stopTime->value());
    emit Changed(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnClipPreInfinityChanged()
{
    n_assert(this->currentClip.isvalid());
    this->currentClip->SetPreInfinity((Clip::InfinityType)this->ui->preInfinity->currentIndex());
    emit Changed(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnClipPostInfinityChanged()
{
    n_assert(this->currentClip.isvalid());
    this->currentClip->SetPostInfinity((Clip::InfinityType)this->ui->postInfinity->currentIndex());
    emit Changed(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnNewEvent()
{
    n_assert(this->currentClip.isvalid());
    Ptr<ClipEvent> event = ClipEvent::Create();
    event->SetName("New event");
    event->SetCategory("");
    event->SetMarkerAsMilliseconds(0);

    this->currentClip->AddEvent(event);
    this->events.Append(event);

    // add to ui
    this->ui->events->addItem(event->GetName().AsCharPtr());
	this->ui->events->setCurrentIndex(this->events.Size() - 1);
    this->SetEventsEmpty(false);

    emit Changed(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnRemoveEvent()
{
    n_assert(this->currentClip.isvalid());
    n_assert(this->currentEvent.isvalid());

    this->currentClip->RemoveEvent(this->currentEvent);
    IndexT eventIndex = this->events.FindIndex(this->currentEvent);
    this->ui->events->blockSignals(true);
    this->ui->events->removeItem(eventIndex);    
    this->ui->events->blockSignals(false);
    this->events.EraseIndex(eventIndex);

    if (this->events.Size() == 0)
    {
        this->SetEventsEmpty(true);
    }
    else
    {
        this->OnEventSelected();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnEventSelected()
{
    n_assert(this->currentClip.isvalid());

    IndexT index = this->ui->events->currentIndex();   
    const Ptr<ClipEvent>& event = this->events[index];
    this->currentEvent = event;

    // block all signals
    this->ui->eventName->blockSignals(true);
    this->ui->eventCategory->blockSignals(true);
    this->ui->marker->blockSignals(true);
    this->ui->markerType->blockSignals(true);

    // setup UI
    this->ui->eventName->setText(event->GetName().AsCharPtr());
    this->ui->eventCategory->setText(event->GetCategory().AsCharPtr());
    this->ui->marker->setValue(event->GetMarker());
    this->ui->markerType->setCurrentIndex(event->GetMarkerType());

    // unblock them
    this->ui->eventName->blockSignals(false);
    this->ui->eventCategory->blockSignals(false);
    this->ui->marker->blockSignals(false);
    this->ui->markerType->blockSignals(false);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnEventNameChanged()
{
    n_assert(this->currentClip.isvalid());
    n_assert(this->currentEvent.isvalid());
    this->currentEvent->SetName(this->ui->eventName->text().toUtf8().constData());
    IndexT eventIndex = this->events.FindIndex(this->currentEvent);
    this->ui->events->setItemText(eventIndex, this->currentEvent->GetName().AsCharPtr());
    emit Changed(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnEventCategoryChanged()
{
    n_assert(this->currentClip.isvalid());
    n_assert(this->currentEvent.isvalid());
    this->currentEvent->SetCategory(this->ui->eventCategory->text().toUtf8().constData());
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnEventMarkerChanged()
{
    n_assert(this->currentClip.isvalid());
    n_assert(this->currentEvent.isvalid());
    IndexT markerType = this->ui->markerType->currentIndex();
    switch (markerType)
    {
    case ClipEvent::Ticks:
        this->currentEvent->SetMarkerAsMilliseconds(this->ui->marker->value());
        break;
    case ClipEvent::Frames:
        this->currentEvent->SetMarkerAsFrames(this->ui->marker->value());
        break;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::OnEventMarkerTypeChanged()
{
    // do the same as the above function
    this->OnEventMarkerChanged();
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::SetReader( NFbxReader* reader )
{
	this->reader = reader;
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::ReaderLoaded( int status, const IO::URI& path )
{
	if (status == NFbxReader::Success)
	{
		Util::String fileExt = path.GetHostAndLocalPath().ExtractFileName().AsCharPtr();
		fileExt.StripFileExtension();
		String file = fileExt.AsCharPtr();
		String category = path.GetHostAndLocalPath().ExtractLastDirName().AsCharPtr();
		String model = category + "/" + file;

        this->ui->take->blockSignals(true);
        this->ui->clips->blockSignals(true);
        this->ui->events->blockSignals(true);
		this->ui->take->clear();
        this->ui->clips->clear();
        this->ui->events->clear();        
        QApplication::processEvents();

		const Array<String> takes = this->reader->GetTakes();
		for (int takeIndex = 0; takeIndex < takes.Size(); takeIndex++)
		{
			this->ui->take->addItem(takes[takeIndex].AsCharPtr());
		}

        this->ui->take->blockSignals(false);
        this->ui->clips->blockSignals(false);
        this->ui->events->blockSignals(false);
	
		Ptr<ModelAttributes> attrs = ModelDatabase::Instance()->LookupAttributes(model);
		this->attributes = attrs;	

		Array<Ptr<Take> > takeList;
		IndexT i;
		for (i = 0; i < takes.Size(); i++)
		{
			if (attrs->HasTake(takes[i]))
			{
				takeList.Append(attrs->GetTake(takes[i]));
			}
			else
			{
				Ptr<Take> newTake = Take::Create();
				newTake->SetName(takes[i]);
				takeList.Append(newTake);
			}
		}

		// now clear batch takes, this ensures we don't have old takes laying around
		attrs->ClearTakes();

		// now add takes again
		for (i = 0; i < takeList.Size(); i++)
		{
			attrs->AddTake(takeList[i]);
		}

		// setup clips
		this->Load();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::Load()
{
	bool enableControllers = true;
    this->clips.Clear();
    this->events.Clear();
	if (this->attributes->GetTakes().Size() > 0)
	{
        this->ui->take->setCurrentIndex(0);
        const Ptr<Take>& take = this->attributes->GetTakes()[0];

        // enable clips view
        this->SetClipsEnabled(true);

        if (take->GetNumClips() > 0)
        {
            const Util::Array<Ptr<Clip>>& clips = take->GetClips();
            IndexT i;
            this->ui->clips->blockSignals(true);
            for (i = 0; i < clips.Size(); i++)
            {
                const Ptr<Clip>& clip = clips[i];
                this->clips.Append(clip);
                this->ui->clips->addItem(clip->GetName().AsCharPtr());
            }
            this->ui->clips->blockSignals(false);

            // set clips enabled
            this->SetEventsEnabled(true);

            // invoke slot which triggers selection of clips
            this->OnClipSelected();
        }
        else
        {
            // we have no clips
            this->SetClipsEmpty(true);
        }
	}
	else
	{
		enableControllers = false;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::EmitChanged()
{
	emit Changed(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::SetControllersEnabled( bool state )
{
    this->SetClipsEnabled(state);
    this->SetEventsEnabled(state);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::SetClipsEnabled( bool state )
{
    this->ui->clipFrame->setEnabled(state);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::SetEventsEnabled( bool state )
{
    this->ui->eventFrame->setEnabled(state);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::SetClipsEmpty( bool state )
{
    this->ui->clips->setEnabled(!state);
    this->ui->removeClip->setEnabled(!state);
    this->ui->clipName->setEnabled(!state);
    this->ui->take->setEnabled(!state);
    this->ui->startTime->setEnabled(!state);
    this->ui->stopTime->setEnabled(!state);
    this->ui->preInfinity->setEnabled(!state);
    this->ui->postInfinity->setEnabled(!state);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipController::SetEventsEmpty( bool state )
{
    this->ui->events->setEnabled(!state);
    this->ui->removeEvent->setEnabled(!state);
    this->ui->eventName->setEnabled(!state);
    this->ui->eventCategory->setEnabled(!state);
    this->ui->marker->setEnabled(!state);
    this->ui->markerType->setEnabled(!state);
}
} // namespace ModelImporter