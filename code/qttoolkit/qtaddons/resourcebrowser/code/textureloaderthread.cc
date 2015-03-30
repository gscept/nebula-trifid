//------------------------------------------------------------------------------
//  textureloaderthread.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "textureloaderthread.h"
#include "io/uri.h"

namespace ResourceBrowser
{

//------------------------------------------------------------------------------
/**
*/
TextureLoaderThread::TextureLoaderThread(QObject* parent) : 
	QThread(parent),
	shouldStop(false),
	paused(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
TextureLoaderThread::~TextureLoaderThread()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
TextureLoaderThread::run()
{
	// run forever
	while (!this->shouldStop)
	{
		while (!this->queue.IsEmpty() && !this->paused)
		{
			// get unit and lock it so that we don't delete it
			TextureLoaderUnit* unit = this->queue.Dequeue();
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
TextureLoaderThread::Start()
{
	this->start();
}

//------------------------------------------------------------------------------
/**
*/
void
TextureLoaderThread::Stop()
{
	this->queue.Clear();
	this->shouldStop = true;
	this->quit();
}

//------------------------------------------------------------------------------
/**
*/
void
TextureLoaderThread::Pause(bool b)
{
	this->paused = b;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureLoaderThread::Clear()
{
	this->queue.Clear();
}

} // namespace ResourceBrowser