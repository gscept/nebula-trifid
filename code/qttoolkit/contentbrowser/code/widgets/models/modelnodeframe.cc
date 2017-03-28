//------------------------------------------------------------------------------
//  modelnodeframe.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "modelnodeframe.h"

namespace Widgets
{
//------------------------------------------------------------------------------
/**
*/
ModelNodeFrame::ModelNodeFrame()
{
	this->setLayout(&this->layout);
	this->layout.setContentsMargins(0, 2, 0, 0);
	this->layout.addStretch();
}

//------------------------------------------------------------------------------
/**
*/
ModelNodeFrame::~ModelNodeFrame()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeFrame::AddModelNode(const Util::String& name, const Util::String& path, const Util::String& res)
{
	Ptr<ModelNodeHandler> handler = ModelNodeHandler::Create();
	handler->SetModelHandler(this->modelHandler);
	Ui::ModelNodeInfoWidget* ui = new Ui::ModelNodeInfoWidget;
	QFrame* frame = new QFrame;
	ui->setupUi(frame);
	handler->SetUI(ui);
	handler->SetName(name);
	handler->SetPath(path);
	handler->Setup(res);
	this->layout.insertWidget(0, frame);
	this->itemHandlers.Append(handler);
	this->itemFrames.Append(frame);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeFrame::Discard()
{
	this->modelHandler = 0;
	IndexT i;
	for (i = 0; i < this->itemHandlers.Size(); i++)
	{
		this->itemHandlers[i]->Discard();
		this->layout.removeWidget(this->itemFrames[i]);
		delete this->itemFrames[i];
	}
	this->itemHandlers.Clear();
	this->itemFrames.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeFrame::Refresh()
{
	IndexT i;
	for (i = 0; i < this->itemHandlers.Size(); i++)
	{
		this->itemHandlers[i]->Refresh();
	}
}

} // namespace Widgets