#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::TrackModel
    
    Implements the data model for the track table view.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QStandardItemModel>
namespace Widgets
{
class TrackModel : public QStandardItemModel
{
public:
	/// constructor
	TrackModel();
	/// destructor
	virtual ~TrackModel();
}; 
} // namespace Widgets
//------------------------------------------------------------------------------