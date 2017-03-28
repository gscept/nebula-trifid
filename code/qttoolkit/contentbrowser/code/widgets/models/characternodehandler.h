#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::CharacterNodeHandler
    
    Handles a specific character node.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "graphics/modelentity.h"
#include "ui_characternodeinfowidget.h"
#include "modelhandler.h"

#include <QTimer>
namespace Widgets
{
class CharacterNodeHandler : 
		public QObject,
		public Core::RefCounted
{
	Q_OBJECT
	__DeclareClass(CharacterNodeHandler);
public:
	/// constructor
	CharacterNodeHandler();
	/// destructor
	virtual ~CharacterNodeHandler();

	/// sets the ui
	void SetUI(Ui::CharacterNodeInfoWidget* ui);
	/// gets the ui
	Ui::CharacterNodeInfoWidget* GetUI() const;

	/// set pointer to original item handler
	void SetItemHandler(const Ptr<ModelHandler>& itemHandler);
	/// get pointer to original item handler
	const Ptr<ModelHandler>& GetItemHandler() const;

	/// sets the node name of the handler
	void SetName(const Util::String& name);
	/// gets the node name of the handler
	const Util::String GetName() const;

	/// constructs the internal structure for the model node item
	void Setup();
	/// discards a model node handler
	void Discard();


	/// render viewport handles
	void OnFrame();

private slots:

	/// called whenever the render skeleton check box gets checked
	void OnRenderSkeleton(bool b);

	/// handle joint row double click
	void OnJointRowClicked(QTreeWidgetItem* item, int column);
	/// handle joint row right click
	void OnJointRowItemContextMenu(const QPoint& point);

	/// called when the skin list is fetched
	void OnFetchedSkinList(const Ptr<Messaging::Message>& msg);
	/// called when the clip list is fetched
	void OnFetchedClipList(const Ptr<Messaging::Message>& msg);

	/// creates a new mask and selects it
	void OnNewMask();
	/// deletes the mask currently selected
	void OnDeleteMask();
	/// called whenever the mask name is changed, this will change the name of the current mask
	void OnMaskNameChanged();
	/// handle a mask getting selected
	void OnMaskSelected(int index);

private:

	friend class ModelHandler;

	/// helper function which toggles all the currently visible skins to be shown again.
	/// this is useful if we reload the model and are showing more skins than one. 
	void ReshowSkins();

	/// helper function for setting up joint hierarchy
	void SetupJointHierarchy();
	/// helper function to update joint weights
	void SetupJointWeights();
	/// helper function to recursively flood joint mask downwards
	void RecurseFloodJointMaskDown(QTreeWidgetItem* item, float value);
	/// helper function to recursively flood joint masks upwards
	void RecurseFloodJointMaskUp(QTreeWidgetItem* item, float value);

	Util::String nodeName;
	Ptr<ModelHandler> itemHandler;
	Ui::CharacterNodeInfoWidget* ui;
	Ptr<Graphics::ModelEntity> model;
	Util::Array<Util::StringAtom> skins;
	Util::Array<Util::StringAtom> clips;
	Util::Array<SizeT> durations;
    Util::FixedArray<SizeT> currentDurations;

	Util::FixedArray<bool> skinVisible;
	Util::FixedArray<bool> clipsPlaying;
	Util::FixedArray<int> clipsMask;
	Util::FixedArray<float> clipWeights;
	Util::FixedArray<Characters::CharacterJoint> joints;
	Characters::CharacterSkeleton* skeleton;
	int selectedJoint;
	int numFrames;
	int currentFrame;
	bool paused;

	IndexT currentMaskIndex;
	Util::Array<ToolkitUtil::JointMask> masks;
	ToolkitUtil::JointMask* currentMask;
	Util::Array<Characters::CharacterJointMask*> characterJointMasks;
	Util::Dictionary<int, QTreeWidgetItem*> jointIndexToItemMap;
	int selectedMask;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
CharacterNodeHandler::SetUI(Ui::CharacterNodeInfoWidget* ui)
{
	n_assert(ui);
	this->ui = ui;
}

//------------------------------------------------------------------------------
/**
*/
inline Ui::CharacterNodeInfoWidget* 
CharacterNodeHandler::GetUI() const
{
	return this->ui;
}

//------------------------------------------------------------------------------
/**
*/
inline void
CharacterNodeHandler::SetName(const Util::String& name)
{
	n_assert(name.IsValid());
	this->nodeName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String 
CharacterNodeHandler::GetName() const
{
	return this->nodeName;
}

//------------------------------------------------------------------------------
/**
*/
inline void
CharacterNodeHandler::SetItemHandler(const Ptr<ModelHandler>& itemHandler)
{
	n_assert(itemHandler.isvalid());
	this->itemHandler = itemHandler;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ModelHandler>& 
CharacterNodeHandler::GetItemHandler() const
{
	return this->itemHandler;
}


} // namespace Widgets
//------------------------------------------------------------------------------