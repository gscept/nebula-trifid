#pragma once
//------------------------------------------------------------------------------
/**
    @class QtToolkitUtil::GrayPalette
    
    Implements a gray palette.
    
    (C) 2012-2016 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QPalette>
#include <QFont>
namespace QtToolkitUtil
{
class NebulaStyleTool :	public QPalette
{
public:
	/// constructor
	NebulaStyleTool();
	/// destructor
	virtual ~NebulaStyleTool();

	QString globalStyleSheet;
	QFont font;
}; 

} // namespace QtToolkitUtil
//------------------------------------------------------------------------------