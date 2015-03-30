//------------------------------------------------------------------------------
//  scriptfeature/managers/questmanager.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/managers/questmanager.h"
#include "scriptfeature/scriptattr/scriptattributes.h"
#include "scriptfeature/quest/task.h"
#include "scriptfeature/managers/scripttemplatemanager.h"
#include "db/dbserver.h"
#include "db/reader.h"
#include "db/writer.h"

namespace Script
{
__ImplementClass(QuestManager, 'QUMA', Game::Manager);
__ImplementSingleton(QuestManager);

//using namespace UI;

//------------------------------------------------------------------------------
/**
*/
QuestManager::QuestManager() :
    questsLoaded(false),
    numQuestsEvaluatedPerFrame(10),
    currentQuestIndex(0)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
QuestManager::~QuestManager()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    This method initializes the quest manager when it is attached to the
    game server.
*/
void
QuestManager::OnActivate()
{
    n_assert(!this->QuestsLoaded());
    Manager::OnActivate();
    n_assert(ScriptTemplateManager::HasInstance());
    ScriptTemplateManager::Instance()->RegisterTable("_Story_Tasks", Attr::TaskParentGuid);
    this->LoadQuests();
    this->currentQuestIndex = 0;
}

//------------------------------------------------------------------------------
/**
    This method shuts the quest manager and will unload all quests
*/
void
QuestManager::OnDeactivate()
{
    n_assert(this->QuestsLoaded());
    this->UnloadQuests();
    //this->questLogUserInterface = 0;
    Manager::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    Handle per-frame stuff for the quest manager.
*/
void
QuestManager::OnFrame()
{
    if (this->questsLoaded)
    {
        // evaluate quests in subset (properly handle wraparound
        IndexT i;
        IndexT questIndex = 0;
        for (i = 0; i < this->numQuestsEvaluatedPerFrame; i++)
        {
            questIndex = (i + this->currentQuestIndex) % this->quests.Size();
            this->EvaluateQuest(this->quests[questIndex]);
        }
        this->currentQuestIndex = questIndex + 1;
    }
    Manager::OnFrame();
}

//------------------------------------------------------------------------------
/**
    Evaluate a single quest.
*/
void
QuestManager::EvaluateQuest(const Ptr<Task>& curQuest)
{
    n_assert(0 != curQuest);
    if (curQuest->GetState() == Task::Unlocked)
    {
        // evaluate open conditions of subtasks
        curQuest->EvaluateSubTasks();

        // evaluate whether quest should be closed
        if (curQuest->EvaluateCloseConditions())
        {
            curQuest->CloseTask();
        }
        else if (curQuest->EvaluateFailConditions())
        {
            curQuest->FailTask();
        }
    }
    else if (curQuest->GetState() == Task::Locked)
    {
        // evaluate open condition of current quest
        if (curQuest->HasOpenConditions() && curQuest->EvaluateOpenConditions())
        {
            curQuest->UnlockTask();
        }
    }

    // invoke callback if quest status has changed
    if (curQuest->HasTaskStatusChanged())
    {
        this->OnQuestStatusChanged(curQuest);
    }
}

//------------------------------------------------------------------------------
/**
    Force evaluation of all quests, call this on occasions where it is
    critical that all quests are uptodate.
*/
void
QuestManager::EvaluateAllQuests()
{
    IndexT questIndex;
    SizeT numQuests = this->quests.Size();
    for (questIndex = 0; questIndex < numQuests; questIndex++)
    {
        Task* curQuest = this->quests[questIndex];
        this->EvaluateQuest(curQuest);
    }
}

//------------------------------------------------------------------------------
/**
    Evaluate a subsets of quests (to distribute quest evaluation over
    several frames).
*/
void
QuestManager::EvaluateQuestSubset(IndexT firstQuestIndex, SizeT numQuests)
{
    IndexT i;
    for (i = 0; i < numQuests; i++)
    {
        Task* curQuest = this->quests[firstQuestIndex + i];
        this->EvaluateQuest(curQuest);
    }
}

//------------------------------------------------------------------------------
/**
    Update the Questlog and set the quest to unchanged
*/
void
QuestManager::OnQuestStatusChanged(const Ptr<Script::Task>& quest)
{
    //// update questlog if available and open
    //if(this->questLogUserInterface.isvalid() && this->questLogUserInterface->IsOpen())
    //{
    //    this->questLogUserInterface->QuestStateChanged(quest);
    //}

    quest->SetTaskViewed(false);
    // mark parents
    this->SetTaskViewed(quest);

	quest->SetTaskStatusUnchanged();
}

//------------------------------------------------------------------------------
/**    
*/
void
QuestManager::SetTaskViewed(const Ptr<Script::Task>& task)
{    
    if (task->GetParentGuid().IsValid())
    {
        Task* parent = this->FindQuestByGuid(task->GetParentGuid());
        if (parent)
        {
            parent->SetTaskViewed(task->GetTaskViewed());        
            this->SetTaskViewed(parent);        
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method loads all quests from the database and puts them into the
    internal quest array. Quests internals (specifically, the quest tasks 
    will only be loaded for currently active quests).
*/
void
QuestManager::LoadQuests()
{
    n_assert(!this->questsLoaded);
    
    // load lock status from savegame
    Ptr<Db::Reader> gdbReader = Db::Reader::Create();
    gdbReader->SetDatabase(Db::DbServer::Instance()->GetGameDatabase());
    gdbReader->SetTableName("_Story_Tasks");
    if (gdbReader->Open())
    {
        int rowIndex;
        int numRows = gdbReader->GetNumRows();
        this->taskLockStatus.Reserve(numRows);
        if (gdbReader->HasAttr(Attr::TaskViewed))
        {
            this->taskViewedStatus.Reserve(numRows);
        }
        for (rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            gdbReader->SetToRow(rowIndex);
            Util::String taskId = gdbReader->GetString(Attr::TaskId);
            this->taskLockStatus.Add(gdbReader->GetString(Attr::TaskId), Script::Task::StringToState(gdbReader->GetString(Attr::TaskState)));
            
            // load task viewed flag and mark our already loaded quests
            if (gdbReader->HasAttr(Attr::TaskViewed))
            {
                this->taskViewedStatus.Add(gdbReader->GetString(Attr::TaskId), gdbReader->GetBool(Attr::TaskViewed));
            }
        }
        gdbReader->Close();
    }

    // load all tasks from static db
    Ptr<Db::Reader> dbReader = ScriptTemplateManager::Instance()->GetReader("_Story_Tasks");
    int rowIndex;
    int numRows = dbReader->GetNumRows();
    for (rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
        dbReader->SetToRow(rowIndex);
        if (!dbReader->GetGuid(Attr::TaskParentGuid).IsValid())
        {
            Ptr<Task> newQuest = Task::Create();
            newQuest->Load(dbReader);
            if (newQuest->IsLoaded())
            {
                this->quests.Append(newQuest);
            }
        }
    }
    this->questsLoaded = true;

    // assert open and close actions and conditions
    int i;
    for (i = 0; i < this->GetQuests().Size(); i++)
    {
        this->quests[i]->Assert(Script::Task::OpenConditions);
        this->quests[i]->Assert(Script::Task::CloseConditions);
        this->quests[i]->Assert(Script::Task::FailActions);
        this->quests[i]->Assert(Script::Task::FailConditions);
        this->quests[i]->Assert(Script::Task::OpenActions);
        this->quests[i]->Assert(Script::Task::CloseActions);
    }
    this->currentQuestIndex = 0;
}

//------------------------------------------------------------------------------
/**
    Save quest state.
*/
void
QuestManager::OnSave()
{
    Ptr<Db::Writer> dbWriter = Db::Writer::Create();
    dbWriter->SetDatabase(Db::DbServer::Instance()->GetGameDatabase());
    dbWriter->SetTableName("_Story_Tasks");
    dbWriter->AddColumn(Db::Column(Attr::TaskId, Db::Column::Primary));
    dbWriter->AddColumn(Db::Column(Attr::TaskState));
    dbWriter->AddColumn(Db::Column(Attr::TaskViewed));
    if (dbWriter->Open())
    {
        int questIndex;
        int numQuests = this->quests.Size();
        for (questIndex = 0; questIndex < numQuests; questIndex++)
        {
            this->quests[questIndex]->Save(dbWriter);
        }
        dbWriter->Close();
    }
}

//------------------------------------------------------------------------------
/**
    This method unloads all loaded quests.
*/
void
QuestManager::UnloadQuests()
{
    n_assert(this->questsLoaded);
    this->quests.Clear();
    this->questsLoaded = false;
}

//------------------------------------------------------------------------------
/**
    Return all quests.
*/
const Util::Array<Ptr<Task> >&
QuestManager::GetQuests() const
{
    return this->quests;
}

//------------------------------------------------------------------------------
/**
    Returns all quests which are in a specific quest state.
*/
Util::Array<Ptr<Task> >
QuestManager::FindQuestsByState(Task::State state) const
{
    n_assert(this->questsLoaded);
    Util::Array<Ptr<Task> > result;
    int i;
    int num = this->quests.Size();
    for (i = 0; i < num; i++)
    {
        if (this->quests[i]->GetState() == state)
        {
            result.Append(this->quests[i]);
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Find a single quest by its readable id. May return 0 if not found.
*/
Ptr<Script::Task> 
QuestManager::FindQuestById(const Util::String& id) const
{
    n_assert(this->questsLoaded);
    n_assert(id.IsValid());
    int i;
    int num = this->quests.Size();
    for (i = 0; i < num; i++)
    {
        if (this->quests[i]->GetId() == id)
        {
            return this->quests[i];
        }
    }
    // fallthrough: not found
    return 0;
}

//------------------------------------------------------------------------------
/**
    Find a single quest by its guid. May return 0 if not found.
*/
Ptr<Script::Task> 
QuestManager::FindQuestByGuid(const Util::Guid& guid) const
{
    n_assert(this->questsLoaded);
    n_assert(guid.IsValid());
    int i;
    int num = this->quests.Size();
    for (i = 0; i < num; i++)
    {
        if (this->quests[i]->GetGuid() == guid)
        {
            return this->quests[i];
        }
    }
    // fallthrough: not found
    return 0;
}

//------------------------------------------------------------------------------
/**
    finds a single task by its guid
*/
Ptr<Script::Task> 
QuestManager::FindTaskByGuid(const Util::Guid& guid) const
{
    n_assert(this->questsLoaded);
    Task* result = 0;

    if(guid.IsValid())
    {
        int idxQuest;
        for (idxQuest = 0; idxQuest < this->quests.Size(); idxQuest++)
        {
            if (this->quests[idxQuest]->GetGuid() == guid)
            {
                result = this->quests[idxQuest];
                break;
            }
            else
            {
                result = this->quests[idxQuest]->FindSubTaskByGuid(guid);
                if(0 != result)
                {
                    break;
                }
            }
        }
    }

    // return the result
    return result;
}

//------------------------------------------------------------------------------
/**
    finds a single task by its id an quest name
*/
Ptr<Script::Task> 
QuestManager::FindTaskById(const Util::String& quest, const Util::String& id) const
{
    n_assert(this->questsLoaded);
    Task* result = 0;

    if(quest.IsValid())
    {
        int idxQuest;
        for (idxQuest = 0; idxQuest < this->quests.Size(); idxQuest++)
        {
            if (this->quests[idxQuest]->GetId() == quest)
            {
                if (id.IsValid())
                {
                    result = this->quests[idxQuest]->FindSubTaskById(id);
                }
                else
                {
                    result = this->quests[idxQuest];
                }
                break;
            }
        }
    }

    // return the result
    return result;
}

//------------------------------------------------------------------------------
/**
    Open the quest log window.
*/
void
QuestManager::OpenQuestLog()
{
    //n_assert2(this->questLogUserInterface.isvalid(), "No Questlog Userinterface assigned!");
    //if (!this->questLogUserInterface->IsOpen())
    //{
    //    this->questLogUserInterface->Open();

    //    // we have loaded all quest, set them unchanged 
    //    int i;
    //    for (i = 0; i < this->quests.Size(); i++)
    //    {
    //        quests[i]->SetTaskStatusUnchanged();
    //    }
    //} 
}

//------------------------------------------------------------------------------
/**
    Close the quest log window.
*/
void
QuestManager::CloseQuestLog()
{
    //n_assert2(this->questLogUserInterface.isvalid(), "No Questlog Userinterface assigned!");
    //if (this->questLogUserInterface->IsOpen())
    //{
    //    this->questLogUserInterface->Close();
    //}
}

//------------------------------------------------------------------------------
/**
    Return true if quest log open.
*/
bool
QuestManager::IsQuestLogOpen() const
{
    //n_assert2(this->questLogUserInterface.isvalid(), "No Questlog Userinterface assigned!");
    //return this->questLogUserInterface->IsOpen();
	return false;
}

//------------------------------------------------------------------------------
/**
*/
Script::Task::State
QuestManager::GetTaskLockedStatus(const Util::String& questTaskKey) const
{
    IndexT idx = this->taskLockStatus.FindIndex(questTaskKey);
    if (idx != InvalidIndex)
    {
        return this->taskLockStatus.ValueAtIndex(idx);
    }
    return Script::Task::InvalidState;
}

//------------------------------------------------------------------------------
/**
*/
bool 
QuestManager::GetTaskViewedStatus(const Util::String& questTaskKey) const
{
    IndexT idx = this->taskViewedStatus.FindIndex(questTaskKey);
    if (idx != InvalidIndex)
    {
        return this->taskViewedStatus.ValueAtIndex(idx);
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    called from Task if Failed
*/

void 
QuestManager::OnTaskFailed(const Ptr<Script::Task>& quest)
{
    quest->SetTaskViewed(false);
    // mark parents
    this->SetTaskViewed(quest);
	return;
}

//------------------------------------------------------------------------------
/**
	 called from Task if Unlocked
*/

void 
QuestManager::OnTaskClosed(const Ptr<Script::Task>& quest)
{
    quest->SetTaskViewed(false);
    // mark parents
    this->SetTaskViewed(quest);    
	return;
}	

//------------------------------------------------------------------------------
/**
    called from Task if Failed
*/

void 
QuestManager::OnTaskUnlocked(const Ptr<Script::Task>& quest)
{
    quest->SetTaskViewed(false);
    // mark parents
    this->SetTaskViewed(quest);
	return;
}

}; // namespace Script
