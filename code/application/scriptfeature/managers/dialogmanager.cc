//------------------------------------------------------------------------------
//  managers/story/dialogmanager.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/managers/dialogmanager.h"
#include "scriptfeature/managers/scripttemplatemanager.h"
#include "scriptfeature/scriptattr/scriptattributes.h"
#include "scriptfeature/dialog/dialogtake.h"
#include "scriptfeature/dialog/dialog.h"
#include "scriptfeature/dialog/dialogdesc.h"
#include "scriptfeature/scriptprotocol.h"
#include "db/reader.h"
#include "db/writer.h"
#include "db/dbserver.h"

//#include "kernel/nprofiler.h"

namespace Script
{
__ImplementClass(DialogManager, 'DIMA', Game::Manager);
__ImplementSingleton(DialogManager);

//------------------------------------------------------------------------------
/**
*/
DialogManager::DialogManager() :
    dialogTablesLoaded(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
DialogManager::~DialogManager()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Script::Dialog>
DialogManager::CreateScriptDialog()
{
    return Script::Dialog::Create();
}

//------------------------------------------------------------------------------
/**
*/
void
DialogManager::OnLoad()
{
    this->LoadDialogTakeStates();
    this->LoadDialogs();
}

//------------------------------------------------------------------------------
/**
*/
void
DialogManager::OnSave()
{
    this->SaveDialogTakeStates();
    this->SaveDialogs();
}

//------------------------------------------------------------------------------
/**
*/
void
DialogManager::OnActivate()
{
    Manager::OnActivate();
    this->PreloadDialogTables();
}

//------------------------------------------------------------------------------
/**
    This method shuts down the dialog manager when it is removed from the
    game server.
*/
void
DialogManager::OnDeactivate()
{
    // deactivate and unload the dialog
    // this->CancelCurrentDialog();

    // cleanup dialog ui if present
    //if (this->dialogUI.isvalid())
    //{
    //    if( this->dialogUI->IsDialogOpen() )
    //    {
    //        this->dialogUI->CloseDialog();
    //    }
    //    this->dialogUI->Deactivate();
    //    this->dialogUI = 0;
    //}

    if (this->IsDialogLoaded())
    {	
        if (this->activeDialog->IsActive())
        {
            this->activeDialog->DeactivateDialog();
        }
        this->UnloadDialog();
    }

    this->activeDialog = 0;
    Manager::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    This method starts a dialog with an Speakername. If a previous dialog is in 
    progress it will be canceled and unloaded.
*/
void
DialogManager::TalkTo(const Util::String& speaker)
{
    n_assert(speaker.IsValid());
    n_assert(this->HasDialogs(speaker));

    this->CancelCurrentDialog();
    if (this->LoadAssociatedDialogs(speaker))
    {
        this->ActivateDialog();
    }
}

//------------------------------------------------------------------------------
/**
    Activate a currently loaded dialog.
*/
bool
DialogManager::ActivateDialog() 
{
    n_assert(this->activeDialog.isvalid())
    n_assert(this->activeDialog->IsLoaded());

    // activate the loaded dialog
    if (this->activeDialog->ActivateDialog())
    {
        // update response and take texts
        this->activeDialog->UpdateDialogData();
        const Ptr<Script::DialogTake>& curTake = this->activeDialog->GetCurrentTake();
        n_assert(curTake != 0);

        // set the DialogTakeState to passed on current Take
        this->SetDialogTakeState(this->activeDialog->GetId(), curTake->GetId(), true);

        this->PlaySound(curTake);
        this->PlayEmotes(curTake);
        // execute Take actions
        this->ExecuteTakeActions(curTake);

        // update dialog UI if exists
        //if (this->dialogUI.isvalid())
        //{
        //    if (this->dialogUI->IsDialogOpen())
        //    {
        //        this->dialogUI->CloseDialog();
        //    } 
        //    this->dialogUI->OpenDialog(); 
        //    
        //    if( this->dialogUI->IsDialogOpen() )
        //    {
        //        this->dialogUI->UpdateDialog();
        //    }
        //}
    }

    // check if there is a activated dialog
    if (!this->IsDialogActive())
    {
        n_warning("Dialog '%s' has no default take!", this->activeDialog->GetId().AsCharPtr());
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    This method returns true if a dialog is currently in progress.
*/
bool
DialogManager::IsDialogActive() const
{
    return (this->activeDialog.isvalid() && this->activeDialog->IsActive());
}

//------------------------------------------------------------------------------
/**
    This method returns true if a dialog is loaded
*/
bool
DialogManager::IsDialogLoaded() const
{
    return (this->activeDialog.isvalid() && this->activeDialog->IsLoaded());
}

//------------------------------------------------------------------------------
/**
    This method cancels and unloads the currently active dialog
*/
void
DialogManager::CancelCurrentDialog()
{
    // close the dialog UI if present
    //if (this->dialogUI.isvalid())
    //{
    //    if (this->dialogUI->IsDialogOpen())
    //    {
    //        this->dialogUI->CloseDialog();
    //    }
    //}

    // cleanup currently active/loaded dialog
    if (this->IsDialogLoaded())
    {	
        if (this->activeDialog->IsActive())
        {
            this->activeDialog->DeactivateDialog();
        }
        this->UnloadDialog();
    }
}

//------------------------------------------------------------------------------
/**
    This method checks if dialogs are assigned to an NPC at all.
*/
bool
DialogManager::HasDialogs(const Util::String& speakerName, bool asConversation) const
{
    n_assert(speakerName.IsValid());
    IndexT dialogIndex;
    for (dialogIndex = 0; dialogIndex < this->dialogDescs.Size(); dialogIndex++)
    {
        Ptr<DialogDesc> desc = this->dialogDescs[dialogIndex];
        if ((!desc->IsLocked()) 
            && ((!desc->IsConversation() && (!asConversation))
              || (asConversation && desc->IsConversation()))
            && (desc->GetSpeaker() == speakerName))
        {
            return true;
        }
    }
    // fallthrough: no dialogs assigned to speaker
    return false;
}

//------------------------------------------------------------------------------
/**
    This private method loads the dialogtree associated with a given speaker or group
    from the database into the dialog manager. Returns true if at least
    one dialog has been activated.
    if loadGroupDialog is false, speaker will be loaded

    @param speakerOrGroup  name of a speaker or a group
    @param loadGroupDialog if true dialog for given group name will be loaded

    @return true, a dialog was found in db
*/
bool
DialogManager::LoadAssociatedDialogs(const Util::String& speakerOrGroupName, bool loadGroupDialog)
{
    n_assert(speakerOrGroupName.IsValid());
    n_assert(!this->activeDialog.isvalid());

    this->activeDialog = this->GetDialog(speakerOrGroupName, loadGroupDialog);
    if (this->activeDialog == 0)
    {
        n_warning("DialogManager: no dialogs assigned to speaker or group '%s'!", speakerOrGroupName.AsCharPtr());
        return false;
    }

    // assert dialog actions and conditions
    if (this->activeDialog->IsLoaded() && this->activeDialog->AssertDialog())
    {
        return true;
    }
    else
    {
        this->activeDialog = 0;
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Script::Dialog>
DialogManager::GetDialog(const Util::String& dialogId)
{
    n_assert(dialogId.IsValid());
    IndexT dialogIndex = this->dialogDescsById.FindIndex(dialogId);
    if (dialogIndex == InvalidIndex)
    {
        return 0;
    }
    n_assert(this->dialogTablesLoaded);

    const Util::Guid& dialogGuid = this->dialogDescsById[dialogId]->GetGuid();
    this->ValidateDialog(dialogGuid);
    return this->loadedDialogs[dialogGuid];
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Script::Dialog>
DialogManager::GetDialog(const Util::String& speakerOrGroupName, bool loadGroupDialog, bool asConversation)
{
    n_assert(speakerOrGroupName.IsValid());

    // find the guid of unlocked dialog by speaker or group name
    IndexT dialogIndex;
    for (dialogIndex = 0; dialogIndex < this->dialogDescs.Size(); dialogIndex++)
    {
        const Ptr<DialogDesc>& desc = this->dialogDescs[dialogIndex];
        if (!desc->IsLocked() && 
            ((!desc->IsConversation() && !asConversation)
             || (asConversation && desc->IsConversation())))
        {
            if (loadGroupDialog)
            {
                if (desc->GetGroup() == speakerOrGroupName) break;
            }
            else
            {
                if (desc->GetSpeaker() == speakerOrGroupName) break;
            }
        }
    }
    if (dialogIndex == this->dialogDescs.Size())
    {
        // fallthrough: not found
        return 0;
    }

    n_assert(this->dialogTablesLoaded);
    const Util::Guid& dialogGuid = this->dialogDescs[dialogIndex]->GetGuid();
    this->ValidateDialog(dialogGuid);
    return this->loadedDialogs[dialogGuid];
}

//------------------------------------------------------------------------------
/**
    Unload the currently loaded dialog.
*/
void
DialogManager::UnloadDialog()
{
    n_assert((!this->activeDialog.isvalid()) || (!this->activeDialog->IsActive()));    
    this->activeDialog = 0;
}

//------------------------------------------------------------------------------
/**
     Start a dialog for a given speaker
*/
void
DialogManager::StartDialog(const Util::String& speaker) 
{
    if (this->HasDialogs(speaker))
    {
        this->TalkTo(speaker);
    }
}

//------------------------------------------------------------------------------
/**
    Get the take texts of the current dialog node.
*/
const Util::Array<Util::String>&
DialogManager::GetDialogText() const
{
    n_assert(this->activeDialog->IsActive());
    return this->activeDialog->GetDialogTexts();
}

//------------------------------------------------------------------------------
/**
    Get the response texts of the current dialog node.
*/
const Util::Array<Util::String>&
DialogManager::GetResponses() const
{
    n_assert(this->activeDialog->IsActive());
    return this->activeDialog->GetResponseTexts();
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Util::String>&
DialogManager::GetShortResponseTexts() const
{
    n_assert(this->activeDialog->IsActive());
	return this->activeDialog->GetShortResponseTexts();
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
DialogManager::GetTakeSoundID() const
{
    n_assert(this->activeDialog->IsActive());
	return this->activeDialog->GetTakeSoundID();
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Util::String>&
DialogManager::GetResponseSoundIDs() const
{
    n_assert(this->activeDialog->IsActive());
	return this->activeDialog->GetResponseSoundIDs();
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
DialogManager::GetTakeSpeaker() const
{
    n_assert(this->activeDialog->IsActive());
	return this->activeDialog->GetTakeSpeaker();
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Util::String>&
DialogManager::GetResponseSpeakers() const
{
    n_assert(this->activeDialog->IsActive());
	return this->activeDialog->GetResponseSpeakers();
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<bool>&
DialogManager::GetResponsePassedStates() const
{
    n_assert(this->activeDialog->IsActive());
	return this->activeDialog->GetResponsePassedStates();
}

//------------------------------------------------------------------------------
/**
    Tells the dialog manager which response has been choosen by the user.
*/
bool
DialogManager::ChooseResponse(int responseIndex)
{
    // return on no valid active dialog
    if (!this->activeDialog.isvalid())
    {
        return true;
    }

    n_assert(this->activeDialog->IsActive());
    n_assert((responseIndex >= 0)&& (responseIndex < this->activeDialog->GetResponseTexts().Size()));
    Ptr<Script::DialogTake> responseTake = this->activeDialog->GetResponseAtIndex(responseIndex);
    n_assert(responseTake.isvalid());

    // play emotes for pc
    this->PlayEmotes(responseTake);
    this->PlaySound(responseTake, true);

    // execute the actions of this response
    this->ExecuteTakeActions(responseTake);

    // set the DialogTakeState to passed on response
    this->SetDialogTakeState(this->activeDialog->GetId(), responseTake->GetId(), true);

    // branch to sub-take
    Ptr<Script::DialogTake> nextTake = this->activeDialog->NavigateToNextLayer(responseTake);
    if (nextTake.isvalid())
    {	
        // step to the new node in the dialog hierarchy
        this->activeDialog->SetCurrentTake(nextTake);

        // set the DialogTakeState to passed on current Take
        this->SetDialogTakeState(this->activeDialog->GetId(), this->activeDialog->GetCurrentTake()->GetId(), true);

        // update dialog data for the next layer
        this->activeDialog->UpdateDialogData();

        // play sound for npc
        this->PlaySound(this->activeDialog->GetCurrentTake());
        // play emotes for npc
        this->PlayEmotes(this->activeDialog->GetCurrentTake());

        // execute Take actions
        this->ExecuteTakeActions(this->activeDialog->GetCurrentTake());

        // update the dialog UI if present
        //if (this->dialogUI.isvalid())
        //{
        //    if (!this->dialogUI->IsDialogOpen())
        //    {
        //        this->dialogUI->OpenDialog();
        //    }
        //    this->dialogUI->UpdateDialog();
        //}
        return true;
    }
    else
    {
        // no next take, close the dialog
        this->CancelCurrentDialog();
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Executes the actions of a given take
*/
void
DialogManager::ExecuteTakeActions(const Ptr<DialogTake>& take)
{
    n_assert(this->IsDialogActive());
    n_assert(0 != take);
    take->ExecuteActions();
}

//------------------------------------------------------------------------------
/**
    play emotes for current speaker
*/
void
DialogManager::PlayEmotes(const Ptr<DialogTake>& take)
{
    // empty... override
}

//------------------------------------------------------------------------------
/**
    Returns the speaker name of the currently active dialog.
*/
const Util::String&
DialogManager::GetSpeaker() const
{
    n_assert(this->IsDialogLoaded());
    return this->activeDialog->GetSpeaker();
}

//------------------------------------------------------------------------------
/**
    Returns the id of the currently active dialog
*/
const Util::String&
DialogManager::GetDialogId() const
{
    n_assert(this->IsDialogLoaded());
    return this->activeDialog->GetId();
}

//------------------------------------------------------------------------------
/**
    Load dialog take state table from database.
*/
void
DialogManager::LoadDialogTakeStates()
{
    // load all takes
    Ptr<Db::Reader> tdbReader = ScriptTemplateManager::Instance()->GetReader("_Story_DialogTakes");

    SizeT numRows = tdbReader->GetNumRows();
    IndexT rowIndex;
    Util::String key;
    this->takeStatus.Reserve(numRows);
    for (rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
        tdbReader->SetToRow(rowIndex);
        // add default take status to dictionary
        Util::String key;
        key.Append(tdbReader->GetString(Attr::DialogId));
        key.Append(tdbReader->GetString(Attr::TakeId));
        this->takeStatus.Add(key, false);
    }

    // load talked about data
    Ptr<Db::Reader> dbReader = Db::Reader::Create();
    dbReader->SetDatabase(Db::DbServer::Instance()->GetGameDatabase());
    dbReader->SetTableName("_Story_DialogTakeStatus");
    if (dbReader->Open())
    {
        SizeT numRows = dbReader->GetNumRows();
        IndexT rowIndex;
        Util::String key;
        for (rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            dbReader->SetToRow(rowIndex);
            // add take status index to mapper dictionary
            const Util::String& takeId = dbReader->GetString(Attr::TakeId);
            if (this->takeStatus.Contains(takeId))
            {
                this->takeStatus[takeId] = dbReader->GetBool(Attr::TakeStatus);
            }
        }
        dbReader->Close();
    }
    else
    {
        n_error("DialogManager::LoadDialogTakeStates(): loading dialog take states failed!");
    }
}

//------------------------------------------------------------------------------
/**
    Save passed state flags of takes back into database.
*/
void
DialogManager::SaveDialogTakeStates()
{
    Ptr<Db::Writer> dbWriter = Db::Writer::Create();
    dbWriter->SetDatabase(Db::DbServer::Instance()->GetGameDatabase());
    dbWriter->SetTableName("_Story_DialogTakeStatus");
    dbWriter->AddColumn(Db::Column(Attr::TakeId, Db::Column::Primary));
    dbWriter->AddColumn(Db::Column(Attr::TakeStatus));
    if (dbWriter->Open())
    {
        // write take status for each take...
        IndexT i;
        for (i = 0; i < this->takeStatus.Size(); i++)
        {
            dbWriter->BeginRow();
            dbWriter->SetString(Attr::TakeId, this->takeStatus.KeyAtIndex(i));
            dbWriter->SetBool(Attr::TakeStatus, this->takeStatus.ValueAtIndex(i));
            dbWriter->EndRow();
        }
        dbWriter->Close();
    }
    else
    {
        n_error("DialogManager::SaveDialogTakeStates(): saving dialog take states failed!");
    }
}
 
//------------------------------------------------------------------------------
/**
    Load the dialog table from the database.
*/
void
DialogManager::LoadDialogs()
{
    Ptr<Db::Reader> dbReader = Db::Reader::Create();
    dbReader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());
    dbReader->SetTableName("_Story_Dialogs");
    if (dbReader->Open())
    {
        IndexT rowIndex;
        SizeT numRows = dbReader->GetNumRows();
        this->dialogDescs.Reserve(numRows);
        this->dialogDescsById.Reserve(numRows);
        this->dialogDescsByGuid.Reserve(numRows);
        for (rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            dbReader->SetToRow(rowIndex);
            Ptr<DialogDesc> newDesc = Script::DialogDesc::Create();
            newDesc->SetGuid(dbReader->GetGuid(Attr::DialogGUID));
            newDesc->SetId(dbReader->GetString(Attr::DialogId));
            newDesc->SetSpeaker(dbReader->GetString(Attr::DialogSpeaker));
            newDesc->SetGroup(dbReader->GetString(Attr::DialogGroup));
            newDesc->SetConversation(dbReader->GetBool(Attr::DialogAsConversation));
            this->dialogDescs.Append(newDesc);
            this->dialogDescsById.Add(newDesc->GetId(), newDesc);
            this->dialogDescsByGuid.Add(newDesc->GetGuid(), newDesc);
        }
        dbReader->Close();
    }
    else
    {
        n_error("DialogManager::LoadDialogs(): error reading dialog table!");
    }

    Ptr<Db::Reader> gdbReader = Db::Reader::Create();
    gdbReader->SetDatabase(Db::DbServer::Instance()->GetGameDatabase());
    gdbReader->SetTableName("_Story_Dialogs");
    if (gdbReader->Open())
    {
        IndexT rowIndex;
        SizeT numRows = gdbReader->GetNumRows();
        for (rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            gdbReader->SetToRow(rowIndex);
            Util::String dialogId = gdbReader->GetString(Attr::DialogId);
            bool locked = gdbReader->GetBool(Attr::DialogLocked);
            
            IndexT idx = this->dialogDescsById.FindIndex(dialogId);
            if (idx != InvalidIndex)
            {
                this->dialogDescsById.ValueAtIndex(idx)->SetLocked(locked);
            }
        }
        gdbReader->Close();
    }
    else
    {
        n_error("DialogManager::LoadDialogs(): error reading dialog table!");
    }
}

//------------------------------------------------------------------------------
/**
    Make sure a dialog is loaded by GUID.
*/
void
DialogManager::ValidateDialog(const Util::Guid& guid)
{
    n_assert2(this->dialogDescsByGuid.Contains(guid), "Invalid Dialog GUID!");
    n_assert(this->dialogTablesLoaded);
    if (!this->loadedDialogs.Contains(guid))
    {
        Ptr<Dialog> newDialog = Script::Dialog::Create();
        newDialog->LoadDialogContent(this->dialogDescsByGuid[guid]);
        this->loadedDialogs.Add(guid, newDialog);
    }
}

//------------------------------------------------------------------------------
/**
    Save dialog status back to database, only the locked flag can change, so
    only write this back into the database.
*/
void
DialogManager::SaveDialogs()
{
    Ptr<Db::Writer> dbWriter = Db::Writer::Create();
    dbWriter->SetDatabase(Db::DbServer::Instance()->GetGameDatabase());
    dbWriter->SetTableName("_Story_Dialogs");
    dbWriter->AddColumn(Db::Column(Attr::DialogId, Db::Column::Primary));
    dbWriter->AddColumn(Db::Column(Attr::DialogLocked));
    if (dbWriter->Open())
    {
        IndexT dialogIndex;
        SizeT numDialogs = this->dialogDescsById.Size();
        for (dialogIndex = 0; dialogIndex < numDialogs; dialogIndex++)
        {
            dbWriter->BeginRow();
            dbWriter->SetString(Attr::DialogId, this->dialogDescsById.ValueAtIndex(dialogIndex)->GetId());
            dbWriter->SetBool(Attr::DialogLocked, this->dialogDescsById.ValueAtIndex(dialogIndex)->IsLocked());
            dbWriter->EndRow();
        }
        dbWriter->Close();
    }
    else
    {
        n_error("DialogManager::SaveDialogs(): error writing dialog table!");
    }
}

//------------------------------------------------------------------------------
/**
    Make sure that a dialog/take pair exists.
*/
void
DialogManager::AssertDialogTakeExists(const Util::String& dialogTakeKey, const Util::String& dialogId, const Util::String& takeId, const Util::String& callerName)
{
    if (!this->takeStatus.Contains(dialogTakeKey))
    {
        n_error("%s: Dialog '%s' and/or Take '%s' not found!", 
            callerName.AsCharPtr(), dialogId.AsCharPtr(), takeId.AsCharPtr());
    }
}

//------------------------------------------------------------------------------
/**
    Set a take state to passed (when the take has been displayed).
*/
void
DialogManager::SetDialogTakeState(const Util::String& dialogId, const Util::String& takeId, bool isPassed)
{
    Util::String key = dialogId + takeId;
    this->AssertDialogTakeExists(key, dialogId, takeId, "DialogManager::SetDialogState()");
    this->takeStatus[key] = isPassed;
}

//------------------------------------------------------------------------------
/**
    Return the passed flag of a dialog take.
*/
bool
DialogManager::GetDialogTakeState(const Util::String& dialogId, const Util::String& takeId)
{
    Util::String key = dialogId + takeId;
    this->AssertDialogTakeExists(key, dialogId, takeId, "DialogManager::GetDialogState()");
    return this->takeStatus[key];
}

//------------------------------------------------------------------------------
/**
    Set the locked flag of a dialog.
*/
void
DialogManager::SetDialogLockState(const Util::String& dialogId, bool locked)
{
    n_assert(dialogId.IsValid());
    if (this->dialogDescsById.Contains(dialogId))
    {
        this->dialogDescsById[dialogId]->SetLocked(locked);
    }
    else
    {
        n_error("DialogManager::SetDialogLockedState(): Dialog '%s' not found!", dialogId.AsCharPtr());
    }
}

//------------------------------------------------------------------------------
/**
    Set the locked flag of a dialog.
*/
bool
DialogManager::GetDialogLockState(const Util::String& dialogId)
{
    n_assert(dialogId.IsValid());
    if (this->dialogDescsById.Contains(dialogId))
    {
        return this->dialogDescsById[dialogId]->IsLocked();
    }
    else
    {
        n_error("DialogManager::GetDialogLockedState(): Dialog '%s' not found!", dialogId.AsCharPtr());
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
DialogManager::SetupAcceptedMessages()
{
	this->RegisterMessage(ScriptFeature::StartDialog::Id);
    this->RegisterMessage(ScriptFeature::ChooseResponse::Id);
    this->RegisterMessage(ScriptFeature::CancelDialog::Id);
    Game::Manager::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
DialogManager::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    if (msg->CheckId(ScriptFeature::StartDialog::Id)) 
    {
		Ptr<ScriptFeature::StartDialog> startDialog = msg.cast<ScriptFeature::StartDialog>();
        this->StartDialog(startDialog->GetSpeaker());
    }
    else if (msg->CheckId(ScriptFeature::ChooseResponse::Id)) 
    {
		Ptr<ScriptFeature::ChooseResponse> chooseResponse = msg.cast<ScriptFeature::ChooseResponse>();
        this->ChooseResponse(chooseResponse->GetResponse());
    }
    else if (msg->CheckId(ScriptFeature::CancelDialog::Id)) 
    {
        this->CancelCurrentDialog();
    }
    else
    {
        Game::Manager::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
    Return true if a dialogtake exists by take id.
*/
bool
DialogManager::DialogTakeExistsById(const Util::String& dialogId, const Util::String& takeId) const
{
    n_assert(dialogId.IsValid());
    n_assert(takeId.IsValid());
    Util::String key = dialogId + takeId;
    return this->takeStatus.Contains(key);
}

//------------------------------------------------------------------------------
/**
    Preload tables from db via ScriptTemplateManager 
*/
void 
DialogManager::PreloadDialogTables()
{
    n_assert(ScriptTemplateManager::HasInstance());
    ScriptTemplateManager::Instance()->RegisterTable("_Story_DialogTakeChildMapping", Attr::TakeGUID);
    ScriptTemplateManager::Instance()->RegisterTable("_Story_DialogTakes", Attr::DialogGUID);
    this->dialogTablesLoaded = true;
}

//------------------------------------------------------------------------------
/**
*/
//void
//DialogManager::SetDialogUI(UI::DialogUI* ui)
//{
//    n_assert(0 != ui);
//	this->dialogUI = ui;
//}

//------------------------------------------------------------------------------
/**
*/
//UI::DialogUI*
//DialogManager::GetDialogUI() const
//{
//	return this->dialogUI;
//}

//------------------------------------------------------------------------------
/**
*/
bool
DialogManager::DialogExistsById(const Util::String& dialogId) const
{
    n_assert(dialogId.IsValid());
    return this->dialogDescsById.Contains(dialogId);
}

//------------------------------------------------------------------------------
/**
    play sound... override this
*/
void
DialogManager::PlaySound(const Ptr<Script::DialogTake>& take, bool isResponse)
{
    // empty
}

} // namespace Script
