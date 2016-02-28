//------------------------------------------------------------------------------
//  audioitemhandler.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "audiohandler.h"
#include "faudio/audiodevice.h"
#include "io/ioserver.h"

namespace Widgets
{
__ImplementClass(Widgets::AudioHandler, 'AUIH', Widgets::BaseHandler);

//------------------------------------------------------------------------------
/**
*/
AudioHandler::AudioHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AudioHandler::~AudioHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
AudioHandler::Setup()
{
	BaseHandler::Setup();

	// connect reload buttons to actions
	connect(this->ui->stopButton, SIGNAL(clicked()), this, SLOT(Stop()));
	connect(this->ui->pauseButton, SIGNAL(clicked()), this, SLOT(Pause()));
	connect(this->ui->playButton, SIGNAL(clicked()), this, SLOT(Play()));
}

//------------------------------------------------------------------------------
/**
*/
void
AudioHandler::Cleanup()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
AudioHandler::Discard()
{	
	if (this->eventInstance.isvalid())
	{
		this->Stop();
		FAudio::AudioDevice::Instance()->DiscardEvent(this->eventInstance);
		this->eventInstance = 0;		
	}
	return BaseHandler::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void
AudioHandler::SetUI(Ui::AudioInfoWidget* ui)
{
	n_assert(ui);
	this->ui = ui;
	int count = 0;
	this->ui->tableWidget->insertRow(0);
	QTableWidgetItem * label = new QTableWidgetItem("Path");
	this->ui->tableWidget->setItem(count, 0, label);
	label = new QTableWidgetItem("");
	this->ui->tableWidget->setItem(count++, 1, label);

	this->ui->tableWidget->insertRow(count);
	label = new QTableWidgetItem("Length");
	this->ui->tableWidget->setItem(count, 0, label);
	label = new QTableWidgetItem("");
	this->ui->tableWidget->setItem(count++, 1, label);

	this->ui->tableWidget->insertRow(count);	
	label = new QTableWidgetItem("Max Distance");
	this->ui->tableWidget->setItem(count, 0, label);
	label = new QTableWidgetItem("");
	this->ui->tableWidget->setItem(count++, 1, label);

	this->ui->tableWidget->insertRow(count);
	label = new QTableWidgetItem("Min Distance");
	this->ui->tableWidget->setItem(count, 0, label);
	label = new QTableWidgetItem("");
	this->ui->tableWidget->setItem(count++, 1, label);

	this->ui->tableWidget->insertRow(count);
	label = new QTableWidgetItem("Is 3D");
	this->ui->tableWidget->setItem(count, 0, label);
	label = new QTableWidgetItem("");
	this->ui->tableWidget->setItem(count++, 1, label);

	this->ui->tableWidget->insertRow(count);
	label = new QTableWidgetItem("Oneshot");
	this->ui->tableWidget->setItem(count, 0, label);
	label = new QTableWidgetItem("");
	this->ui->tableWidget->setItem(count++, 1, label);

	this->ui->tableWidget->insertRow(count);
	label = new QTableWidgetItem("Streaming");
	this->ui->tableWidget->setItem(count, 0, label);
	label = new QTableWidgetItem("");
	this->ui->tableWidget->setItem(count++, 1, label);
}
//------------------------------------------------------------------------------
/**
*/
void
AudioHandler::LoadAudioInfo()
{
	if (this->eventInstance.isvalid())
	{
		this->Stop();
		FAudio::AudioDevice::Instance()->DiscardEvent(this->eventInstance);
		this->eventInstance = 0;
	}
	
	this->eventInstance = FAudio::AudioDevice::Instance()->CreateEvent(this->path);
	
	FMOD::Studio::System* system = FAudio::AudioDevice::Instance()->GetSystem();
	FMOD::Studio::EventDescription * info;
	FMOD_RESULT res = system->getEvent(this->path.AsCharPtr(), &info);
	
	char buffer[255];
	int count = 0;
	if (res == FMOD_OK)
	{
		QTableWidgetItem * value;
		info->getPath(buffer, 255, 0);		
		value = this->ui->tableWidget->takeItem(count, 1);
		value->setText(buffer);
		this->ui->tableWidget->setItem(count++, 1, value);
			
		int length;
		info->getLength(&length);
		float flength = (float)length / 1000.0f;
		QString valString;
		valString.setNum(flength);
		valString += "s";
		value = this->ui->tableWidget->takeItem(count, 1);
		value->setText(valString);
		this->ui->tableWidget->setItem(count++, 1, value);

		info->getMaximumDistance(&flength);
		valString.setNum(flength);
		valString += "m";
		value = this->ui->tableWidget->takeItem(count, 1);
		value->setText(valString);
		this->ui->tableWidget->setItem(count++, 1, value);

		info->getMinimumDistance(&flength);
		valString.setNum(flength);
		valString += "m";
		value = this->ui->tableWidget->takeItem(count, 1);
		value->setText(valString);
		this->ui->tableWidget->setItem(count++, 1, value);
		
		bool fb;				
		info->is3D(&fb);
		valString = fb ? "true" : "false";		
		value = this->ui->tableWidget->takeItem(count, 1);
		value->setText(valString);
		this->ui->tableWidget->setItem(count++, 1, value);
		
		info->isOneshot(&fb);
		valString = fb ? "true" : "false";
		value = this->ui->tableWidget->takeItem(count, 1);
		value->setText(valString);
		this->ui->tableWidget->setItem(count++, 1, value);
		
		info->isStream(&fb);
		valString = fb ? "true" : "false";
		value = this->ui->tableWidget->takeItem(count, 1);
		value->setText(valString);
		this->ui->tableWidget->setItem(count++, 1, value);

		this->ui->tableWidget->resizeColumnsToContents();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AudioHandler::Stop()
{	
	if (this->eventInstance.isvalid())
	{
		this->eventInstance->Stop();
	}		
}

//------------------------------------------------------------------------------
/**
*/
void
AudioHandler::Pause()
{
	n_assert(this->eventInstance.isvalid());

	FAudio::EventState state = this->eventInstance->GetState();
	switch (state)
	{
		case FAudio::EventPlaying:
			this->eventInstance->Pause();
			break;
		case FAudio::EventPaused:
			this->eventInstance->Resume();
			break;			
	}		
}
//------------------------------------------------------------------------------
/**
*/
void
AudioHandler::Play()
{
	n_assert(this->eventInstance.isvalid());
	FAudio::EventState state = this->eventInstance->GetState();	
		
	switch (state)
	{
	case FAudio::EventStopped:
		this->eventInstance->Play();
		break;
	case FAudio::EventPaused:
		this->eventInstance->Resume();
	}
}

} // namespace Widgets