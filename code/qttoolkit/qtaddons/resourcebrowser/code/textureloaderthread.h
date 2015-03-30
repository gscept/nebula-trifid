#pragma once
//------------------------------------------------------------------------------
/**
	@class ResourceBrowser::TextureLoaderThread
	
	Asynchronous texture loader.
	
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

class TextureLoaderUnit : public QObject
{
	Q_OBJECT
public:
	QMutex mutex;
	Util::String path;
	QImage* texture;

signals:
	/// signal when loaded
	void OnLoaded();

	friend class TextureLoaderThread;
};

class TextureLoaderThread : public QThread
{
public:
	/// constructor
	TextureLoaderThread(QObject* parent);
	/// destructor
	virtual ~TextureLoaderThread();

	/// run thread
	void run();

	/// start thread
	void Start();
	/// stop thread
	void Stop();
	/// pause/unpause thread
	void Pause(bool b);
	/// enqueue new texture
	void Enqueue(TextureLoaderUnit* unit);	

	/// clear any pending requests
	void Clear();

private:
	bool paused;
	bool shouldStop;
	Threading::SafeQueue<TextureLoaderUnit*> queue;
};

//------------------------------------------------------------------------------
/**
*/
inline void
TextureLoaderThread::Enqueue(TextureLoaderUnit* unit)
{
	this->queue.Enqueue(unit);
}

} // namespace ResourceBrowser