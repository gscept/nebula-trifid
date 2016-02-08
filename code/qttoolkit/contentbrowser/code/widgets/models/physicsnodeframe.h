#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::PhysicsNodeFrame
    
    Implements a frame specific for physics nodes.
    
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QFrame>
#include "physicsnodehandler.h"
#include "ui_physicsnodeinfowidget.h"
namespace Widgets
{
class PhysicsNodeFrame : public QFrame
{
public:
	/// constructor
	PhysicsNodeFrame();
	/// destructor
	virtual ~PhysicsNodeFrame();

	/// set the model handler to which this physics node frame should operate on, do this prior to adding model nodes
	void SetModelHandler(const Ptr<PhysicsNodeHandler>& handler);
	/// returns pointer to handler
	const Ptr<PhysicsNodeHandler>& GetHandler() const;
	/// discards a model node frame
	void Discard();
private:
	Ptr<PhysicsNodeHandler> itemHandler;
	Ui::PhysicsNodeInfoWidget ui;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
PhysicsNodeFrame::SetModelHandler(const Ptr<PhysicsNodeHandler>& handler)
{
	this->itemHandler = handler;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<PhysicsNodeHandler>&
PhysicsNodeFrame::GetHandler() const
{
	return this->itemHandler;
}
} // namespace Widgets
//------------------------------------------------------------------------------