//------------------------------------------------------------------------------
// filewatcher.cc
// (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "filewatcher.h"
#include <QFileInfo>

namespace ResourceBrowser
{

//------------------------------------------------------------------------------
/**
*/
FileWatcher::FileWatcher()
{
	connect(this, SIGNAL(directoryChanged(const QString&)), this, SLOT(FolderChanged(const QString&)));
}

//------------------------------------------------------------------------------
/**
*/
FileWatcher::~FileWatcher()
{
	disconnect(this, SIGNAL(directoryChanged(const QString&)), this, SLOT(FolderChanged(const QString&)));
}

//------------------------------------------------------------------------------
/**
*/
void
FileWatcher::WatchFile(const Util::String& file)
{
	QFileInfo info(file.AsCharPtr());
	if (info.exists())
	{
		n_assert(info.isFile());
		this->addPath(file.AsCharPtr());
	}
	else
	{
		Util::String dir = file.ExtractDirName();
		this->addPath(dir.AsCharPtr());
		this->files.Append(file);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FileWatcher::WatchDirectory(const Util::String& dir)
{
	QFileInfo info(dir.AsCharPtr());
	if (info.exists())
	{
		n_assert(info.isDir());
		this->addPath(dir.AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FileWatcher::FolderChanged(const QString& folder)
{
#ifdef __USE_QT4
	int i;
	for (i = 0; i < this->files.Size(); i++)
	{
		const Util::String& file = this->files[i];
		QFileInfo info(file.AsCharPtr());
		if (info.exists())
		{
			emit this->fileChanged(file.AsCharPtr());
			this->addPath(file.AsCharPtr());
			this->files.EraseIndex(i--);
		}		
	}
#else
    // FIXME what is this even used for? nothing connects to the signal
#endif
}

} // namespace ResourceBrowser