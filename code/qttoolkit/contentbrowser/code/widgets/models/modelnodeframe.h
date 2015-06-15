#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::ModelNodeFrame
    
    Overrides QFrame to implement model-node specific stuff
    
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QFrame>
#include "modelnodehandler.h"
#include "ui_modelnodeinfowidget.h"
namespace Widgets
{
class ModelNodeFrame : 
	public QFrame
{
public:
	/// constructor
	ModelNodeFrame();
	/// destructor
	virtual ~ModelNodeFrame();

	/// returns pointer to handler
	const Ptr<ModelNodeHandler>& GetHandler() const;
	/// discards a model node frame
	void Discard();
private:
	Ptr<ModelNodeHandler> itemHandler;
	Ui::ModelNodeInfoWidget ui;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ModelNodeHandler>& 
ModelNodeFrame::GetHandler() const
{
	return this->itemHandler;
}
} // namespace Widgets
//------------------------------------------------------------------------------