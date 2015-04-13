#include "stdneb.h"
#include "progressnotifier.h"
#include <QDataStream>
namespace QtTools
{
ProgressNotifier* ProgressNotifier::instance = 0;
//------------------------------------------------------------------------------
/**
*/
ProgressNotifier::ProgressNotifier() : 
	bar(0),
	status(0)
{
	// empty	
}

//------------------------------------------------------------------------------
/**
*/
ProgressNotifier::~ProgressNotifier()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ProgressNotifier* 
ProgressNotifier::Instance()
{
	if (instance == 0)
	{
		instance = new ProgressNotifier();
	}

	return instance;
}

//------------------------------------------------------------------------------
/**
*/
void 
ProgressNotifier::SetProgressBar( QProgressBar* bar )
{
	this->bar = bar;
}

//------------------------------------------------------------------------------
/**
*/
void 
ProgressNotifier::SetStatusLabel( QLabel* label )
{
	this->status = label;
}


//------------------------------------------------------------------------------
/**
*/
void 
ProgressNotifier::Update( float value, const QString& status )
{
	if (this->bar)
	{
		this->bar->setValue(value);
	}
	if (this->status)
	{
		this->status->setText("Loading: " + status);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ProgressNotifier::Start( const QString& status /* = "" */ )
{
	if (this->bar)
	{
		this->bar->setVisible(true);
		this->bar->setValue(-1);

		this->bar->setMinimum(0);
		this->bar->setMaximum(0);
	}
	if (this->status)
	{
		this->status->setVisible(true);
		this->status->setText(status);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ProgressNotifier::End()
{
	if (this->bar)
	{
		// reset min and max
		this->bar->setMinimum(0);
		this->bar->setMaximum(0);

		this->bar->setValue(0);
		this->bar->setVisible(false);
	}
	if (this->status)
	{
		this->status->setVisible(false);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ProgressNotifier::Increment( float increment, const QString& status /* = "" */ )
{
	if (this->bar)
	{
		this->bar->setValue(this->bar->value()+increment);
	}
	if (this->status)
	{
		this->status->setText(status);
	}
}

}