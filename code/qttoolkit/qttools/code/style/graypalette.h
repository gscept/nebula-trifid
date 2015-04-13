#pragma once
//------------------------------------------------------------------------------
/**
    @class QtToolkitUtil::GrayPalette
    
    Implements a gray palette.
    
    (C) 2012-2015 Gustav Sterbrant
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

} // namespace QtToolkitUtil
//------------------------------------------------------------------------------