#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::GrayPalette
    
    Implements a gray palette.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QPalette>
namespace QtToolkitUtil
{
class GrayPalette :	public QPalette
{
public:
	/// constructor
	GrayPalette();
	/// destructor
	virtual ~GrayPalette();
}; 
} // namespace ContentBrowser
//------------------------------------------------------------------------------