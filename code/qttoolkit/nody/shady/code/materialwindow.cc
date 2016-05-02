//------------------------------------------------------------------------------
// materialwindow.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "materialwindow.h"

namespace Shady
{

__ImplementClass(Shady::MaterialWindow, 'MTWI', Core::RefCounted);
//------------------------------------------------------------------------------
/**
*/
MaterialWindow::MaterialWindow()
{
	this->ui.setupUi(&this->contents);
	this->layout.addWidget(&this->contents);
	this->setLayout(&this->layout);
}

//------------------------------------------------------------------------------
/**
*/
MaterialWindow::~MaterialWindow()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialWindow::Setup()
{

}

} // namespace Shady