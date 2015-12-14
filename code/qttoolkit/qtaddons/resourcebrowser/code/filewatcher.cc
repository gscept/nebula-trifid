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
FileWatcher::FolderChanged(const QString& folder)
{
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
}

} // namespace ResourceBrowser