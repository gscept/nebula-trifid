#pragma once
//------------------------------------------------------------------------------
/**
	@class ResourceBrowser::TextureLoaderThread
	
	Asynchronous image icon loader.
	
	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "filewatcher.h"
#include "threading/safequeue.h"
#include "util/string.h"

#include <QThread>
#include <QImage>
#include <QMutex>
#include <QPixmap>
namespace ResourceBrowser
{

class ImageLoaderUnit : public QObject
{
	Q_OBJECT
public:
	QMutex* mutex;
	Util::String path;
	QImage* texture;
	FileWatcher thumbnailWatcher;
	int refCount;

	/// constructor
	ImageLoaderUnit() : refCount(1) 
	{
		connect(&this->thumbnailWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(OnThumbnailFileChanged()));
		this->texture = new QImage;
		this->mutex = new QMutex;
	}

	/// destructor
	~ImageLoaderUnit()
	{
		disconnect(&this->thumbnailWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(OnThumbnailFileChanged()));
		delete this->texture;
		delete this->mutex;
	}

	/// bump a ref
	void Retain();
	/// release a ref
	void Release();

	/// perform actual loading
	void Load();

signals:
	/// signal when loaded
	void OnLoaded();

private slots:
	/// signal when thumbnail has changed
	void OnThumbnailFileChanged();

private:

	friend class ImageLoaderThread;
};

class ImageLoaderThread : public QThread
{
public:
	/// constructor
	ImageLoaderThread(QObject* parent);
	/// destructor
	virtual ~ImageLoaderThread();

	/// run thread
	void run();

	/// start thread
	void Start();
	/// stop thread
	void Stop();
	/// pause/unpause thread
	void Pause(bool b);
	/// enqueue new texture
	void Enqueue(ImageLoaderUnit* unit);	

	/// clear any pending requests
	void Clear();

private:
	bool paused;
	bool shouldStop;
	Threading::SafeQueue<ImageLoaderUnit*> queue;

	friend class ImageLoaderUnit;
	static QPixmap* NoThumbnailPixmap;
};

//------------------------------------------------------------------------------
/**
*/
inline void
ImageLoaderThread::Enqueue(ImageLoaderUnit* unit)
{
	unit->Retain();
	this->queue.Enqueue(unit);
}

//------------------------------------------------------------------------------
/**
*/
inline void
ImageLoaderUnit::Retain()
{
	this->mutex->lock();
	this->refCount++;
	this->mutex->unlock();
}

//------------------------------------------------------------------------------
/**
*/
inline void
ImageLoaderUnit::Release()
{
	this->mutex->lock();
	this->refCount--;
	this->mutex->unlock();
	if (this->refCount == 0)
	{
		delete this;
	}
}

} // namespace ResourceBrowser