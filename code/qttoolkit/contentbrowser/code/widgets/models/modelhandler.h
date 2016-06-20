#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::ModelItemHandler
    
    Handles model items.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/basehandler.h"
#include <QObject>
#include "qtreewidget.h"
#include "materials/material.h"
#include "n3util/n3xmlmodifier.h"
#include "actions/modelmodifyaction.h"
#include "modelutil/modelattributes.h"
#include "modelutil/modelconstants.h"
#include "modelutil/modelphysics.h"
#include "ui_modelinfowidget.h"
#include "messaging/message.h"
#include "code/filewatcher.h"

namespace ContentBrowser
{
	class ContentBrowserWindow;
}

namespace Widgets
{
class CharacterNodeFrame;
class ParticleNodeFrame;
class ModelNodeFrame;
class PhysicsNodeFrame;
class ModelHandler : public BaseHandler
{
	Q_OBJECT
	__DeclareClass(ModelHandler);
public:
	/// constructor
	ModelHandler();
	/// destructor
	virtual ~ModelHandler();

    /// cleanup the handler
    void Cleanup();

	/// sets the ui
	void SetUI(Ui::ModelInfoWidget* ui);
	/// gets the ui
	Ui::ModelInfoWidget* GetUI() const;

	/// sets the resource
	void SetModelResource(const Util::String& model);
	/// gets the resource
	const Util::String& GetModelResource() const;
	/// sets the model category
	void SetModelCategory(const Util::String& category);
	/// gets the model category
	const Util::String& GetModelCategory() const;

	/// return pointer to attributes
	const Ptr<ToolkitUtil::ModelAttributes>& GetAttributes() const;
	/// return pointer to constants
	const Ptr<ToolkitUtil::ModelConstants>& GetConstants() const;
	/// return pointer to physics
	const Ptr<ToolkitUtil::ModelPhysics>& GetPhysics() const;

	/// calls the preview state to preview the selected model
	bool Preview();
	/// populates the tree widget with the contents of the model
	void Setup();
	/// discards the model item handler, this should be called when a the current item handler has been removed
	bool Discard();
	/// discards the model item handler without the option to cancel
	void DiscardNoCancel();
	/// mute the UI by disabling it
	void Mute();

	/// refresh after redo/undo action
	void Refresh();

	/// use constants, physics and attributes to make a new model (intermediate) and have it reloaded
	void MakeModel();

	/// called whenever a model is reloaded
	void OnModelReloaded( const Ptr<Messaging::Message>& msg);
	/// saves the current state of the model to memory, so that we may undo/redo the changes
	void OnModelModified(bool structureChange = false);
	/// saves this current state as a new version
	void OnNewVersion();

	/// handles on-frame event
	void OnFrame();

signals:
	/// called whenever a model has been saved with a new name
	void ModelSavedAs(const Util::String& res);

public slots:

	/// removes a particle node with a name, node is not const ref because we will remove the item handler!
	void RemoveParticleNode(const Util::String path, const Util::String node);

private slots:
	friend class ContentBrowser::ContentBrowserWindow;

	/// add new particle node to model
	void OnAddParticleNode();
	/// saves changes 
	void OnSave();
	/// save changes as another file
	void OnSaveAs();
	/// reimport model
	void OnReconfigure();

private:

	/// setup or remake tabs
	void SetupTabs();
	/// update model thumbnail icon
	void UpdateModelThumbnail();

	Ptr<Actions::ModelModifyAction> action;
	Util::String file;
	Util::String category;
	Ui::ModelInfoWidget* ui;
	Util::Array<ModelNodeFrame*> nodeFrames;
	Util::Array<ParticleNodeFrame*> particleFrames;
	Util::Array<PhysicsNodeFrame*> physicsFrames;
	CharacterNodeFrame* characterFrame;
	Ptr<ToolkitUtil::ModelAttributes> attributes;
	Ptr<ToolkitUtil::ModelConstants> constants;
	Ptr<ToolkitUtil::ModelPhysics> physics;
	ResourceBrowser::FileWatcher thumbnailWatcher;

	QMenu* saveMenu;
	QAction* saveAction;
	QAction* saveAsAction;
	QAction* reconfigAction;
	QAction* particleNodeAction;
	QIcon savedIcon;
	QIcon unsavedIcon;
	QIcon blankIcon;
	QString savedStyle;
	QString unsavedStyle;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
ModelHandler::SetUI(Ui::ModelInfoWidget* ui)
{
	n_assert(ui);
	this->ui = ui;
}

//------------------------------------------------------------------------------
/**
*/
inline Ui::ModelInfoWidget* 
ModelHandler::GetUI() const
{
	return this->ui;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ModelHandler::SetModelResource(const Util::String& model)
{
	n_assert(model.IsValid());
	this->file = model;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
ModelHandler::GetModelResource() const
{
	return this->file;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ModelHandler::SetModelCategory( const Util::String& category )
{
	n_assert(category.IsValid());
	this->category = category;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
ModelHandler::GetModelCategory() const
{
	return this->category;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ToolkitUtil::ModelAttributes>& 
ModelHandler::GetAttributes() const
{
	return this->attributes;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ToolkitUtil::ModelConstants>& 
ModelHandler::GetConstants() const
{
	return this->constants;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ToolkitUtil::ModelPhysics>& 
ModelHandler::GetPhysics() const
{
	return this->physics;
}

} // namespace Widgets
//------------------------------------------------------------------------------