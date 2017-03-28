#pragma once
//------------------------------------------------------------------------------
/**
    @class Script::DialogManager
  
    Base funktions:
    TalkTo: load a dialog from DB, for a Speaker, and start it.
    CancelCurrentDialog: Cancel a currently running Dialog

    Manage Locked an Unlocked States of dialog, and allowes status request
    Manage if a Dialog Take was spoken, an save this data.

    Over this Manager the flow of the current active dialog is controlled

    A complete overview of the dialog system can be found here:
    http://gambar/wiki/index.php/DSA_Story_Subsystem

    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "game/manager.h"
#include "core/singleton.h"
#include "util/dictionary.h"
#include "util/guid.h"

namespace Actions
{
    class Action;
}

//namespace UI
//{
//    class DialogUI;
//}

//------------------------------------------------------------------------------
namespace Script
{
class Dialog;
class DialogTake;
class DialogDesc;

class DialogManager : public Game::Manager
{
    __DeclareClass(DialogManager);
    __DeclareSingleton(DialogManager);
public:
    /// constructor
    DialogManager();
    /// destructor
    virtual ~DialogManager();
    /// called when attached from game server
    virtual void OnActivate();
    /// called when removed from game server
    virtual void OnDeactivate();
    /// called after loading game state
    virtual void OnLoad();
    /// called before saving game state
    virtual void OnSave();

    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
	virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

    /// create a Dialog
    virtual Ptr<Script::Dialog> CreateScriptDialog();

    /// start a dialog for a given speaker
    virtual void StartDialog(const Util::String& speaker);
    /// return true if the entity has dialogs assigned
    bool HasDialogs(const Util::String& speaker, bool asConversation = false) const;
    /// initiate a dialog with an NPC
    void TalkTo(const Util::String& speaker);
    /// return true if a dialog is currently in progress
    bool IsDialogActive() const;
    /// return true if a dialog is loaded
    bool IsDialogLoaded() const;
    /// cancel the currently active dialog
    virtual void CancelCurrentDialog();
    /// return true if a dialog exists by dialog id
    bool DialogExistsById(const Util::String& dialogId) const;
    /// return true if a dialogtake exists by take id
    bool DialogTakeExistsById(const Util::String& dialogId, const Util::String& takeId) const;

	///// set the dialog UI 
	//void SetDialogUI(UI::DialogUI* dialogUI);
	///// get the dialog UI
	//UI::DialogUI* GetDialogUI() const;

	/// returns the Speaker name
	virtual const Util::String& GetSpeaker() const;
	/// returns the dialog id
	const Util::String& GetDialogId() const;

	/// get the Take-Text of the current dialog node
	const Util::Array<Util::String>& GetDialogText() const;
	/// get an array of texts filled with the response-texts of current dialog node
	const Util::Array<Util::String>& GetResponses() const;
    /// get short response text array
	const Util::Array<Util::String>& GetShortResponseTexts() const;
    /// get the CueId of the current take
	const Util::String& GetTakeSoundID() const;
    /// get a array with the CueIDs of the current responses
	const Util::Array<Util::String>& GetResponseSoundIDs() const;
    /// get the Speaker of the current take
	const Util::String& GetTakeSpeaker() const;
    /// get a array with the Speaker's of the current responses
	const Util::Array<Util::String>& GetResponseSpeakers() const;
    /// gets a array with the passed state of the current responses
	const Util::Array<bool>& GetResponsePassedStates() const;
	/// this function says the manager witch response is choosed by the user
	bool ChooseResponse(int responseIndex);

	/// set the state of a dialog take
	void SetDialogTakeState(const Util::String& dialogId, const Util::String& takeId, bool isPassed);
	/// get the state of a dialog take
	bool GetDialogTakeState(const Util::String& dialogId, const Util::String& takeId);
	/// sets the locked flag of a dialog
	void SetDialogLockState(const Util::String& dialogId, bool locked);
	/// gets the locked flag of a dialog
	bool GetDialogLockState(const Util::String& dialogId);
    
    /// set localize flag 
    void SetLocalizeDialog(bool localize);

    /// get a dialog by speaker or group name 
    Ptr<Script::Dialog> GetDialog(const Util::String& speakerOrGroupName, bool loadGroupDialog, bool asConversation = false);    
    /// get a dialog by id
    Ptr<Script::Dialog> GetDialog(const Util::String& dialogId);

protected:
	/// load the table DialogTakeState
	void LoadDialogTakeStates();
	/// save the table DialogTakeState
	void SaveDialogTakeStates();
    /// load the Dialog table with locked state
	void LoadDialogs();
	/// save the Dialog table with locked state
	void SaveDialogs();
    /// make sure dialog by GUID is loaded
    void ValidateDialog(const Util::Guid& guid);

    /// try preload tables through ScriptTemplateManager
    void PreloadDialogTables();

	/// executes the actions of a given take
	virtual void ExecuteTakeActions(const Ptr<DialogTake>& take); 
    /// play the emotes of a given take.. override this
	virtual void PlayEmotes(const Ptr<DialogTake>& take); 
    /// play sound... override this
	virtual void PlaySound(const Ptr<DialogTake>& take, bool isResponse = false); 
	/// load dialogs associated with an Speaker or group
    bool LoadAssociatedDialogs(const Util::String& speaker, bool loadForGroup=false); 
    /// unload currently loaded dialogs
    void UnloadDialog();
    /// activate the Dialog, dialog must beloaded before
    virtual bool ActivateDialog();
    /// assert that a dialog and take id exists
    void AssertDialogTakeExists(const Util::String& dialogTakeKey, const Util::String& dialogId, const Util::String& takeId, const Util::String& callerName);

    Ptr<Script::Dialog> activeDialog;
    Util::Array<Ptr<Script::DialogDesc> > dialogDescs;
    Util::Dictionary<Util::String, Ptr<Script::DialogDesc> > dialogDescsById;
    Util::Dictionary<Util::Guid, Ptr<Script::DialogDesc> > dialogDescsByGuid;
                                 
    bool dialogTablesLoaded;                                  
	//Ptr<UI::DialogUI> dialogUI;

    Util::Dictionary<Util::String,bool> takeStatus;

    Util::Dictionary<Util::Guid, Ptr<Script::Dialog> > loadedDialogs;
};

}; // namespace Script
//------------------------------------------------------------------------------
