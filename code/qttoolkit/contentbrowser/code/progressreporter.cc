//------------------------------------------------------------------------------
//  progressreporter.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "progressreporter.h"
#include <QApplication>

namespace ContentBrowser
{
__ImplementSingleton(ContentBrowser::ProgressReporter);
__ImplementClass(ContentBrowser::ProgressReporter, 'PROG', Core::RefCounted);
//------------------------------------------------------------------------------
/**
*/
ProgressReporter::ProgressReporter() :
	isOpen(false),
	isReporting(false),
	progressBar(0),
	statusLabel(0)
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ProgressReporter::~ProgressReporter()
{
	if (this->IsOpen())
	{
		this->Close();
	}
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
ProgressReporter::Open()
{
	n_assert(!this->IsOpen());
	this->isOpen = true;
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ProgressReporter::Close()
{
	n_assert(this->IsOpen());
	this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void 
ProgressReporter::ReportProgress( float progress, const Util::String& status )
{
	ProgressReporter* self = ProgressReporter::Instance();
	n_assert(self->IsOpen());
	n_assert(0 != self->progressBar);
	n_assert(0 != self->statusLabel);

	self->progressBar->setValue(self->progressBar->value() + progress);
	self->statusLabel->setText(status.AsCharPtr());

}

//------------------------------------------------------------------------------
/**
*/
void 
ProgressReporter::ReportProgress( const Util::String& status )
{
	ProgressReporter* self = ProgressReporter::Instance();
	n_assert(self->IsOpen());
	n_assert(0 != self->progressBar);
	n_assert(0 != self->statusLabel);

	self->progressBar->setValue(self->progressBar->value() + 1/(float)self->progressBar->maximum());
	self->statusLabel->setText(status.AsCharPtr());

	// force event update
	QApplication::processEvents();
}

//------------------------------------------------------------------------------
/**
*/
void 
ProgressReporter::SetupProgress( int min, int max )
{
	ProgressReporter* self = ProgressReporter::Instance();
	n_assert(self->IsOpen());
	n_assert(0 != self->progressBar);
	n_assert(0 != self->statusLabel);
	
	self->progressBar->setMinimum(min);
	self->progressBar->setMaximum(max);
	self->statusLabel->setText("");
}

//------------------------------------------------------------------------------
/**
*/
void 
ProgressReporter::BeginProgressReporting()
{
	n_assert(!this->IsReporting());
	this->isReporting = true;
	this->progressBar->setVisible(true);
	this->statusLabel->setVisible(true);

	// makes the labels become visible directly
	QApplication::processEvents();
}

//------------------------------------------------------------------------------
/**
*/
void 
ProgressReporter::EndProgressReporting()
{
	n_assert(this->IsReporting());
	this->isReporting = false;
	this->progressBar->setVisible(false);
	this->statusLabel->setVisible(false);

	this->progressBar->reset();
	this->statusLabel->setText("");

	this->statusLabel = 0;
	this->progressBar = 0;
}
}
