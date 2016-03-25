#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::LevelEditorState
    
    Base state for leveleditor window
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include "basegamefeature/statehandlers/gamestatehandler.h"
#include "graphics/globallightentity.h"
#include "graphics/pointlightentity.h"
#include "graphics/modelentity.h"
#include "game/entity.h"
#include "actions/actionmanager.h"
#include "actions/transformaction.h"
#include "dynui/console/imguiconsole.h"
#include "dynui/console/imguiconsolehandler.h"
#include "picking/pickingserver.h"


//------------------------------------------------------------------------------
namespace LevelEditor2
{
class SelectionUtil;
class PlacementUtil;

class LevelEditorState : public BaseGameFeature::GameStateHandler
{
	__DeclareClass(LevelEditorState);		
public:
	/// constructor
	LevelEditorState();
	/// destructor
	virtual ~LevelEditorState();

	/// override on state enter
	void OnStateEnter(const Util::String& prevState);
	/// override on state leave
	void OnStateLeave(const Util::String& nextState);
	/// override on frame
	Util::String OnFrame();

	/// set the current selected entities
	void UpdateSelection(const Util::Array<EntityGuid>& entityIds);	
	/// clear selection, without undo. for use when creating a new level
	void ClearSelection();
	///
	void TogglePerformanceOverlay();
	

private:
	friend class ActionManager;
	friend class EntityTreeWidget;	//< hack-ish, the treewidget must be able to call HandleSelection in order to sync entity selections

	/// handle input
	void HandleInput();
	/// checks the selection utility and notify the placement util
	void HandleSelection();

	/// checks the placement utility and updates selected entities.
	void HandlePlacementChanges();
	
	Ptr<LevelEditor2::SelectionUtil> selectionUtil;
	Ptr<LevelEditor2::PlacementUtil> placementUtil;

	Ptr<Dynui::ImguiConsole> console;
	Ptr<Dynui::ImguiConsoleHandler> consoleHandler;
	Ptr<Game::Entity> defaultCam;	
	Ptr<Game::Entity> fpsCam;
	Ptr<TransformAction> activeTransformAction;
	Ptr<Picking::PickingServer> pickingServer;
	Input::Key::Code activateSelectionZoom;

	Timing::Time lastFrameTime;
	Timing::Time lastUpdateFpsTime;
	bool showPerformance;
	int performanceFrame;
	Util::RingBuffer<int> drawcallBuffer;
	Util::RingBuffer<int> primitivesBuffer;
	Util::RingBuffer<int> computesBuffer;
}; 

} // namespace LevelEditor2
//------------------------------------------------------------------------------