#pragma once
//------------------------------------------------------------------------------
/**
    @class Script::Task
    
    Represents a task. All tasks without a parentId will be loaded into memory, but
    the sub tasks will only be loaded for active (unlocked) tasks.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "util/guid.h"

//------------------------------------------------------------------------------
namespace Db
{
    class Reader;
    class Writer;
};
namespace Actions
{
    class Action;
};
namespace Conditions
{
    class Condition;
};

namespace Script
{
class Task : public Core::RefCounted
{
    __DeclareClass(Task);
public:
    /// quest states
    enum State
    {
        InvalidState,
        Locked,
        Unlocked,
        Closed,
        Failed,
        Defective
    };
    /// assert mode for assert method
    enum AssertMode
    {
        OpenConditions,
        CloseConditions,
        FailConditions,
        OpenActions,
        CloseActions,
        FailActions
    };

    /// constructor
    Task();
    /// destructor
    virtual ~Task();

    /// initialize quest from database
    void Load(const Ptr<Db::Reader>& dbReader);
    /// save the quest back to the database
    void Save(const Ptr<Db::Writer>& dbWriter, const Util::String& parentName = "");
    /// return true if quest is loaded
    bool IsLoaded() const;
    /// load the quest tasks of this quest from the database
    void LoadSubTasks();

    /// return true if task has open conditions
    bool HasOpenConditions() const;
    /// get the open conditions
    const Ptr<Conditions::Condition>& GetOpenConditions() const;
    /// get the close conditions
    const Ptr<Conditions::Condition>& GetCloseConditions() const;
    /// get fail conditions
    const Ptr<Conditions::Condition>& GetFailConditions() const;

    /// load open conditions
    void LoadOpenConditions();
    /// load close conditions
    void LoadCloseConditions();
    /// load fail conditions
    void LoadFailConditions();
    /// load open actions
    void LoadOpenActions();
    /// load close actions
    void LoadCloseActions();
    /// load fail actions
    void LoadFailActions();
 
    /// unload the quest tasks of this quest
    void UnloadSubTasks();
    /// return true if tasks are loaded
    bool AreSubTasksLoaded() const;

    /// asserts open or close conditions or open or close actions
    bool Assert(AssertMode assertMode);

    /// evaluate the quest task open conditions
    bool EvaluateOpenConditions();
    /// evaluate the quest task closed conditions
    bool EvaluateCloseConditions();
    /// evaluate fail conditions
    bool EvaluateFailConditions();

    /// execute the open actions
    void ExecuteOpenActions();
    /// execute the close actions
    void ExecuteCloseActions();
    /// execute fail actions
    void ExecuteFailActions();

    /// get the quest's guid
    const Util::Guid& GetGuid() const;
    /// get parent guid
    const Util::Guid& GetParentGuid() const;
    /// get the current quest state
    State GetState() const;
    /// get the quest id string
    const Util::String& GetId() const;
    /// get the  title
    const Util::String& GetTitle() const;
    /// get the tasks of the quest
    const Util::Array<Ptr<Task> >& GetSubTasks() const;
    /// get number of subtasks
    int GetNumSubTasks() const;
    /// has subtasks
    bool HasSubTasks() const;

    /// set open text
    void SetOpenText(const Util::String& t);
    /// get open text
    const Util::String& GetOpenText() const;
    /// set close text
    void SetCloseText(const Util::String& t);
    /// get close text
    const Util::String& GetCloseText() const;
    /// set failed text
    void SetFailedText(const Util::String& t);
    /// get failed text
    const Util::String& GetFailedText() const;
    /// get current state text
    Util::String GetText() const;

    /// unlock the task
    void UnlockTask();
    /// close the task
    void CloseTask();
    /// set the task failed
    void FailTask();
    /// evaluate the subtasks
    void EvaluateSubTasks();

    /// convert string to state
    static State StringToState(const Util::String& s);
    /// convert state to string
    static Util::String StateToString(State s);

    /// checks if task status has changed
    bool HasTaskStatusChanged() const;
    /// set task status on unchanged (for updating GUI)
    void SetTaskStatusUnchanged();

	/// set task target entity
	const Util::String& GetTargetEntityName() const;

    /// finds a single task by guid
    Ptr<Script::Task> FindSubTaskByGuid(const Util::Guid& guid);
    /// finds a single task by Id
    Ptr<Script::Task> FindSubTaskById(const Util::String& Id);

    /// set parent key
    void SetParentKey(const Util::String& key);
    /// get parent key
    const Util::String& GetParentKey() const;

    /// set task viewed
    void SetTaskViewed(bool b);
    /// get task viewed state
    bool GetTaskViewed() const;

private:
    struct GuidRefBlock
    {
        Util::Guid ref;
        Util::Guid block;
        bool isValid;
    };

    /// set the quest's guid
    void SetGuid(const Util::Guid& g);
    /// set the quest id string
    void SetId(const Util::String& s);
    /// set the readable quest name
    void SetTitle(const Util::String& s);
    /// set the current quest state
    void SetState(State s);
    /// add a task to the quest
    void AddTask(const Ptr<Task>& t);
    /// set parent guid
    void SetParentGuid(const Util::Guid& s);


	/// set task target entity
	void SetTargetEntityName(const Util::String& name);

    /// execute unlock actions for task an subtasks
    void ExecuteUnlockTask();
    /// set Task Status changed true   
    void SetTaskStatusChanged();

    /// converts an assert mode to its string representation
    Util::String AssertModeToString(AssertMode assertMode);
    /// converts an assert mode to its tag string representation
    Util::String AssertModeToTagString(AssertMode assertMode);

    /// validates a localized text
    void ValidateLocalizedText(Util::String& localizedText, const Util::String& fallbackText);

    Util::Guid guid;                 // Task GUID is unique
    Util::String id;                 // Task Id 
    Util::String title;              // Task Title/Name
    Util::Guid parentGuid;           // a GUID of a Parent Task, such as a Quest
    State state;					 // Task state, locked, unlocked, closed or failed

    GuidRefBlock openCondGuids;
    GuidRefBlock closeCondGuids;
    GuidRefBlock failCondGuids;

    GuidRefBlock openActionGuids;
    GuidRefBlock closeActionGuids;
    GuidRefBlock failActionGuids;

    Util::String openText;
    Util::String closeText;
    Util::String failedText;
	Util::String targetEntityName;
    Util::String parentKey;

    Util::Array<Ptr<Task> > subTasks;                 // list of subtasks
    Util::Array<Ptr<Actions::Action> > openActions;   // list of actions, executed when a task is opend
    Util::Array<Ptr<Actions::Action> > closeActions;  // list of actions, executed when a task is closed
    Util::Array<Ptr<Actions::Action> > failActions;   // list of actions, executed when a task is failed
    Ptr<Conditions::Condition> openCondition;  // Ptr to the OpenCondition 
    Ptr<Conditions::Condition> closeCondition; // Ptr to the CloseCondition 
    Ptr<Conditions::Condition> failCondition;  // Ptr to the FailCondition 

    bool isLoaded;          // is task loaded
    bool subTasksLoaded;    // are subtasks loaded
    bool areActionsLoaded;  // are open and close Actions are loaded
    bool taskStatusChanged;
    bool taskViewed;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
Task::SetTaskViewed(bool b)
{
    this->taskViewed = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Task::GetTaskViewed() const
{
    return this->taskViewed;
}
}; // namespace Script
//------------------------------------------------------------------------------