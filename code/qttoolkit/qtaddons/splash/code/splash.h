#pragma once
//------------------------------------------------------------------------------
/**
	@class Splash::SplashWindow
	
	Opens a splash window which displays an icon and application name.
	
	(C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include <QDialog>
#include <QPixmap>

namespace Ui
{
	class SplashDialog;
}

namespace SplashAddon
{
class Splash : public Core::RefCounted
{
	__DeclareClass(Splash);
public:
	/// constructor
	Splash();
	/// destructor
	virtual ~Splash();

	/// open splash
	void Open();
	/// close splash
	void Close();

	/// sets the title of the splash
	void SetTitle(const QString& title);
	/// sets the icon to use by the splash
	void SetIcon(const QPixmap& icon);

private:
	bool open;
	QString title;
	QPixmap icon;
	Ui::SplashDialog* ui;
	QDialog dialog;
};

//------------------------------------------------------------------------------
/**
*/
inline void
Splash::SetTitle(const QString& title)
{
	this->title = title;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Splash::SetIcon(const QPixmap& icon)
{
	this->icon = icon;
}

} // namespace Splash