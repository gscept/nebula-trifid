//------------------------------------------------------------------------------
//  trackcontroller.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "trackcontroller.h"

namespace Widgets
{

//------------------------------------------------------------------------------
/**
*/
TrackController::TrackController(Ui::CharacterNodeInfoWidget* ui)
{
	n_assert(ui);
	this->model = new TrackModel;
	this->delegate = new TrackDelegate(this);
	this->ui = ui;

	this->ui->trackView->setItemDelegate(this->delegate);
	this->ui->trackView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	this->ui->trackView->verticalHeader()->setDefaultSectionSize(15);
	this->ui->trackView->verticalHeader()->setResizeMode(QHeaderView::Fixed);
	this->ui->trackView->setModel(this->model);
}

//------------------------------------------------------------------------------
/**
*/
TrackController::~TrackController()
{
	delete this->model;
	delete this->delegate;
	this->ui = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
TrackController::AddClip( const Util::String& clip )
{
	// add data to arrays
	this->clips.Append(clip);
	this->weights.Append(1.0f);

	// resize model
	this->model->setRowCount(this->clips.Size());

	IndexT i;
	for (i = 0; i < this->clips.Size(); i++)
	{
		// add to list
		this->model->setData(this->model->index(i, 0), clips[i].AsCharPtr());
		this->model->setData(this->model->index(i, 1), weights[i]);
		this->model->item(i, 0)->setTextAlignment(Qt::AlignCenter);
		this->model->item(i, 1)->setTextAlignment(Qt::AlignCenter);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
TrackController::RemoveClip( IndexT i )
{
	if (i != InvalidIndex)
	{
		// remove data from arrays
		this->clips.EraseIndex(i);
		this->weights.EraseIndex(i);

		// resize model
		this->model->setRowCount(this->clips.Size());

		IndexT j;
		for (j = 0; j < this->clips.Size(); j++)
		{
			// add to list
			this->model->setData(this->model->index(j, 0), clips[j].AsCharPtr());
			this->model->setData(this->model->index(j, 1), weights[j]);
		}
	}
}

} // namespace Widgets