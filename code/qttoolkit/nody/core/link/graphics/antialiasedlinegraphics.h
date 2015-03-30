#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::AntialiasedLineGraphics
    
    Implements a special type of line which uses anti-aliasing.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QGraphicsPathItem>
namespace Nody
{
class AntialiasedLineGraphics : public QGraphicsPathItem
{
public:
	/// constructor
	AntialiasedLineGraphics();
	/// destructor
	virtual ~AntialiasedLineGraphics();

	/// paint
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
}; 
} // namespace Nody
//------------------------------------------------------------------------------