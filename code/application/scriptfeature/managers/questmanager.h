#pragma once
//------------------------------------------------------------------------------
/**
    @class Managers::QuestManager
    
    Provides access to the quest system, can list all quest in their various
    states, can unlock quests, etc...
    
    A complete overview of the quest system can be found here:
    http://gambar/wiki/index.php/DSA_Story_Subsystem
    
    (C) 2005 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "game/manager.h"
#include "core/singleton.h"
#include "scriptfeature/quest/task.h"
#include "util/dictionary.h"

//------------------------------------------------------------------------------
//namespace UI
//{
//    class QuestLogUserInterface;
//};

namespace Script
{
class QuestManager : public Game::Manager
{
    __DeclareClass(QuestManager);
    __DeclareSingleton(QuestManager);
public:

    /// constructor
    QuestManager();
    /// destructor
    virtual ~QuestManager();
    /// called when attached to game server
    virtual void OnActivate();
    /// called when removed from game server
    virtual void OnDeactivate();
    /// called per-frame by the game server
    virtual void OnFrame();
    /// called before saving game state
    virtual void OnSave();
    /// this loads all quests from the world database
    virtual void LoadQuests();
    /// this unloads all quests
    virtual void UnloadQuests();
    /// returns an array of all quests
    const Util::Array<Ptr<Script::Task> >& GetQuests() const;
    /// finds all quests which have a specific quest state
    Util::Array<Ptr<Script::Task> > FindQuestsByState(Script::Task::State s) const;
    /// finds a single quest by its id
    Ptr<Script::Task> FindQuestById(const Util::String& id) const;
    /// finds a single quest by its guid
    Ptr<Script::Task> FindQuestByGuid(const Util::Guid& guid) const;
    /// finds a single task by its guid
    Ptr<Script::Task> FindTaskByGuid(const Util::Guid& guid) const;
    /// finds a single task by its id and quest
    Ptr<Script::Task> FindTaskById(const Util::String& quest, const Util::String& task) const;
    /// open the quest log window
    void OpenQuestLog();
    /// close the quest log window (if open)
    void CloseQuestLog();
    /// return true if quest log window is open
    bool IsQuestLogOpen() const;
    /// return true if quests are loaded
    bool QuestsLoaded() const;
    /// set localize quests flag 
    void LocalizeQuests(bool localize);
    /// force evaluation of all quests
    void EvaluateAllQuests();

private: 
    friend class Script::Task;
    /// evaluate a single quest
    void EvaluateQuest(const Ptr<Script::Task>& curQuest);
    /// evaluate a subset of the quests (to distribute quest evaluation across several frames)
    void EvaluateQuestSubset(IndexT firstQuestIndex, SizeT numQuests);
    /// get task locked status
    Script::Task::State GetTaskLockedStatus(const Util::String& questTaskKey) const;    
    /// get task viewed status
    bool GetTaskViewedStatus(const Util::String& questTaskKey) const;
    /// set task viewed recersivly
    void SetTaskViewed(const Ptr<Script::Task>& task);

protected:
    /// called from OnFrame if a quest status changed
    virtual void OnQuestStatusChanged(const Ptr<Script::Task>& quest); 
	/// called from Task if Failed
    virtual void OnTaskFailed(const Ptr<Script::Task>& quest); 
	/// called from Task if Unlocked
    virtual void OnTaskClosed(const Ptr<Script::Task>& quest); 
	/// called from Task if Failed
    virtual void OnTaskUnlocked(const Ptr<Script::Task>& quest); 

    Util::Array<Ptr<Script::Task> > quests;
    Ptr<Script::Task> activeQuest;
    bool questsLoaded;
    //Ptr<UI::QuestLogUserInterface> questLogUserInterface;
    bool localizeQuest;

    SizeT numQuestsEvaluatedPerFrame;										// number of quests to evaluate per-frame
    IndexT currentQuestIndex;												// first quest index to evaluate this frame
    Util::Dictionary<Util::String, Script::Task::State> taskLockStatus;		//last savegame lock status for all tasks
    Util::Dictionary<Util::String, bool> taskViewedStatus;					//last savegame lock status for all tasks
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
QuestManager::QuestsLoaded() const
{
    return questsLoaded;
}

}; // namespace Script
//------------------------------------------------------------------------------
