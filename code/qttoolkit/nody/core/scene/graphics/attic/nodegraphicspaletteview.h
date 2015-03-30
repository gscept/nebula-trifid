#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::NodeGraphicsPaletteView
    
    Implements the graphics palette view
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <QGraphicsView>
#include "core/ptr.h"
#include "nodegraphicspalette.h"

namespace Nody
{
class NodeGraphicsPaletteView : public QGraphicsView
{
	Q_OBJECT
public:
	/// constructor
	NodeGraphicsPaletteView(QWidget* parent);
	/// destructor
	virtual ~NodeGraphicsPaletteView();
	
	/// returns pointer to graphics palette
	const Ptr<NodeGraphicsPalette> GetScene() const;

private:

	Ptr<NodeGraphicsPalette> graphicsPalette;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const Ptr<NodeGraphicsPalette> 
NodeGraphicsPaletteView::GetScene() const
{
	return this->graphicsPalette;
}
} // namespace Nody
//------------------------------------------------------------------------------