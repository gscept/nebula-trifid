#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::TrackController
    
    Implements a track controller for the character animation UI.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QObject>
#include "core/refcounted.h"
#include "ui_characternodeinfowidget.h"
#include "trackmodel.h"
#include "trackdelegate.h"
namespace Widgets
{
class TrackController : public QObject
{
	Q_OBJECT
public:
	/// constructor
	TrackController(Ui::CharacterNodeInfoWidget* ui);
	/// destructor
	virtual ~TrackController();

	/// adds a clip with default settings
	void AddClip(const Util::String& clip);
	/// removes clip and its settings
	void RemoveClip(IndexT i);

	/// returns list of clips
	const Util::Array<Util::String>& GetClips();
	/// returns list of tracks
	const Util::Array<IndexT>& GetTracks();
	/// returns list of weights
	const Util::Array<float>& GetWeights();
	/// returns list of loop counts
	const Util::Array<SizeT>& GetLoopCounts();

signals:
	/// called from delegate when a weight has changed
	void WeightChanged(IndexT i);
    /// called from delegate when a clip has been removed
    void ClipRemoved(IndexT i);

private:
	friend class TrackDelegate;

	TrackModel* model;
	TrackDelegate* delegate;
	Ui::CharacterNodeInfoWidget* ui;
	Util::Array<Util::String> clips;
	Util::Array<IndexT> tracks;
	Util::Array<float> weights;
	Util::Array<SizeT> loopCounts;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Util::String>& 
TrackController::GetClips()
{
	return this->clips;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<IndexT>& 
TrackController::GetTracks()
{
	return this->tracks;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<float>& 
TrackController::GetWeights()
{
	return this->weights;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<SizeT>& 
TrackController::GetLoopCounts()
{
	return this->loopCounts;
}

} // namespace Widgets
//------------------------------------------------------------------------------