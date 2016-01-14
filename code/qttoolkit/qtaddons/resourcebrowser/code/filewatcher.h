#pragma once
//------------------------------------------------------------------------------
/**
	Implements a QFileSystemWatcher which noticed if files are created.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QFileSystemWatcher>
#include "util/string.h"
#include "util/array.h"
namespace ResourceBrowser
{
class FileWatcher : public QFileSystemWatcher
{
	Q_OBJECT
public:
	/// constructor
	FileWatcher();
	/// destructor
	virtual ~FileWatcher();

	/// add file, if it doesn't exist, this watcher will watch the folder and report when it gets created
	void WatchFile(const Util::String& file);
	/// add directory to watch
	void WatchDirectory(const Util::String& dir);
public slots:
	/// emitted by myself when the folder changed
	void FolderChanged(const QString& folder);

private:
	Util::Array<Util::String> files;
};
} // namespace ResourceBrowser