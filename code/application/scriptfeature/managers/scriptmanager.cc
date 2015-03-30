//------------------------------------------------------------------------------
//  managers/story/scriptmanager.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/managers/scriptmanager.h"
#include "managers/entitymanager.h"
#include "scriptfeature/managers/scripttemplatemanager.h"
#include "db/dbserver.h"
#include "db/reader.h"
#include "scriptfeature/scriptattr/scriptattributes.h"
#include "scriptfeature/actions/ifthenelseaction.h"
#include "scriptfeature/actions/sequenceaction.h"
#include "scriptfeature/managers/logmanager.h"
#include "scriptfeature/log/infolog.h"
#include "scriptfeature/conditions/condition.h"
#include "scriptfeature/conditions/not.h"
#include "scriptfeature/conditions/and.h"
#include "scriptfeature/conditions/or.h"
#include "scriptfeature/fsm/statemachine.h"
#include "scriptfeature/fsm/state.h"
#include "scriptfeature/fsm/transition.h"
#include "scriptfeature/actions/actionlist.h"
#include "basegamefeature/basegameattr/basegameattributes.h"

namespace Script
{
__ImplementClass(ScriptManager, 'SCMA', Game::Manager);
__ImplementSingleton(ScriptManager);

using namespace Util;
//------------------------------------------------------------------------------
/**
*/
ScriptManager::ScriptManager() 
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ScriptManager::~ScriptManager()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void 
ScriptManager::OnActivate()
{  
    this->LoadCanScript();
    this->LoadOnScript();
    this->LoadStateMachines();

    ScriptTemplateManager::Instance()->RegisterTable("_Scripts_Conditions", Attr::ConditionBlockGUID);
    ScriptTemplateManager::Instance()->RegisterTable("_Scripts_Statements", Attr::StatementBlockGUID);

    Manager::OnActivate();
}

//------------------------------------------------------------------------------
/**
*/
void 
ScriptManager::OnDeactivate()
{
    this->UnloadStateMachines();
    this->UnloadOnScript();
    this->UnloadCanScript();

    Manager::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Conditions::Condition> 
ScriptManager::LoadConditionScripts(const Util::Array<Util::String>& scripts, bool combineWithOr, const Ptr<Script::InfoLog>& infoLog)
{
    Ptr<Conditions::Condition> conditionBlock = 0;
    n_assert(this->canScriptDBReader.isvalid() && this->canScriptDBReader->IsOpen());

    if (infoLog != 0)
    {
        infoLog->BeginSection("<ScriptManager>");
    }

    // get the actions out of every script
    int scriptCount;
    for (scriptCount = 0; scriptCount < scripts.Size(); scriptCount++)
    {    
        if (this->conditionScriptIndices.Contains(scripts[scriptCount]))
        {
            this->canScriptDBReader->SetToRow(this->conditionScriptIndices[scripts[scriptCount]]);
            Ptr<Conditions::Condition> condition = this->LoadCondition(this->canScriptDBReader->GetGuid(Attr::ConditionBlock), this->canScriptDBReader->GetGuid(Attr::ConditionRef), "_Scripts_Conditions");
            n_assert(condition != 0);

            if (combineWithOr && (conditionBlock == 0))
            {
                conditionBlock = Conditions::Or::Create();
            }

            // if conditionBlock was already set, merge it with this one
            if (conditionBlock != 0)
            {
                if (conditionBlock->IsA(Conditions::Or::RTTI))
                {
                    ((Conditions::Or*)conditionBlock.get_unsafe())->AddCondition(condition);
                }
                else if (conditionBlock->IsA(Conditions::And::RTTI))
                {
                    ((Conditions::And*)conditionBlock.get_unsafe())->AddCondition(condition);
                }
                else 
                {
                    n_error("ScriptManager->LoadCondition: Invalid Condition, must be Or or And");
                }
            }
            else
            { 
                conditionBlock = condition;
            }                
        }
        else
        {
            // if infoLog is available add info
            if (infoLog != 0)
            {
                infoLog->AddInfo("Condition script '" + scripts[scriptCount] + "' does not exist!");
            }
            // if not generate an own log entry
            else
            {
                LogManager::Instance()->PutLogEntry(
                    "Loading of conditions script",
                    "ScriptManager::LoadConditionScripts(const Util::Array<Util::String>&, bool)",
                    Script::InfoLog::LogLevel(Script::InfoLog::Warning | Script::InfoLog::Programming),
                    "Condition script '" + scripts[scriptCount] + "' does not exist!");
            }
        }   
    }

    if (infoLog != 0)
    {
        infoLog->EndSection("</ScriptManager>");
    }

    return conditionBlock;
}

//------------------------------------------------------------------------------
/**
    Load an action from a given script name the action is allways a sequence action
*/
Ptr<Actions::Action>
ScriptManager::LoadActionFromScript(const Util::String& scriptName, const Ptr<Script::InfoLog>& infoLog)
{
    Ptr<Actions::Action> newAction;

    Util::Array<Util::String> scripts;
    scripts.Append(scriptName);

    Util::Array<Ptr<Actions::Action> > actions = ScriptManager::Instance()->LoadActionScripts(scripts, infoLog);
    n_assert(1 == actions.Size());
    n_assert(actions[0]->IsA(Actions::SequenceAction::RTTI));

    return actions[0];
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Actions::Action> > 
ScriptManager::LoadActionScripts(const Util::Array<Util::String>& scripts, const Ptr<Script::InfoLog>& infoLog)
{
    Util::Array<Ptr<Actions::Action> > actionList;

    if (infoLog != 0)
    {
        infoLog->BeginSection("<ScriptManager>");
    }

    // get the actions out of every script
    int scriptCount;
    for (scriptCount = 0; scriptCount < scripts.Size(); scriptCount++)
    {
        if (this->actionScriptIndices.Contains(scripts[scriptCount]))
        {
            this->onScriptDBReader->SetToRow(this->actionScriptIndices[scripts[scriptCount]]);
            Ptr<Actions::Action> action = this->LoadStatement(this->onScriptDBReader->GetGuid(Attr::StatementBlock), this->onScriptDBReader->GetGuid(Attr::StatementRef), "_Scripts_Statements").cast<Actions::Action>();                
            n_assert(action != 0);
            actionList.Append(action);
        }
        // warning when script not found
        else
        {
            // if infoLog is available add info
            if (infoLog != 0)
            {
                infoLog->AddInfo("Action script '" + scripts[scriptCount] + "' does not exist!");
                n_warning("Action script '%s' does not exist!", scripts[scriptCount].AsCharPtr());
            }
            // if not generate an own log entry
            else
            {
                LogManager::Instance()->PutLogEntry(
                    "Loading of action script",
                    "ScriptManager::LoadActionScripts(const Util::Array<Util::String>&, bool)",
                    Script::InfoLog::LogLevel(Script::InfoLog::Warning | Script::InfoLog::Programming),
                    "Action script '" + scripts[scriptCount] + "' does not exist!");
            }
        }   
    }

    if (infoLog != 0)
    {
        infoLog->EndSection("</ScriptManager>");
    }

    return actionList;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ScriptManager::HasActionScript(const Util::String& script)
{
    return this->actionScriptIndices.Contains(script);
}

//------------------------------------------------------------------------------
/**
*/
void 
ScriptManager::LoadCanScript()
{
    n_assert(!this->canScriptDBReader.isvalid())
    this->canScriptDBReader = Db::Reader::Create(); 
    this->canScriptDBReader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());
    this->canScriptDBReader->SetTableName("_Scripts_ConditionScripts");
    this->canScriptDBReader->Open();
    n_assert(this->canScriptDBReader->IsOpen());
    
    this->conditionScriptIndices.Clear();

    // build a dictionary for scripts
    int rowIndex;
    int numRows = canScriptDBReader->GetNumRows();
    if (numRows > 0)
    {
        this->conditionScriptIndices.Reserve(numRows);
        for (rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            canScriptDBReader->SetToRow(rowIndex);
            this->conditionScriptIndices.Add(canScriptDBReader->GetString(Attr::ScriptName), rowIndex);
        }
    }    
}

//------------------------------------------------------------------------------
/**
*/
void 
ScriptManager::UnloadCanScript()
{
    this->canScriptDBReader->Close();
    this->canScriptDBReader = 0;
    this->conditionScriptIndices.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
ScriptManager::LoadOnScript()
{
    n_assert(!this->onScriptDBReader.isvalid())
    // loads scripts from db
    this->onScriptDBReader = Db::Reader::Create(); 
    this->onScriptDBReader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());
    this->onScriptDBReader->SetTableName("_Scripts_ActionScripts");
    this->onScriptDBReader->Open();
    n_assert(this->onScriptDBReader->IsOpen());

    this->actionScriptIndices.Clear();

    // build a dictionary for scripts
    int rowIndex;
    int numRows = onScriptDBReader->GetNumRows();
    if (numRows)
    {
        this->actionScriptIndices.Reserve(numRows);
        for (rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            onScriptDBReader->SetToRow(rowIndex);
            this->actionScriptIndices.Add(onScriptDBReader->GetString(Attr::ScriptName), rowIndex);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ScriptManager::UnloadOnScript()
{
    this->onScriptDBReader->Close();
    this->onScriptDBReader = 0;
    this->actionScriptIndices.Clear();
}

//------------------------------------------------------------------------------
/** Load statement block from db
*/
Ptr<Actions::ActionList>
ScriptManager::LoadStatement(const Util::Guid& blockGuid, const Util::Guid& startId, const Util::String& table)
{
    n_assert(blockGuid.IsValid());
    n_assert(startId.IsValid());
    n_assert(table.IsValid());

    Util::Array<Statement> statementList;
    Util::Dictionary<Util::Guid, int> statementListIndices;
    this->LoadTableContent(blockGuid, startId, table, statementList, statementListIndices);
    this->FillActionAndConditionLists(statementList, statementListIndices);
    this->ConstructContent(statementList, statementListIndices);
    
    // get Begin statment
    n_assert(statementListIndices.Contains(startId));
    int index = statementListIndices[startId];
    n_assert(statementList[index].actionOrCondition.action->IsA(Actions::ActionList::RTTI));
    
    // get ActionList action from statement
	Ptr<Actions::ActionList> actionlist = statementList[index].actionOrCondition.action.cast<Actions::ActionList>();

    return actionlist;
}

//------------------------------------------------------------------------------
/** 
*/
void  
ScriptManager::LoadTableContent
(
    const Util::Guid& blockGuid, 
    const Util::Guid& startId, 
    const Util::String& table,
    Util::Array<Statement>& statements,
    Util::Dictionary<Util::Guid, int>& statementListIndices
)
{
    Ptr<Db::Reader> dbReader = ScriptTemplateManager::Instance()->GetReader(table);
    n_assert(dbReader != 0);
    const Util::Array<int>& rows = ScriptTemplateManager::Instance()->GetBlockIndices(table, blockGuid);
    
    int rowIndex;
    int numRows = rows.Size();
    statementListIndices.Reserve(numRows);
    for (rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
        dbReader->SetToRow(rows[rowIndex]);
        
        // assert that there are no empty values
        n_assert2(dbReader->GetGuid(Attr::StatementGUID).IsValid(), "ScriptManager: Cannot load script! Empty entry in table!!");
        n_assert2(dbReader->GetString(Attr::StatementContent).IsValid(), "ScriptManager: Cannot load script! Empty entry in table!!");
        n_assert2(dbReader->GetString(Attr::StatementType).IsValid(), "ScriptManager: Cannot load script! Empty entry in table!!");

        Statement statement;
        statement.content   = dbReader->GetString(Attr::StatementContent);
        statement.guid      = dbReader->GetGuid(Attr::StatementGUID);
        statement.type      = dbReader->GetString(Attr::StatementType);
        statements.Append(statement);
        statementListIndices.Add(statement.guid, rowIndex);
    }
}

//------------------------------------------------------------------------------
/** 
*/
void 
ScriptManager::FillActionAndConditionLists(Util::Array<Statement>& statementList, Util::Dictionary<Util::Guid, int>& statementListIndices)
{
    int i;
    for( i = 0; i < statementList.Size(); i++)
    {
        if (statementList[i].type == "Action")
        {
            // load actions
            Util::Array<Ptr<Actions::Action> > actions = Actions::Action::CreateActionsFromString(statementList[i].content);
            n_assert2(actions.Size() > 0, "Actions array is empty!");
            
            // create a new ActionList action
            Ptr<Actions::ActionList> actionList = Actions::ActionList::Create();            
            actionList->SetActionList(actions);

            // apppent ActionList to listOfActionsOrConditions
            statementList[i].actionOrCondition.action = actionList;
        }           
        else if (statementList[i].type == "Condition")
        {
            // create condition 
            Ptr<Conditions::Condition> conditionBlock = Conditions::Condition::Create();

            // append condition 
            statementList[i].actionOrCondition.condition = conditionBlock;
        }
        else if (statementList[i].type == "If")
        {
            // create a new IfThenElseAction
            Ptr<Actions::IfThenElseAction> ifAction = Actions::IfThenElseAction::Create();

            // apppent IfThenElseAction to listOfActionsOrConditions
            statementList[i].actionOrCondition.action = ifAction;
        }
        // default case for Else, Then and Begin
        else 
        {
            // create a new ActionList action
            Ptr<Actions::ActionList> actionList = Actions::ActionList::Create();

            // apppent ActionList to listOfActionsOrConditions
            statementList[i].actionOrCondition.action = actionList;
        }
    }
}

//------------------------------------------------------------------------------
/** 
*/
int
ScriptManager::FindGuidIndex(const Util::Dictionary<Util::Guid, int>& statementListIndices, const Util::Guid& guid)
{
    return statementListIndices[guid];
}

//------------------------------------------------------------------------------
/** 
*/
void 
ScriptManager::ConstructContent(Util::Array<Statement>& statementList, Util::Dictionary<Util::Guid, int>& statementListIndices)
{
    int i;     
    for( i = 0; i < statementList.Size(); i++)
    {
        if (statementList[i].type == "Action")
        {
            // action is fine, leave it
        }
        else if (statementList[i].type == "Condition")
        {
            // load condition
            Util::Array<Util::String> blockAndStartId = statementList[i].content.Tokenize(";");
            n_assert(blockAndStartId.Size() == 2);
            
            /// blockAndStartId: first element is ConditionRef, second ConditionBlock
            statementList[i].actionOrCondition.condition = this->LoadCondition(Util::Guid::FromString(blockAndStartId[1]), Util::Guid::FromString(blockAndStartId[0]), "_Scripts_Conditions");
        }
        else if (statementList[i].type == "If")
        {
            // load 
            n_assert(statementList[i].actionOrCondition.action.isvalid());
            Util::Array<Util::String> contentElements = statementList[i].content.Tokenize(";");
            n_assert((contentElements.Size() == 2) || (contentElements.Size() == 3));

            // if (condition)
            // get index of condition
            int index = this->FindGuidIndex(statementListIndices, Util::Guid::FromString(contentElements[0]));
            n_assert(index != -1);
            n_assert(statementList[index].actionOrCondition.condition.isvalid());
            // set condition
            static_cast<Actions::IfThenElseAction*>(statementList[i].actionOrCondition.action.get())->SetCondition(statementList[index].actionOrCondition.condition);
 
            // then
            // get index of then action
            index = this->FindGuidIndex(statementListIndices, Util::Guid::FromString(contentElements[1]));
            n_assert(index != -1);
            n_assert(statementList[index].actionOrCondition.action.isvalid());
            // set action
            Ptr<Actions::ActionList> actionlist = /*static_cast*/statementList[index].actionOrCondition.action.cast<Actions::ActionList>();
            (static_cast<Actions::IfThenElseAction*>(statementList[i].actionOrCondition.action.get()))->SetThenBlock(actionlist);
            
            if (contentElements.Size() == 3)
            {
                // get index of else action
                index = this->FindGuidIndex(statementListIndices, Util::Guid::FromString(contentElements[2]));
                n_assert(index != -1);
                n_assert(statementList[index].actionOrCondition.action.isvalid());
                // set else action
                Ptr<Actions::ActionList> actionlist = /*static_cast*/statementList[index].actionOrCondition.action.cast<Actions::ActionList>();
                (static_cast<Actions::IfThenElseAction*>(statementList[i].actionOrCondition.action.get()))->SetElseBlock(actionlist);
            }
        }
        else
        {
            // load 
            n_assert(statementList[i].actionOrCondition.action.isvalid());
            Util::Array<Util::String> contentElements = statementList[i].content.Tokenize(";");
            n_assert(contentElements.Size() > 0);

            n_assert(statementList[i].actionOrCondition.action->IsA(Actions::ActionList::RTTI));
            Ptr<Actions::ActionList> actionList = statementList[i].actionOrCondition.action.cast<Actions::ActionList>();
            Array<Ptr<Actions::Action> > listOfActions;
            int index;
            // for each guid in content list get the action and attach it to a list
            int elem;
            for (elem = 0; elem < contentElements.Size(); elem++)
            {
                // get index of then action
                index = this->FindGuidIndex(statementListIndices, Util::Guid::FromString(contentElements[elem]));
                n_assert(index != InvalidIndex);
                n_assert(statementList[index].actionOrCondition.action.isvalid());
                
                // append to list
                listOfActions.Append(statementList[index].actionOrCondition.action);
            }
            actionList->SetActionList(listOfActions);
        }
    }
}

//------------------------------------------------------------------------------
/** Load condition from db
*/
Ptr<Conditions::Condition>  
ScriptManager::LoadCondition(const Util::Guid& blockGuid, const Util::Guid& startId, const Util::String& table)
{
    n_assert(blockGuid.IsValid());
    n_assert(startId.IsValid());
    n_assert(table.IsValid());

    Util::Array<Ptr<Conditions::Condition> > listOfConditions;
   
    Ptr<Db::Reader> dbReader = ScriptTemplateManager::Instance()->GetReader(table);
    n_assert(dbReader != 0);
    
    const Util::Array<int>& rows = ScriptTemplateManager::Instance()->GetBlockIndices(table, blockGuid);
    
    Util::Dictionary<Util::Guid, int> conditionGuids;
    Util::Array<Util::String> conditionContents;
    Util::Array<Util::String> conditionTypes;

    int rowIndex;
    int numRows = rows.Size();
    conditionGuids.Reserve(numRows);
    conditionContents.Reserve(numRows);
    conditionTypes.Reserve(numRows);
    for (rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
        dbReader->SetToRow(rows[rowIndex]);
        conditionGuids.Add(dbReader->GetGuid(Attr::ConditionGUID), rowIndex);
        conditionContents.Append(dbReader->GetString(Attr::ConditionContent));
        conditionTypes.Append(dbReader->GetString(Attr::ConditionType));
    }

    // create typed condition List
    Util::Array<Ptr<Conditions::Condition> > conditionList;
    int i;
    for( i = 0; i < conditionTypes.Size(); i++)
    {
        if (conditionTypes[i] == "Atom")
        {
            Util::Array<Ptr<Conditions::Condition> > condition = Conditions::Condition::CreateConditionsFromString(conditionContents[i]);                    
            n_assert2(condition.Size() == 1, "More than one element in atom condition");
            conditionList.Append(condition[0]);
            continue;
        }                   
        if (conditionTypes[i] == "Or")
        {
            Ptr<Conditions::Condition> orCondition = Conditions::Or::Create();
            conditionList.Append(orCondition);                 
            continue;
        }                
        if (conditionTypes[i] == "Not")
        {
            Ptr<Conditions::Condition> notCondition = Conditions::Not::Create();
            conditionList.Append(notCondition);     
            continue;
        }
        Ptr<Conditions::Condition> andCondition = Conditions::And::Create();
        conditionList.Append(andCondition);     
    }
    
    for (i = 0; i < conditionTypes.Size(); i++)
    {
        if (conditionTypes[i] == "Atom")
        {
            continue;
        }
        // set or content 
        if (conditionTypes[i] == "Or")
        {
            // find conditions of child guids
            Ptr<Conditions::Or> orCondition  = (Conditions::Or*) conditionList[i].get();  
            Util::Array<Util::String> condGuids = conditionContents[i].Tokenize(";");
            Util::Array<Ptr<Conditions::Condition> > orContent;
            int index;
            for (index = 0; index < condGuids.Size(); index++)
            {
                n_assert(conditionGuids.Contains(Util::Guid::FromString(condGuids[index])));
                int pos = conditionGuids[Util::Guid::FromString(condGuids[index])];
                orContent.Append(conditionList[pos]);
            }
            n_assert2(condGuids.Size() > 0, "Or condition has no content");
            // set condition
            int cIdx;
            for (cIdx = 0; cIdx < orContent.Size(); cIdx++)
            {
                orCondition->AddCondition(orContent[cIdx]);
            }
            continue;
        }                
        if (conditionTypes[i] == "Not")
        {   
            Ptr<Conditions::Not> condition  = (Conditions::Not*) conditionList[i].get();  
            
            // tokenize child guids
            Util::Array<Util::String> condGuids = conditionContents[i].Tokenize(";");
            n_assert2(condGuids.Size() > 0, "Not condition has no content");
            Util::Array<Ptr<Conditions::Condition> > content;

            // find condition of child guid
            n_assert(conditionGuids.Contains(Util::Guid::FromString(condGuids[0])));
            int pos = conditionGuids[Util::Guid::FromString(condGuids[0])];
            content.Append(conditionList[pos]);

            n_assert2(content.Size() > 0, "Not condition has no content");
            // set condition
            condition->SetCondition(content[0]);
            continue;
        }               
        Ptr<Conditions::And> andCondition  = (Conditions::And*) conditionList[i].get();  
        
        // find conditions of child guids
        Util::Array<Util::String> condGuids = conditionContents[i].Tokenize(";");
        Util::Array<Ptr<Conditions::Condition> > content;
        int index;
        for (index = 0; index < condGuids.Size(); index++)
        {
            n_assert(conditionGuids.Contains(Util::Guid::FromString(condGuids[index])));
            int pos = conditionGuids[Util::Guid::FromString(condGuids[index])];
            content.Append(conditionList[pos]);
        }
        n_assert2(condGuids.Size() > 0, "And condition has no content");

        // set conditions
        int cIdx;
        for (cIdx = 0; cIdx < content.Size(); cIdx++)
        {
            andCondition->AddCondition(content[cIdx]);
        }
    }
    n_assert(conditionGuids.Contains(startId));
    int pos = conditionGuids[startId];
    listOfConditions.Append(conditionList[pos]);

    Ptr<Conditions::And> andCondition = Conditions::And::Create();
    n_assert(listOfConditions.Size() > 0);
    int numConditions;
    for (numConditions = 0; numConditions < listOfConditions.Size(); numConditions++)
    {   
        n_assert(listOfConditions[numConditions] != 0);
        andCondition->AddCondition(listOfConditions[numConditions]);
    }
    Ptr<Conditions::Condition> condition = andCondition.upcast<Conditions::Condition>();

    return condition;
}

//------------------------------------------------------------------------------
/**
    
*/
void ScriptManager::ReloadScripts()
{
    this->UnloadOnScript();
    this->UnloadCanScript();

    ScriptTemplateManager::Instance()->ReloadReader("_Scripts_Conditions");
    ScriptTemplateManager::Instance()->ReloadReader("_Scripts_Statements");
    this->LoadCanScript();
    this->LoadOnScript();
}


//------------------------------------------------------------------------------
/**
*/
Ptr<FSM::StateMachine> 
ScriptManager::LoadStateMachine(const Util::String& stateMachineName, const Ptr<Game::Entity>& entity)
{
    n_assert(stateMachineName.IsValid());
    n_assert(entity != 0);
    n_assert(this->stateMachineDBReader.isvalid());
    n_assert(this->stateMachineDBReader->IsOpen());
    this->stateMachineEntity = entity;

    Ptr<FSM::StateMachine> stateMachine = FSM::StateMachine::Create();
    stateMachine->SetName(stateMachineName);
    stateMachine->SetEntity(entity);

    IndexT index = this->stateMachineRacks.FindIndex(stateMachineName);
    //n_printf("stateMachineName %s, found: %s\n", stateMachineName.AsCharPtr(), this->stateMachineRacks.KeyAtIndex(index).Value());
    n_assert2(index != InvalidIndex, ("ScriptManager::LoadStateMachine missing state machine: " + stateMachineName).AsCharPtr());

    // load states
    this->LoadStates(stateMachine, index);

    return stateMachine;
}

//------------------------------------------------------------------------------
/**
*/
void 
ScriptManager::LoadStates(Ptr<FSM::StateMachine> stateMachine, IndexT indexRack)
{
    n_assert(stateMachine != 0);
    n_assert(this->stateDBReader.isvalid());
    n_assert(this->stateDBReader->IsOpen());

    const Util::Array<StateRack>& statesR = this->stateMachineRacks.ValueAtIndex(indexRack);
    int stateRIndex;
    for (stateRIndex = 0; stateRIndex < statesR.Size(); stateRIndex++)
    {
        stateDBReader->SetToRow(statesR[stateRIndex].stateRowNumber);
        //n_printf("stateRowNumber %i\n", statesR[stateRIndex].stateRowNumber);
        // create state
        Ptr<FSM::State> state = this->LoadState(stateMachine->GetName(), statesR[stateRIndex], indexRack);
        n_assert(state != 0);
        stateMachine->AddState(state);
    }
}

//------------------------------------------------------------------------------
/**
*/
Ptr<FSM::State> 
ScriptManager::LoadState(const Util::String& stateMachineName, StateRack rack, IndexT indexRack)
{
    n_assert(this->stateMachineEntity != 0);
    Ptr<FSM::State> state = FSM::State::Create();
    state->SetName(this->stateDBReader->GetString(Attr::StateName));

    //n_printf("ScriptManager::LoadState: state: %s of machine %s \n", state->GetName().AsCharPtr(), stateMachineName.AsCharPtr());

    Util::Guid blockGuid;
    Util::Guid refGuid;

    // load on entry actions
    refGuid = this->stateDBReader->GetGuid(Attr::OnEntryStatementRef);
    blockGuid = this->stateDBReader->GetGuid(Attr::OnEntryStatementBlock);
    Ptr<Actions::ActionList> entry;

    if (refGuid.IsValid() && blockGuid.IsValid())
    {
        entry = this->LoadStatement(blockGuid, refGuid, "_Scripts_Statements");
    }
    else
    {
        // create empty action
        entry = Actions::ActionList::Create();
    }
    entry->SetEntity(this->stateMachineEntity);
    IndexT currentRow = this->stateDBReader->GetCurrentRowIndex();
    entry->Init();
    this->stateDBReader->SetToRow(currentRow);
    state->AddEntryAction(entry);

    // load onframe actions
    refGuid = this->stateDBReader->GetGuid(Attr::OnFrameStatementRef);
    blockGuid = this->stateDBReader->GetGuid(Attr::OnFrameStatementBlock);
    Ptr<Actions::ActionList> onframe;

    if (refGuid.IsValid() && blockGuid.IsValid())
    {
        onframe = this->LoadStatement(blockGuid, refGuid, "_Scripts_Statements");
    }
    else
    {
        // create empty action
        onframe= Actions::ActionList::Create();
    }
    onframe->SetEntity(this->stateMachineEntity);
    currentRow = this->stateDBReader->GetCurrentRowIndex();
    onframe->Init();
    this->stateDBReader->SetToRow(currentRow);
    state->AddFrameAction(onframe);


    // load on exit actions
    refGuid = this->stateDBReader->GetGuid(Attr::OnExitStatementRef);
    blockGuid = this->stateDBReader->GetGuid(Attr::OnExitStatementBlock);

    Ptr<Actions::ActionList> onExit;
    if (refGuid.IsValid() && blockGuid.IsValid())
    {
        onExit = this->LoadStatement(blockGuid, refGuid, "_Scripts_Statements");
    }
    else
    {
        // create empty exit action
        onExit = Actions::ActionList::Create();
    }
    onExit->SetEntity(this->stateMachineEntity);
    currentRow = this->stateDBReader->GetCurrentRowIndex();
    onExit->Init();
    this->stateDBReader->SetToRow(currentRow);
    state->AddExitAction(onExit);

    this->LoadTransitions(state, stateMachineName, rack);

    return state;
}

//------------------------------------------------------------------------------
/**
*/
void 
ScriptManager::LoadTransitions(Ptr<FSM::State> state, const Util::String machineName, StateRack rack)
{
    n_assert(state != 0);
    n_assert(this->transitionDBReader.isvalid());
    n_assert(this->transitionDBReader->IsOpen());

    int rowIndex;
    for (rowIndex = 0; rowIndex < rack.transitionRowNumbers.Size(); rowIndex++)
    {
        transitionDBReader->SetToRow(rack.transitionRowNumbers[rowIndex]);

        Ptr<FSM::Transition> transition = this->LoadTransition();
        n_assert(transition != 0);
        state->AddTransition(transition);
    }
}

//------------------------------------------------------------------------------
/**
*/
Ptr<FSM::Transition>
ScriptManager::LoadTransition()
{
    n_assert(this->stateMachineEntity != 0);

    Ptr<FSM::Transition> transition = FSM::Transition::Create();

    transition->SetTargetState(transitionDBReader->GetString(Attr::ToState));

    Util::Guid blockGuid;
    Util::Guid refGuid;

    // load condition block
    refGuid = this->transitionDBReader->GetGuid(Attr::ConditionRef);
    blockGuid = this->transitionDBReader->GetGuid(Attr::ConditionBlock);
    Ptr<Conditions::Condition> condition = 0;

    n_assert(refGuid.IsValid() && blockGuid.IsValid())
        condition = this->LoadCondition(blockGuid, refGuid, "_Scripts_Conditions");
    n_assert(condition != 0);
    condition->SetEntity(this->stateMachineEntity);
    transition->AddCondition(condition);

    // load action block if one exist
    refGuid = this->transitionDBReader->GetGuid(Attr::StatementRef);
    blockGuid = this->transitionDBReader->GetGuid(Attr::StatementBlock);

    Ptr<Actions::ActionList> actionList;

    if (refGuid.IsValid() && blockGuid.IsValid())
    {
        actionList = this->LoadStatement(blockGuid, refGuid, "_Scripts_Statements");
    }
    else
    {
        actionList = Actions::ActionList::Create();
    }
    n_assert(actionList != 0);    
    actionList->SetEntity(this->stateMachineEntity);
    actionList->Init();
    transition->AddAction(actionList);

    return transition;
}



//------------------------------------------------------------------------------
/** 
*/
void 
ScriptManager::LoadStateMachines()
{
    if (Db::DbServer::Instance()->GetStaticDatabase()->HasTable("_Script_StateMachines")
        && Db::DbServer::Instance()->GetStaticDatabase()->HasTable("_Script_StateMachineStates")
        && Db::DbServer::Instance()->GetStaticDatabase()->HasTable("_Script_StateTransitions"))
    {
        // open statemachine reader
        n_assert(!this->stateMachineDBReader.isvalid());
        this->stateMachineDBReader = Db::Reader::Create(); 
        this->stateMachineDBReader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());
        this->stateMachineDBReader->SetTableName("_Script_StateMachines");
        this->stateMachineDBReader->Open();
        n_assert(this->stateMachineDBReader->IsOpen());

        // open state reader
        n_assert(!this->stateDBReader.isvalid());
        this->stateDBReader = Db::Reader::Create(); 
        this->stateDBReader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());
        this->stateDBReader->SetTableName("_Script_StateMachineStates");
        this->stateDBReader->Open();
        n_assert(this->stateDBReader->IsOpen());

        // open transition reader
        n_assert(!this->transitionDBReader.isvalid());
        this->transitionDBReader = Db::Reader::Create(); 
        this->transitionDBReader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());
        this->transitionDBReader->SetTableName("_Script_StateTransitions");
        this->transitionDBReader->Open();
        n_assert(this->transitionDBReader->IsOpen());

        // build state machine racks with all indices
        this->BuildStateMachineRacks();
    }
}

//------------------------------------------------------------------------------
/** 
*/
void 
ScriptManager::BuildStateMachineRacks()
{
    n_assert(this->stateMachineDBReader->IsOpen());
    n_assert(this->stateDBReader->IsOpen());
    n_assert(this->transitionDBReader->IsOpen());

    this->stateMachineRacks.Clear();

    Dictionary<Util::String, Util::String> stateMachineNames;
    // build a rack for every StateMachine once
    int rowIndex;
    int numRows = stateMachineDBReader->GetNumRows();
    this->stateMachineRacks.Reserve(numRows);
    for (rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
        stateMachineDBReader->SetToRow(rowIndex);
        stateMachineNames.Add(stateMachineDBReader->GetString(Attr::Id), stateMachineDBReader->GetString(Attr::StartState));
        Util::Array<StateRack> states;
        this->stateMachineRacks.Add(stateMachineDBReader->GetString(Attr::Id), states);
    }

    // build an array of indices of tansitions for a state
    Dictionary<Util::String, Util::Array<int> > stateTransitions;
    numRows = transitionDBReader->GetNumRows();
    for (rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
        transitionDBReader->SetToRow(rowIndex);
        // unique name (equal to state id)
        Util::String name = transitionDBReader->GetString(Attr::MachineName);
        name.Append("_");
        name.Append(transitionDBReader->GetString(Attr::StateName));

        // append transition index  to state
        IndexT index = stateTransitions.FindIndex(name);
        if (index == InvalidIndex)
        {
            Util::Array<int> rowNumbers;
            rowNumbers.Append(rowIndex);
            stateTransitions.Add(name, rowNumbers);
        }
        else
        {
            stateTransitions.ValueAtIndex(index).Append(rowIndex);
        }
    }

    numRows = stateDBReader->GetNumRows();
    for (rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
        stateDBReader->SetToRow(rowIndex);

        // create a new state rack
        StateRack newRack;
        newRack.stateRowNumber = rowIndex;

        // get states transition indices
        IndexT index = stateTransitions.FindIndex(stateDBReader->GetString(Attr::Id));
        if (index != InvalidIndex)
        {
            newRack.transitionRowNumbers.AppendArray(stateTransitions.ValueAtIndex(index));
        }

        // define if start state
        IndexT mIndex = stateMachineNames.FindIndex(stateDBReader->GetString(Attr::MachineName));
        n_assert(mIndex != InvalidIndex);

        if (stateMachineNames.ValueAtIndex(mIndex) == stateDBReader->GetString(Attr::StateName))
        {
            newRack.isStartState = true;
        }
        else
        {
            newRack.isStartState = false;
        }

        // add rack to statemachine
        IndexT smrIndex = stateMachineRacks.FindIndex(stateDBReader->GetString(Attr::MachineName));
        n_assert(smrIndex != InvalidIndex);
        this->stateMachineRacks.ValueAtIndex(smrIndex).Append(newRack);
    }
}

//------------------------------------------------------------------------------
/** 
*/
void 
ScriptManager::UnloadStateMachines()
{
    this->stateDBReader->Close();
    this->stateMachineDBReader->Close();
    this->transitionDBReader->Close();    
}
}; // namespace Script
