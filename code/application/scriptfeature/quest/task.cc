//------------------------------------------------------------------------------
//  script/quest.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/scriptattr/scriptattributes.h"
#include "scriptfeature/quest/task.h"

#include "scriptfeature/actions/action.h"
#include "scriptfeature/conditions/condition.h"
#include "scriptfeature/actions/sequenceaction.h"
#include "scriptfeature/actions/actionlist.h"

#include "appgame/appconfig.h"
#include "scriptfeature/managers/logmanager.h"
#include "scriptfeature/log/infolog.h"
//#include "locale/localeserver.h"
#include "db/dbserver.h"
#include "db/writer.h"
#include "db/reader.h"
#include "scriptfeature/managers/questmanager.h"
#include "scriptfeature/managers/scripttemplatemanager.h"
#include "scriptfeature/managers/scriptmanager.h"

namespace Script
{
__ImplementClass(Task, 'TASK', Core::RefCounted);

using namespace Attr;

//------------------------------------------------------------------------------
/**
*/
Task::Task() :
    isLoaded(false),
    subTasksLoaded(false),
    areActionsLoaded(false),
    taskStatusChanged(false),
    taskViewed(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Task::~Task()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This loads the quest from the database.
*/
void
Task::Load(const Ptr<Db::Reader>& dbReader)
{
    n_assert(0 != dbReader);
    n_assert(!this->IsLoaded());

    this->SetGuid(dbReader->GetGuid(Attr::TaskGUID));
    this->SetId(dbReader->GetString(Attr::TaskId));
    this->SetParentGuid(dbReader->GetGuid(Attr::TaskParentGuid));

    // add task data - localize title, open text, close text and failed text

    Util::String localizedText = /*nLocale*/(dbReader->GetGuid(Attr::TaskTitleLocaId).AsString());
    this->ValidateLocalizedText(localizedText, "Warning: No title found!");
    this->SetTitle(localizedText);

    localizedText = /*nLocale*/(dbReader->GetGuid(Attr::TaskOpenTextLocaId).AsString());
    this->ValidateLocalizedText(localizedText, "Warning: No open text found!");
    this->SetOpenText(localizedText);

    localizedText = /*nLocale*/(dbReader->GetGuid(Attr::TaskCloseTextLocaId).AsString());
    this->ValidateLocalizedText(localizedText, "Warning: No close text found!");
    this->SetCloseText(localizedText);

    localizedText = /*nLocale*/(dbReader->GetGuid(Attr::TaskFailedTextLocaId).AsString());
    this->ValidateLocalizedText(localizedText, "Warning: No fail text found!");
    this->SetFailedText(localizedText);


	this->SetTargetEntityName(dbReader->GetString(Attr::TaskTargetEntity));

    // load condition references
    this->openCondGuids.block = dbReader->GetGuid(Attr::OpenConditionBlock);
    this->openCondGuids.ref = dbReader->GetGuid(Attr::OpenConditionRef);
    this->openCondGuids.isValid = this->openCondGuids.ref.IsValid() && this->openCondGuids.block.IsValid();

    this->closeCondGuids.block = dbReader->GetGuid(Attr::CloseConditionBlock);
    this->closeCondGuids.ref = dbReader->GetGuid(Attr::CloseConditionRef);
    this->closeCondGuids.isValid = this->closeCondGuids.ref.IsValid() && this->closeCondGuids.block.IsValid();

    this->failCondGuids.block = dbReader->GetGuid(Attr::FailConditionBlock);
    this->failCondGuids.ref = dbReader->GetGuid(Attr::FailConditionRef);
    this->failCondGuids.isValid = this->failCondGuids.ref.IsValid() && this->failCondGuids.block.IsValid();

    // load action references
    this->openActionGuids.block = dbReader->GetGuid(Attr::OpenStatementBlock);
    this->openActionGuids.ref = dbReader->GetGuid(Attr::OpenStatementRef);
    this->openActionGuids.isValid = this->openActionGuids.ref.IsValid() && this->openActionGuids.block.IsValid();

    this->closeActionGuids.block = dbReader->GetGuid(Attr::CloseStatementBlock);
    this->closeActionGuids.ref = dbReader->GetGuid(Attr::CloseStatementRef);
    this->closeActionGuids.isValid = this->closeActionGuids.ref.IsValid() && this->closeActionGuids.block.IsValid();

    this->failActionGuids.block = dbReader->GetGuid(Attr::FailStatementBlock);
    this->failActionGuids.ref = dbReader->GetGuid(Attr::FailStatementRef);
    this->failActionGuids.isValid = this->failActionGuids.ref.IsValid() && this->failActionGuids.block.IsValid();

    this->isLoaded = true;

    // set the state of the quest object, if the state is unlocked,
    // this will also load the quest tasks
    Util::String key = this->GetParentKey();
    key.Append(this->GetId());
    State state = QuestManager::Instance()->GetTaskLockedStatus(key);
    if (state == InvalidState)
    {
        this->SetState(StringToState(dbReader->GetString(Attr::TaskState)));
    }
    else
    {
        this->SetState(state);
    }
    // get task viewed state
    this->SetTaskViewed(QuestManager::Instance()->GetTaskViewedStatus(key));
    this->taskStatusChanged = false;
}

//------------------------------------------------------------------------------
/**
    Save quest status back to database.
*/
void
Task::Save(const Ptr<Db::Writer>& dbWriter, const Util::String& parentName)
{
    n_assert(0 != dbWriter);
    Util::String key = parentName;
    key.Append(this->GetId());
    dbWriter->BeginRow();
    dbWriter->SetString(Attr::TaskId, key);
    dbWriter->SetString(Attr::TaskState, StateToString(this->state));
    dbWriter->SetBool(Attr::TaskViewed, this->taskViewed);
    dbWriter->EndRow();

    if (this->subTasks.Size() > 0)
    {
        int taskIndex = 0;
        int numTasks = this->subTasks.Size();
        for (taskIndex = 0; taskIndex < numTasks; taskIndex++)
        {
            this->subTasks[taskIndex]->Save(dbWriter, key);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Loads all tasks of the quest. This method is usually called automatically
    as soon as a SetState(Unlocked) is invoked.
*/
void
Task::LoadSubTasks()
{
    n_assert(!this->AreSubTasksLoaded());
    Ptr<Db::Reader> dbReader = ScriptTemplateManager::Instance()->GetReader("_Story_Tasks");

    if (ScriptTemplateManager::Instance()->HasBlockIndices("_Story_Tasks", this->GetGuid()))
    {
        Util::Array<int> indices = ScriptTemplateManager::Instance()->GetBlockIndices("_Story_Tasks", this->GetGuid());
        int rowIndex;
        int numRows = indices.Size();
        for (rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            dbReader->SetToRow(indices[rowIndex]);
            Ptr<Task> newTask = Task::Create();
            Util::String key = this->GetParentKey();
            key.Append(this->GetId());
            newTask->SetParentKey(key);
            newTask->Load(dbReader);
            this->AddTask(newTask);
        }
    }

    // load subtask of subtasks
    int i;
    for (i = 0; i < this->subTasks.Size(); i++)
    {
        if (!this->subTasks[i]->AreSubTasksLoaded())
        {
            this->subTasks[i]->LoadSubTasks();
        }
    }
    this->subTasksLoaded = true;
}

//------------------------------------------------------------------------------
/**
    Unloads all tasks of the quest. This method is usually called automatically
    as soon as a SetState(Closed) is invoked.
*/
void
Task::UnloadSubTasks()
{
    n_assert(this->AreSubTasksLoaded());
    this->subTasks.Clear();
    this->subTasksLoaded = false;
}

//------------------------------------------------------------------------------
/**
    Sets a new state of the quest. This will automatically load/unload the
    quest tasks depending on the state. An unlocked quest will have its 
    Tasks loaded, locked and closed states not.
*/
void
Task::SetState(Task::State newState)
{
    n_assert(this->IsLoaded());
    this->state = newState;
    this->SetTaskStatusChanged(); // state changed

    switch (newState)
    {
        case Task::Locked:
            // if for any reason, tasks are currently loaded
            // (this shouldn't happen), unload them.
            if ((!this->openCondition.isvalid()) && this->openCondGuids.isValid)
            {
                this->openCondition = ScriptManager::Instance()->LoadCondition(this->openCondGuids.block, this->openCondGuids.ref);
                n_assert(this->openCondition.isvalid());
            }
            break;

        case Task::Closed:
            break;
        case Task::Failed:
            break;

        case Task::Unlocked:
            // load all tasks of the quest
            if (!this->AreSubTasksLoaded())
            {
                this->LoadSubTasks();
            }
            if ((!this->closeCondition.isvalid()) && this->closeCondGuids.isValid)
            {
                this->closeCondition = ScriptManager::Instance()->LoadCondition(this->closeCondGuids.block, this->closeCondGuids.ref);
                n_assert(this->closeCondition.isvalid());    
            }
            if ((!this->failCondition.isvalid()) && this->failCondGuids.isValid)
            {
                this->failCondition = ScriptManager::Instance()->LoadCondition(this->failCondGuids.block, this->failCondGuids.ref);
                n_assert(this->failCondition.isvalid());    
            }
            if (!this->areActionsLoaded)
            {
                this->LoadCloseActions();
                this->LoadFailActions();
                this->LoadOpenActions();
                this->areActionsLoaded = true;
            
            }
            break;

        case Task::Defective:
            break;
        case Task::InvalidState:
            n_error("Task::SetState: InvalidState!");
            break;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
Task::LoadOpenConditions()
{
    n_assert(this->IsLoaded());

    if (this->openCondGuids.isValid)
    {
        this->openCondition = ScriptManager::Instance()->LoadCondition(this->openCondGuids.block, this->openCondGuids.ref);
        n_assert(this->openCondition.isvalid());   
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
Task::LoadCloseConditions()
{
    n_assert(this->IsLoaded());

    if (this->closeCondGuids.isValid)
    {
        this->closeCondition = ScriptManager::Instance()->LoadCondition(this->closeCondGuids.block, this->closeCondGuids.ref);
        n_assert(this->closeCondition.isvalid());
    }

}
//------------------------------------------------------------------------------
/**
*/
void 
Task::LoadFailConditions()
{
    n_assert(this->IsLoaded());

    if (this->failCondGuids.isValid)
    {
        this->failCondition = ScriptManager::Instance()->LoadCondition(this->failCondGuids.block, this->failCondGuids.ref);
        n_assert(this->failCondition.isvalid());
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
Task::LoadCloseActions()
{
    n_assert(this->IsLoaded());

    if (this->closeActionGuids.isValid)
    {
        Ptr<Actions::ActionList> seq = ScriptManager::Instance()->LoadStatement(this->closeActionGuids.block, this->closeActionGuids.ref);
        n_assert(seq.isvalid());
        this->closeActions = seq->GetActionList();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
Task::LoadOpenActions()
{
    n_assert(this->IsLoaded());

    if (this->openActionGuids.isValid)
    {
        Ptr<Actions::ActionList> seq = ScriptManager::Instance()->LoadStatement(this->openActionGuids.block, this->openActionGuids.ref);
        n_assert(seq.isvalid());
        this->openActions = seq->GetActionList();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
Task::LoadFailActions()
{
    n_assert(this->IsLoaded());

    if (this->failCondGuids.isValid)
    {
        Ptr<Actions::ActionList> seq = ScriptManager::Instance()->LoadStatement(this->failCondGuids.block, this->failCondGuids.ref);
        n_assert(seq.isvalid());
        this->failActions = seq->GetActionList();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
Task::Assert(AssertMode assertMode)
{
    const Ptr<Script::InfoLog>& infoLog = Script::InfoLog::Create();

    // build info string for info log section
    Util::String beginInfo = Util::String();
    beginInfo.Format("<Task Guid='%s' Id='%s' Title='%s' ParentGuid='%s' State='%s'>",
            this->guid.AsString().AsCharPtr(),
            this->id.AsCharPtr(),
            this->title.AsCharPtr(),
            this->parentGuid.AsString().AsCharPtr(),
            this->StateToString(this->GetState()).AsCharPtr());

    // assert task section
    infoLog->BeginSection(beginInfo);
    {
        // assert open or close conditions or open or close actions
        infoLog->BeginSection("<" + this->AssertModeToTagString(assertMode) + ">");

        int i;
        switch (assertMode)
        {
            case Task::OpenConditions:
                if (this->openCondition.isvalid())
                {
                    this->openCondition->Assert(infoLog);
                }
                break;

            case Task::CloseConditions:
                if (this->closeCondition.isvalid())
                {
                    this->closeCondition->Assert(infoLog);
                }
                break;
            case Task::FailConditions:
                if (this->failCondition.isvalid())
                {
                    this->failCondition->Assert(infoLog);
                }
                break;
            case Task::OpenActions:

                for (i = 0; i < this->openActions.Size(); i++)
                {
                    this->openActions[i]->Assert(infoLog);
                }

                // assert loaded open actions of subtasks
                for (i = 0; i < this->subTasks.Size(); i++)
                {
                    if (this->subTasks[i]->IsLoaded())
                    {
                        this->subTasks[i]->Assert(Task::OpenActions);
                    }
                }
                break;

            case Task::CloseActions:

                for (i = 0; i < this->closeActions.Size(); i++)
                {
                    this->closeActions[i]->Assert(infoLog);
                }

                // assert loaded close actions of subtasks
                for (i = 0; i < this->subTasks.Size(); i++)
                {
                    if (   this->subTasks[i]->IsLoaded() 
                        && this->subTasks[i]->GetState() != Script::Task::Closed 
                        && this->subTasks[i]->GetState() != Script::Task::Failed)
                    {
                        this->subTasks[i]->Assert(Task::CloseActions);
                    }
                }
                break;
            case Task::FailActions:

                for (i = 0; i < this->failActions.Size(); i++)
                {
                    this->failActions[i]->Assert(infoLog);
                }

                // assert loaded fail actions of subtasks
                for (i = 0; i < this->subTasks.Size(); i++)
                {
                    if (   this->subTasks[i]->IsLoaded() 
                        && this->subTasks[i]->GetState() != Script::Task::Closed 
                        && this->subTasks[i]->GetState() != Script::Task::Failed)
                    {
                        this->subTasks[i]->Assert(Task::FailActions);
                    }
                }
                break;
            default :
                n_error("Script::Task::Assert(AssertMode assertMode):\n\nNot supported assert mode!");
                break;
        }

        infoLog->EndSection("</" + this->AssertModeToTagString(assertMode) + ">");
    }
    infoLog->EndSection("</Task>");

    // in case of errors
    if(infoLog->HasInfo())
    {
        infoLog->SetDescription("Assertion of Task");
        infoLog->SetSource("Script::Task::Assert(AssertMode)");
        infoLog->SetLogLevel(Script::InfoLog::LogLevel(
                Script::InfoLog::Error | Script::InfoLog::LevelDesign));

        // log errors with log manager
        LogManager::Instance()->PutLogEntry(infoLog);

#if __SOFT_ASSERT_WITH_INGAME_MESSAGES_FOR_QUEST_SUBSYSTEM__

        n_warning("Script::Task::Assert(Script::Task::AssertMode):\n\nErrors occured while asserting %s of task:\nUtil::Guid\t\t= '%s'\nId\t\t= '%s'\nTitle\t\t= '%s'\nParentGuid\t= '%s'\nState\t\t= '%s'\n\nFor detailed information see log.\n",
                this->AssertModeToString(assertMode).AsCharPtr(),
                this->guid.AsString().AsCharPtr(),
                this->id.AsCharPtr(),
                this->title.AsCharPtr(),
                this->parentGuid.AsString().AsCharPtr(),
                this->StateToString(this->GetState()).AsCharPtr());

#endif // __SOFT_ASSERT_WITH_INGAME_MESSAGES_FOR_QUEST_SUBSYSTEM__

        // set state of this dialog to defective (no actions will be executed in future)
        this->SetState(Task::Defective);

        return false;
    }

    return true;
}


//------------------------------------------------------------------------------
/**
    This evaluates the closed conditions of all open quest tasks. Should
    only be called when the quest is in the unlocked state. If the ClosedConditions
    of a task are true, the task will be closed.
*/
bool
Task::EvaluateCloseConditions()
{
    if (Task::Defective != this->GetState())
    {
        n_assert(Task::Unlocked == this->GetState());

        // eval sub Tasks
        int taskIndex;
        for (taskIndex = 0; taskIndex < this->subTasks.Size(); taskIndex++)
        {
            Ptr<Task> curTask = this->subTasks[taskIndex];
            if (curTask->GetState() == Task::Unlocked)
            {
                // eval subtask close condition
                if (curTask->EvaluateCloseConditions())
                {
                    curTask->SetState(Task::Closed);
                    curTask->ExecuteCloseActions();
                }
            }
        }
        return this->closeCondition->Evaluate();
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    This evaluates the closed conditions of all open quest tasks. Should
    only be called when the quest is in the unlocked state. If the ClosedConditions
    of a task are true, the task will be closed.
*/
bool
Task::EvaluateFailConditions()
{
    if (Task::Defective != this->GetState())
    {
        n_assert(Task::Unlocked == this->GetState());

        // eval sub Tasks
        int taskIndex;
        for (taskIndex = 0; taskIndex < this->subTasks.Size(); taskIndex++)
        {
            Ptr<Task> curTask = this->subTasks[taskIndex];
            if (curTask->GetState() == Task::Unlocked)
            {
                // eval subtask close condition
                if (curTask->EvaluateFailConditions())
                {
                    curTask->SetState(Task::Failed);
                    curTask->ExecuteFailActions();
                }
            }
        }
        return this->failCondition->Evaluate();
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Evaluate open conditions.
*/
bool
Task::EvaluateOpenConditions()
{
    if (Task::Defective != this->GetState())
    {
        return this->openCondition->Evaluate();
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
Task::ExecuteCloseActions()
{
    if (Task::Defective != this->GetState())
    {
        n_assert2(this->areActionsLoaded, "No actions Loaded!");
        int i;
        for (i = 0; i < this->closeActions.Size(); i++)
        {
            this->closeActions[i]->Execute();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Task::ExecuteFailActions()
{
    if (Task::Defective != this->GetState())
    {
        n_assert2(this->areActionsLoaded, "No actions Loaded!");
        int i;
        for (i = 0; i < this->failActions.Size(); i++)
        {
            this->failActions[i]->Execute();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Task::ExecuteOpenActions()
{
    if (Task::Defective != this->GetState())
    {
        n_assert(this->GetState() == Task::Unlocked);
        n_assert2(this->areActionsLoaded, "No actions Loaded!");
        int i;
        for (i = 0; i < this->openActions.Size(); i++)
        {
            this->openActions[i]->Execute();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Task::UnlockTask()
{
    if (this->GetState() == Task::Locked)
    {
		QuestManager::Instance()->OnTaskUnlocked(this);

        SetState(Task::Unlocked);
        this->ExecuteUnlockTask();
    }
}

//------------------------------------------------------------------------------
/**
    This is the public method to close the quest. If the quest is
    not currently in the unlocked state, nothing will happen
*/
void
Task::CloseTask()
{
    if (this->GetState() == Task::Unlocked)
    {
		QuestManager::Instance()->OnTaskClosed(this);

        this->SetState(Task::Closed);   
        this->ExecuteCloseActions();
    }
}

//------------------------------------------------------------------------------
/**
    This is the public method to set the task failed. If the task is
    not currently in the unlocked state, nothing will happen
*/
void
Task::FailTask()
{
    if (this->GetState() == Task::Unlocked)
    {
        this->SetState(Task::Failed);      
        this->ExecuteFailActions();

		QuestManager::Instance()->OnTaskFailed(this);

        // let open subtasks fail 
        if (!this->AreSubTasksLoaded())
        {
            this->LoadSubTasks();
        }
        int i;
        for (i = 0; i < this->subTasks.Size(); i++)
        {
            // fail subtask, if subtask is not unlocked, nothing will happen
            this->subTasks[i]->FailTask();
        }
    }
}

//------------------------------------------------------------------------------
/**
    should only once executed
*/
void
Task::ExecuteUnlockTask()
{
    if (Task::Defective != this->GetState())
    {
        n_assert(this->GetState() == Task::Unlocked);

        // execute the open actions
        this->ExecuteOpenActions();

        // unlock all subtask that can be unlocked and execute their unlock actions
        int i;
        for (i =  0; i < this->subTasks.Size(); i++)
        {
            // if we have opencondition in sub task
            if (this->subTasks[i]->GetState() == Task::Locked && this->subTasks[i]->EvaluateOpenConditions())
            {
                this->subTasks[i]->UnlockTask();
            }
            // if subtask is already unlocked execute open actions
            else if (this->subTasks[i]->GetState() == Task::Unlocked)
            {
                this->subTasks[i]->ExecuteUnlockTask();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    evaluate open condtions of the subtasks, or if they're already open evaluate their subtasks 
*/
void
Task::EvaluateSubTasks()
{
    int i;
    for (i =  0; i < this->subTasks.Size(); i++)
    {
        // if we have opencondition in sub task
        if (this->subTasks[i]->GetState() == Task::Locked && this->subTasks[i]->EvaluateOpenConditions())
        {
           this->subTasks[i]->UnlockTask();
        }
        // if we have an open subtask evaluate their tasks
        if (this->subTasks[i]->GetState() == Task::Unlocked)
        {
            this->subTasks[i]->EvaluateSubTasks();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
Task::IsLoaded() const
{
    return this->isLoaded;
}

//------------------------------------------------------------------------------
/**
*/
bool
Task::HasTaskStatusChanged() const
{
    if (this->taskStatusChanged)
    {
        // if status of this task changed;
        return true;
    }
    else
    {
        // find out if a subTask was changed
        int i;
        for (i = 0; i < this->subTasks.Size(); i++)
        {
            // if just one task status is changed return true
            if (this->subTasks[i]->HasTaskStatusChanged())
            {
                return true;
            }
        }
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Task::SetTaskStatusUnchanged() 
{
    this->taskStatusChanged = false;

    // set status of all subtasks to unchanged
    int i;
    for (i = 0; i < this->subTasks.Size(); i++)
    {
        // if just one task status is changed return true
        this->subTasks[i]->SetTaskStatusUnchanged();
    }
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
Task::GetTargetEntityName() const
{
	return this->targetEntityName;
}

//------------------------------------------------------------------------------
/**
*/
void
Task::SetTargetEntityName(const Util::String& name)
{
	this->targetEntityName = name;
}

//------------------------------------------------------------------------------
/**
*/
void
Task::SetTaskStatusChanged() 
{
    this->taskStatusChanged = true;
}

//------------------------------------------------------------------------------
/**
*/
bool
Task::HasOpenConditions() const
{
    return this->openCondGuids.isValid;
}

//------------------------------------------------------------------------------
/**
*/
Task::State
Task::GetState() const
{
    return this->state;
}

//------------------------------------------------------------------------------
/**
*/
void
Task::SetGuid(const Util::Guid& g)
{
    this->guid = g;
}

//------------------------------------------------------------------------------
/**
*/
const Util::Guid&
Task::GetGuid() const
{
    return this->guid;
}

//------------------------------------------------------------------------------
/**
*/
void
Task::SetParentGuid(const Util::Guid& guid)
{
    this->parentGuid = guid;
}

//------------------------------------------------------------------------------
/**
*/
const Util::Guid&
Task::GetParentGuid() const
{
    return this->parentGuid;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Conditions::Condition>&
Task::GetCloseConditions() const
{
    return this->closeCondition;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Conditions::Condition>&
Task::GetOpenConditions() const
{
    return this->openCondition;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Conditions::Condition>&
Task::GetFailConditions() const
{
    return this->failCondition;
}

//------------------------------------------------------------------------------
/**
*/
bool
Task::AreSubTasksLoaded() const
{
    return this->subTasksLoaded;
}

//------------------------------------------------------------------------------
/**
*/
void
Task::SetId(const Util::String& s)
{
    this->id = s;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
Task::GetId() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
void
Task::SetTitle(const Util::String& s)
{
    this->title = s;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
Task::GetTitle() const
{
    return this->title;
}

//------------------------------------------------------------------------------
/**
*/
void
Task::AddTask(const Ptr<Task>& t)
{
    n_assert(t);
    this->subTasks.Append(t);
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Ptr<Task> >&
Task::GetSubTasks() const
{
    return this->subTasks;
}

//------------------------------------------------------------------------------
/**
*/
void
Task::SetOpenText(const Util::String& t)
{
    this->openText = t;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
Task::GetOpenText() const
{
    return this->openText;
}

//------------------------------------------------------------------------------
/**
*/
void
Task::SetCloseText(const Util::String& t)
{
    this->closeText = t;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
Task::GetCloseText() const
{
    return this->closeText;
}

//------------------------------------------------------------------------------
/**
*/
void
Task::SetFailedText(const Util::String& t)
{
    this->failedText = t;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
Task::GetFailedText() const
{
    return this->failedText;
}

//------------------------------------------------------------------------------
/**
*/
Util::String
Task::GetText() const
{
    switch(this->state)
    {
        case Task::Unlocked:    
            return this->GetOpenText();
            break;
        case Task::Closed:      
            return this->GetCloseText();
            break;
        case Task::Failed:      
            return this->GetFailedText();
            break;
        case Task::Defective:                  
        case Task::Locked:
            break;
        case Task::InvalidState:
            n_error("GetText: this->state is InvalidState!");
            break;
    }

    Util::String result;
    result.Format("No text available for current State: %s", this->StateToString(this->state).AsCharPtr());
    return result;
}

//------------------------------------------------------------------------------
/**
*/
Task::State
Task::StringToState(const Util::String& s)
{
    if      (s == "locked")     return Task::Locked;
    else if (s == "unlocked")   return Task::Unlocked;
    else if (s == "closed")     return Task::Closed;
    else if (s == "failed")     return Task::Failed;
    else if (s == "defective")  return Task::Defective;
    else 
    {
        n_error("Script::Task::StringToState():\n\nNot supported state string: '%s'\n", s.AsCharPtr());
        return Task::InvalidState;
    }
}

//------------------------------------------------------------------------------
/**
*/
Util::String
Task::StateToString(State s)
{
    switch (s)
    {
        case Task::Locked       : return "locked";
        case Task::Unlocked     : return "unlocked";
        case Task::Closed       : return "closed";
        case Task::Failed       : return "failed";
        case Task::Defective    : return "defective";
        default:
            n_error("Script::Task::StateToString():\n\nNot supported state!");
            return "";
    }
}

//------------------------------------------------------------------------------
/**
    Converts an assert mode to its string representation.
*/
Util::String
Task::AssertModeToString(AssertMode assertMode)
{
    switch (assertMode)
    {
        case Task::OpenConditions    : return "open conditions";
        case Task::CloseConditions   : return "close conditions";
        case Task::OpenActions       : return "open actions";
        case Task::CloseActions      : return "close actions";
        default :
            n_error("Script::Task::AssertModeToString():\n\nNot supported assert mode!");
            return "";
    }
}

//------------------------------------------------------------------------------
/**
    Converts an assert mode to its tag string representation.
*/
Util::String
Task::AssertModeToTagString(AssertMode assertMode)
{
    switch (assertMode)
    {
        case Task::OpenConditions    : return "OPENCONDITIONS";
        case Task::CloseConditions   : return "CLOSECONDITIONS";
        case Task::OpenActions       : return "OPENACTIONS";
        case Task::CloseActions      : return "CLOSEACTIONS";
        case Task::FailActions       : return "FAILACTIONS";
        case Task::FailConditions    : return "FAILCONDITION";
        default :
            n_error("Script::Task::AssertModeToTagString():\n\nNot supported assert mode!");
            return "";
    }
}

//------------------------------------------------------------------------------
/**
    has subtasks
*/
bool
Task::HasSubTasks() const
{
    return this->subTasks.Size() > 0;
}

//------------------------------------------------------------------------------
/**
    get the parent guid string
*/
int
Task::GetNumSubTasks() const
{
    return this->subTasks.Size();
}

//------------------------------------------------------------------------------
/**
    find a subtask by guid string
*/
Ptr<Script::Task>
Task::FindSubTaskByGuid(const Util::Guid& guid)
{
    n_assert(this->IsLoaded());
    n_assert(guid.IsValid());

    Ptr<Task> result = 0;

    int idxSubTask;
    for (idxSubTask = 0; idxSubTask < this->subTasks.Size(); idxSubTask++)
    {
        if (this->subTasks[idxSubTask]->GetGuid() == guid)
        {
            result = this->subTasks[idxSubTask];
            break;
        }
        else
        {
            // search in subtasks
            result = this->subTasks[idxSubTask]->FindSubTaskByGuid(guid);
            if(0 != result)
            {
                break;
            }
        }
    }

    // return the result
    return result;
}

//------------------------------------------------------------------------------
/**
    find a subtask by Id string
*/
Ptr<Script::Task>
Task::FindSubTaskById(const Util::String& id)
{
    n_assert(this->IsLoaded());
    n_assert(id.IsValid());

    Ptr<Task> result = 0;

    int idxSubTask;
    for (idxSubTask = 0; idxSubTask < this->subTasks.Size(); idxSubTask++)
    {
        if (this->subTasks[idxSubTask]->GetId() == id)
        {
            result = this->subTasks[idxSubTask];
            break;
        }
        else
        {
            // search in subtasks
            result = this->subTasks[idxSubTask]->FindSubTaskById(id);
            if(0 != result)
            {
                break;
            }
        }
    }

    // return the result
    return result;
}

//------------------------------------------------------------------------------
/**
    validates a localized text - replace with fallback and print a warning, if invalid
*/
void
Task::ValidateLocalizedText(Util::String& localizedText, const Util::String& fallbackText)
{
    if(!localizedText.IsValid())
    {
        localizedText = fallbackText;
        n_printf("Task: %s", fallbackText.AsCharPtr());
    }
    else if(localizedText == "LOCALIZE: '00000000-0000-0000-0000-000000000000'")
    {
        localizedText = "WARNING: No LocaId found!";
    }
}
//------------------------------------------------------------------------------
/**
*/    
void 
Task::SetParentKey(const Util::String& key)
{
    this->parentKey = key;
}

//------------------------------------------------------------------------------
/**
*/    
const Util::String& 
Task::GetParentKey() const
{
    return this->parentKey;
}
}; // namespace Script
