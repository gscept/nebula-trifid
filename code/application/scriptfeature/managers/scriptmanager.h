#pragma once
//------------------------------------------------------------------------------
/**
    @class Script::ScriptManager
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "game/manager.h"
#include "core/singleton.h"
#include "db/reader.h"
#include "util/stringatom.h"
#include "scriptfeature/log/infolog.h"

namespace Actions
{
    class Action;
    class ActionList;
    class SequenceAction;
}
namespace Conditions
{
    class Condition;
}
namespace Game
{
    class Entity;
}
namespace FSM
{
    class StateMachine;
    class State;
    class Transition;
}

//------------------------------------------------------------------------------
namespace Script
{
class ScriptManager : public Game::Manager
{
    __DeclareClass(ScriptManager);
    __DeclareSingleton(ScriptManager);
public:
    /// constructor
    ScriptManager();
    /// destructor
    virtual ~ScriptManager();
    /// called when attached to game server
    virtual void OnActivate();
    /// called when removed from game server
    virtual void OnDeactivate();

    /// load a list of canscripts and build a condition block
    Ptr<Conditions::Condition> LoadConditionScripts(const Util::Array<Util::String>& scripts, bool combineWithOr=false, const Ptr<Script::InfoLog>& infoLog = 0);
    /// load a list of actionscripts and build an array of actions
    Util::Array<Ptr<Actions::Action> > LoadActionScripts(const Util::Array<Util::String>& scripts, const Ptr<Script::InfoLog>& infoLog = 0);
    /// load a statement block
    Ptr<Actions::ActionList> LoadStatement(const Util::Guid& blockGuid, const Util::Guid& startId, const Util::String& table = "_Scripts_Statements");
    /// load condition
    Ptr<Conditions::Condition> LoadCondition(const Util::Guid& blockGuid, const Util::Guid& startId, const Util::String& table = "_Scripts_Conditions");
    /// check if action script is available
    bool HasActionScript(const Util::String& script);
    /// load an action from a script (sequence action if needed)
    Ptr<Actions::Action> LoadActionFromScript(const Util::String& scriptName, const Ptr<Script::InfoLog>& infoLog = 0);

    /// load state machine
    Ptr<FSM::StateMachine> LoadStateMachine(const Util::String& stateMachineName, const Ptr<Game::Entity>& entity);
    /// debug helper function to re-initialize scripts
    virtual void ReloadScripts(); 

protected:
    /// begin a loading sequenz for can scripts, keep dbreader open 
    void LoadCanScript();
    /// end loading sequenz for can scripts, clear dbreader
    void UnloadCanScript();
    /// begin a loading sequenz for on scripts, keep dbreader open 
    void LoadOnScript();
    /// end loading sequenz for on scripts, clear dbreader
    void UnloadOnScript();
    /// begin a loading sequenz for can scripts, keep dbreader open 
    void LoadStateMachines();
    /// end loading sequenz for can scripts, clear dbreader
    void UnloadStateMachines();
    /// build stateMachineRacks dictionary
    void BuildStateMachineRacks();

    struct ActionOrCondition
    {
        Ptr<Actions::Action> action;
        Ptr<Conditions::Condition> condition;
    };

    struct Statement
    {
        Util::Guid   guid;
        Util::String content;
        Util::String type;
        ActionOrCondition actionOrCondition;
    };

    struct StateRack
    {
        bool isStartState;
        int stateRowNumber;
        Util::Array<int> transitionRowNumbers;
    };

    /// helper function to find statement with specific guid, return -1 on failure
    int FindGuidIndex(const Util::Dictionary<Util::Guid, int>& statementListIndices, const Util::Guid& guid);
    /// load content from a table
    void LoadTableContent(const Util::Guid& blockGuid, const Util::Guid& startId, const Util::String& table, Util::Array<Statement>& statementList, Util::Dictionary<Util::Guid, int>& statementListIndices);       
    /// fill action and condition list
    void FillActionAndConditionLists(Util::Array<Statement>& statementList, Util::Dictionary<Util::Guid, int>& statementListIndices);
    /// fill action and conditions with content 
    void ConstructContent(Util::Array<Statement>& statementList, Util::Dictionary<Util::Guid, int>& statementListIndices);
    /// load state of a state machine
    void LoadStates(Ptr<FSM::StateMachine> stateMachine, IndexT indexRack);
    /// load transitions for a state
    void LoadTransitions(Ptr<FSM::State> state, const Util::String machineName, StateRack indexRack);
    /// load a statemachine State
    Ptr<FSM::State> LoadState(const Util::String& stateMachineName, StateRack rack, IndexT indexRack);
    /// load a transition 
    Ptr<FSM::Transition> LoadTransition();

    Util::Dictionary<Util::StringAtom, Util::Array<StateRack> > stateMachineRacks;
    Util::Dictionary<Util::StringAtom, int> actionScriptIndices;
    Util::Dictionary<Util::StringAtom, int> conditionScriptIndices;

    Ptr<Db::Reader> onScriptDBReader;
    Ptr<Db::Reader> canScriptDBReader;
    Ptr<Db::Reader> stateMachineDBReader;    
    Ptr<Db::Reader> stateDBReader;
    Ptr<Db::Reader> transitionDBReader;
    Ptr<Game::Entity> stateMachineEntity;        // needed while loading the stateMachine 
};

} // namespace Managers
//------------------------------------------------------------------------------

