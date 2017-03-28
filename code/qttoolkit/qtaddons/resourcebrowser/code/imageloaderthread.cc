//------------------------------------------------------------------------------
//  textureloaderthread.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "imageloaderthread.h"
#include "io/uri.h"

#include <QPainter>

namespace ResourceBrowser
{


QPixmap* ImageLoaderThread::NoThumbnailPixmap = NULL;
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
			unit->Load();
			unit->Release();
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
	ImageLoaderThread::NoThumbnailPixmap = new QPixmap(48, 48);
	ImageLoaderThread::NoThumbnailPixmap->fill(Qt::transparent);
	QPainter painter(ImageLoaderThread::NoThumbnailPixmap);
	painter.setFont(QFont("Tahoma", 6, QFont::Bold));
	painter.setPen(Qt::red);
	painter.setRenderHint(QPainter::TextAntialiasing, false);
	painter.drawText(QRectF(0, 0, 48, 48), Qt::AlignCenter, "No thumbnail");
}

//------------------------------------------------------------------------------
/**
*/
void
ImageLoaderThread::Stop()
{
	delete ImageLoaderThread::NoThumbnailPixmap;
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

//------------------------------------------------------------------------------
/**
*/
void
ImageLoaderUnit::Load()
{
	// lock mutex
	this->mutex->lock();

	// get parameters
	Util::String path = this->path;
	QImage* image = this->texture;

	// if the image has been deleted, we can skip this item
	if (image == NULL) return;

	// convert to absolute path
	IO::URI uri(path);

	// load image and scale
	QImage localImage(uri.LocalPath().AsCharPtr());
	if (!localImage.isNull()) *image = localImage.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	else                      *image = ImageLoaderThread::NoThumbnailPixmap->toImage();
	this->thumbnailWatcher.WatchFile(uri.LocalPath());

	// emit signal to notify the image has been loaded
	emit this->OnLoaded();

	// unlock mutex
	this->mutex->unlock();
}

//------------------------------------------------------------------------------
/**
*/
void
ImageLoaderUnit::OnThumbnailFileChanged()
{
	this->Load();
}

} // namespace ResourceBrowser