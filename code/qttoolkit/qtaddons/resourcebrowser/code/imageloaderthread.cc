//------------------------------------------------------------------------------
//  textureloaderthread.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "imageloaderthread.h"
#include "io/uri.h"

namespace ResourceBrowser
{

//------------------------------------------------------------------------------
/**
*/
ImageLoaderThread::ImageLoaderThread(QObject* parent) : 
	QThread(parent),
	shouldStop(false),
	paused(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ImageLoaderThread::~ImageLoaderThread()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ImageLoaderThread::run()
{
	// run forever
	while (!this->shouldStop)
	{
		while (!this->queue.IsEmpty() && !this->paused)
		{
			// get unit and lock it so that we don't delete it
			ImageLoaderUnit* unit = this->queue.Dequeue();
			unit->mutex.lock();

			// get parameters
			Util::String path = unit->path;
			QImage* image = unit->texture;

			// convert to absolute path
			IO::URI uri(path);

			// load image and scale
			QImage localImage(uri.LocalPath().AsCharPtr());
			*image = localImage.scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation);

			// emit signal to notify the image has been loaded
			emit unit->OnLoaded();

			// unlock mutex
			unit->mutex.unlock();
		}
		n_sleep(0.1);
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
ImageLoaderThread::Start()
{
	this->start();
}

//------------------------------------------------------------------------------
/**
*/
void
ImageLoaderThread::Stop()
{
	this->queue.Clear();
	this->shouldStop = true;
	this->quit();
}

//------------------------------------------------------------------------------
/**
*/
void
ImageLoaderThread::Pause(bool b)
{
	this->paused = b;
}

//------------------------------------------------------------------------------
/**
*/
void
ImageLoaderThread::Clear()
{
	this->queue.Clear();
}

} // namespace ResourceBrowser