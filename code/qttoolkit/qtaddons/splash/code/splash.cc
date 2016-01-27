//------------------------------------------------------------------------------
//  splash.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "splash.h"
#include "ui_splashdialog.h"

namespace SplashAddon
{
__ImplementClass(SplashAddon::Splash, 'SPLS', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Splash::Splash() :
	open(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Splash::~Splash()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
Splash::Open()
{
	n_assert(!this->open);
	this->ui = new Ui::SplashDialog();
	this->dialog.setWindowFlags(Qt::FramelessWindowHint);
	this->dialog.setModal(true);
	this->dialog.setWindowModality(Qt::ApplicationModal);
	this->ui->setupUi(&this->dialog);
	this->ui->titleLabel->setText(this->title);
	this->ui->iconLabel->setPixmap(this->icon);
	this->dialog.show();
	QApplication::processEvents();
	this->open = true;
}

//------------------------------------------------------------------------------
/**
*/
void
Splash::Close()
{
	n_assert(this->open);
	this->dialog.close();
	delete this->ui;
	this->open = false;
}



} // namespace SplashAddon