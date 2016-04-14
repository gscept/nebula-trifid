#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::ProgressReporter
    
    Singleton which handles progress reporting through a QProgressBar and QLabel
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/singleton.h"
#include "core/refcounted.h"
#include <QtGui/QProgressBar>
#include <QtGui/QLabel>
#include "util/string.h"

namespace ContentBrowser
{
class ProgressReporter : public Core::RefCounted
{
	__DeclareSingleton(ProgressReporter);
	__DeclareClass(ProgressReporter);
public:
	/// constructor
	ProgressReporter();
	/// destructor
	virtual ~ProgressReporter();
	/// open the ProgressReporter
	bool Open();
	/// close the ProgressReporter
	void Close();
	/// return if ProgressReporter is open
	bool IsOpen() const;

	/// sets the QProgressBar
	void SetProgressBar(QProgressBar* progressBar);
	/// sets the QLabel
	void SetStatusLabel(QLabel* statusLabel);

	/// begins progress reporting
	void BeginProgressReporting();
	/// ends progress reporting
	void EndProgressReporting();
	// returns true if progress is being reported
	const bool IsReporting() const;

	/// static progress callback
	static void ReportProgress(float progress, const Util::String& status);
	/// static more handy progress callback
	static void ReportProgress(const Util::String& status);
	/// static progress bar min-max
	static void SetupProgress(int min, int max);
private:
	QProgressBar* progressBar;
	QLabel* statusLabel;
	bool isOpen;
	bool isReporting;
}; 

//------------------------------------------------------------------------------
/**
*/
inline bool
ProgressReporter::IsOpen() const
{
	return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ProgressReporter::SetProgressBar( QProgressBar* progressBar )
{
	n_assert(0 != progressBar);
	this->progressBar = progressBar;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ProgressReporter::SetStatusLabel( QLabel* statusLabel )
{
	n_assert(0 != statusLabel);
	this->statusLabel = statusLabel;
}


//------------------------------------------------------------------------------
/**
*/
inline const bool
ProgressReporter::IsReporting() const
{
	return this->isReporting;
}

} // namespace ContentBrowser
//------------------------------------------------------------------------------