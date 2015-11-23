#pragma once
//------------------------------------------------------------------------------
/**
	@class ResourceBrowser::TextureLoaderThread
	
	Asynchronous image icon loader.
	
	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QThread>
#include <QImage>
#include <QMutex>
#include "threading/safequeue.h"
#include "util/string.h"
namespace ResourceBrowser
{

class ImageLoaderUnit : public QObject
{
	Q_OBJECT
public:
	QMutex mutex;
	Util::String path;
	QImage* texture;

signals:
	/// signal when loaded
	void OnLoaded();

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
};

//------------------------------------------------------------------------------
/**
*/
inline void
ImageLoaderThread::Enqueue(ImageLoaderUnit* unit)
{
	this->queue.Enqueue(unit);
}

} // namespace ResourceBrowser