#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::ModelNodeFrame
    
    Overrides QFrame to implement model-node specific stuff
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "modelnodehandler.h"
#include "ui_modelnodeinfowidget.h"

#include <QFrame>
#include <QVBoxLayout>
namespace Widgets
{
class ModelNodeFrame : public QFrame
{
public:
	/// constructor
	ModelNodeFrame();
	/// destructor
	virtual ~ModelNodeFrame();

	/// add a new model node to the frame
	void AddModelNode(const Util::String& name, const Util::String& path, const Util::String& res);

	/// set the model handler to which this model node frame should operate on, do this prior to adding model nodes
	void SetModelHandler(const Ptr<ModelHandler>& handler);
	/// returns pointer to handler
	const Ptr<ModelHandler>& GetModelHandler() const;

	/// discards a model node frame
	void Discard();
	/// refresh model node frames
	void Refresh();
private:
	Util::Array<Ptr<ModelNodeHandler>> itemHandlers;
	Util::Array<QFrame*> itemFrames;
	QVBoxLayout layout;
	Ptr<ModelHandler> modelHandler;
}; 


//------------------------------------------------------------------------------
/**
*/
inline void
ModelNodeFrame::SetModelHandler(const Ptr<ModelHandler>& handler)
{
	this->modelHandler = handler;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ModelHandler>&
ModelNodeFrame::GetModelHandler() const
{
	return this->modelHandler;
}
} // namespace Widgets
//------------------------------------------------------------------------------