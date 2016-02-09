#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::CharacterNodeHandler
    
    Handles a specific character node.
    
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "graphics/modelentity.h"
#include "ui_characternodeinfowidget.h"
#include "modelhandler.h"
#include "trackcontroller.h"

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
	/// called when the add button gets pressed
	void OnAddClip();
	/// called when the remove button gets pressed
	void OnRemoveClip();
	/// called whenever a clip is double-clicked
	void OnPlayClip();
	/// called whenever a playing clip is double-clicked
	void OnPauseClip();
	/// called whenever a clip gets stopped
	void OnStopClip();
	/// called whenever the clip seek slider is pressed
	void OnClipSeek(int time);
	/// called whenever a clip has been double clicked
	void OnClipDoubleClicked(QListWidgetItem* item);

	/// called whenever a weight has changed
	void OnWeightChanged(IndexT index);
	/// called whenever a skin is double-clicked
	void OnSkinActivated();
	/// called whenever a visible skin is double-clicked
	void OnSkinDeactivated();
	/// called whenever the show skin button is pressed
	void OnShowSkin();
	/// called whenever the hide skin button is pressed
	void OnHideSkin();
	/// called whenever the render skeleton check box gets checked
	void OnRenderSkeleton(bool b);

	/// called when the skin list is fetched
	void OnFetchedSkinList(const Ptr<Messaging::Message>& msg);
	/// called when the clip list is fetched
	void OnFetchedClipList(const Ptr<Messaging::Message>& msg);

	/// creates a new mask and selects it
	void OnNewMask();
	/// deletes the mask currently selected
	void OnDeleteMask();
	/// called whenever the mask name is changed, this will change the name of the current mask
	void OnMaskNameChanged(const QString& value);
	/// handle a mask getting selected
	void OnMaskSelected(int index);

private:

	friend class ModelHandler;

	/// helper function which toggles all the currently visible skins to be shown again.
	/// this is useful if we reload the model and are showing more skins than one. 
	void ReshowSkins();

	Util::String nodeName;
	Ptr<ModelHandler> itemHandler;
	Ui::CharacterNodeInfoWidget* ui;
	Ptr<Graphics::ModelEntity> model;
	Util::Array<Util::StringAtom> skins;
	Util::Array<Util::StringAtom> clips;
	Util::Array<SizeT> durations;
    Util::FixedArray<SizeT> currentDurations;
	QListWidgetItem* currentClipItem;
	TrackController* trackController;
	SizeT currentSeekInterval;

	Util::FixedArray<bool> skinVisible;
	Util::FixedArray<bool> clipsPlaying;
	Util::FixedArray<int> clipsMask;
	Util::FixedArray<float> clipWeights;
	Util::FixedArray<Characters::CharacterJoint> joints;
	int selectedJoint;
	int numFrames;
	int currentFrame;
	bool paused;

	IndexT currentMaskIndex;
	Util::Array<ToolkitUtil::JointMask> masks;
	ToolkitUtil::JointMask* currentMask;
	Util::Array<Characters::CharacterJointMask*> characterJointMasks;
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
CharacterNodeHandler::SetItemHandler( const Ptr<ModelHandler>& itemHandler )
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