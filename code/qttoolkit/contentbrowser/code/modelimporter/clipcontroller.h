#pragma once
//------------------------------------------------------------------------------
/**
    @class Importer::ClipController
    
    Controller class for handling clips
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QObject>
#include <QItemSelection>
#include "ui_modelimporterwindow.h"
#include "toolkitutil/modelutil/clip.h"
#include "toolkitutil/modelutil/clipevent.h"
#include "fbx/nfbxreader.h"
#include "io/uri.h"
#include "toolkitutil/modelutil/modelattributes.h"
namespace ModelImporter
{
class ClipController : public QObject
{
	Q_OBJECT
public:
	/// constructor
	ClipController(Ui::ModelImporterWindow* ui);
	/// destructor
	~ClipController();

	/// handles selection change (in case one decides to use tab)
	void HandleSelection(const QModelIndex& index);
	/// sets the fbx reader
	void SetReader(NFbxReader* reader);

	/// emits the changed signal
	void EmitChanged();
	/// refreshes clip controller without requiring a new read
	void Load();

private slots:
	/// called when a new clip gets created
	void OnNewClip();
	/// called when a clip gets removed
	void OnRemoveClip();

    /// called when a clip is selected
    void OnClipSelected();
    /// called when the name of the current clip changes
    void OnClipNameChanged();
    /// called when the clip take is changed
    void OnClipTakeChanged();
    /// called when the start time is changed
    void OnClipStartChanged();
    /// called when the stop time is changed
    void OnClipStopChanged();
    /// called when the pre-infinity is changed
    void OnClipPreInfinityChanged();
    /// called when the post-infinity is changed
    void OnClipPostInfinityChanged();

    /// called when a new event gets added
    void OnNewEvent();
    /// called when the current event gets removed
    void OnRemoveEvent();

    /// called when an event is selected
    void OnEventSelected();
    /// called when the event name is changed
    void OnEventNameChanged();
    /// called when the event category is changed
    void OnEventCategoryChanged();
    /// called when the event marker is changed
    void OnEventMarkerChanged();
    /// called when the event marker type is changed
    void OnEventMarkerTypeChanged();

public slots:
	/// reader has loaded
	void ReaderLoaded(int status, const IO::URI& path);
	
signals:
	/// called whenever something changed
	void Changed(bool state);


private:

	/// sets controllers to state
	void SetControllersEnabled(bool state);

    /// enables clips
    void SetClipsEnabled(bool state);
    /// enables events
    void SetEventsEnabled(bool state);

    /// set the state of everything clip related except the new clip button
    void SetClipsEmpty(bool state);
    /// sets the sate of everything event related except the new event button
    void SetEventsEmpty(bool state);

	NFbxReader* reader;
	Ui::ModelImporterWindow* ui;
	Ptr<ToolkitUtil::ModelAttributes> attributes;
	Ptr<ToolkitUtil::Clip> currentClip;
    Ptr<ToolkitUtil::ClipEvent> currentEvent;
    Ptr<ToolkitUtil::Take> currentTake;
    Util::Array<Ptr<ToolkitUtil::Clip>> clips;
    Util::Array<Ptr<ToolkitUtil::ClipEvent>> events;
};

}