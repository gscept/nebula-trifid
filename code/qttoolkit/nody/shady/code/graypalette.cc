//------------------------------------------------------------------------------
//  graypalette.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graypalette.h"

//------------------------------------------------------------------------------
/**
*/
GrayPalette::GrayPalette()
{
	this->setBrush(QPalette::Window, QBrush(qRgb(80, 80, 80)));
	this->setBrush(QPalette::Base, QBrush(qRgb(90, 90, 90)));
	this->setBrush(QPalette::Button, QBrush(qRgb(90, 90, 90)));
	this->setBrush(QPalette::ButtonText, QBrush(Qt::lightGray));
	this->setBrush(QPalette::WindowText, QBrush(Qt::lightGray));
	this->setBrush(QPalette::Text, QBrush(Qt::lightGray));
	this->setBrush(QPalette::BrightText, QBrush(Qt::darkGray));
	this->setBrush(QPalette::ToolTipBase, QBrush(Qt::darkGray));
	this->setBrush(QPalette::ToolTipText, QBrush(Qt::gray));

	this->setBrush(QPalette::Disabled, QPalette::Button, QBrush(qRgb(60, 60, 60)));
	this->setBrush(QPalette::Disabled, QPalette::ButtonText, QBrush(Qt::black));
	this->setBrush(QPalette::Disabled, QPalette::WindowText, QBrush(qRgb(20, 20, 20)));
	this->setBrush(QPalette::Disabled, QPalette::Text, QBrush(qRgb(20, 20, 20)));
	this->setBrush(QPalette::Disabled, QPalette::BrightText, QBrush(Qt::lightGray));
	this->setBrush(QPalette::Disabled, QPalette::Shadow, QBrush(Qt::black));

	this->setBrush(QPalette::Highlight, QBrush(qRgb(210, 105, 30))); // orange	
	this->setBrush(QPalette::Mid, QBrush(qRgb(80, 80, 80)));
	this->setBrush(QPalette::Light, QBrush(qRgb(120, 120, 120)));
	this->setBrush(QPalette::Midlight, QBrush(qRgb(100, 100, 100)));
	this->setBrush(QPalette::Dark, QBrush(qRgb(20, 20, 20)));
	this->setBrush(QPalette::Shadow, QBrush(Qt::black));
}

//------------------------------------------------------------------------------
/**
*/
GrayPalette::~GrayPalette()
{
	// empty
}
