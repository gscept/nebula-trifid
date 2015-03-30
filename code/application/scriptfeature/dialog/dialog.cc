//------------------------------------------------------------------------------
//  script/dialog/dialogtake.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/dialog/dialog.h"
#include "scriptfeature/scriptattr/scriptattributes.h"
#include "db/reader.h"
#include "db/database.h"
#include "appgame/appconfig.h"
#include "scriptfeature/managers/logmanager.h"
#include "scriptfeature/log/infolog.h"
//#include "locale/localeserver.h"
#include "scriptfeature/managers/dialogmanager.h"
#include "scriptfeature/managers/scriptmanager.h"
#include "scriptfeature/managers/scripttemplatemanager.h"
#include "scriptfeature/dialog/dialogdesc.h"
#include "scriptfeature/dialog/dialogtake.h"
#include "scriptfeature/conditions/condition.h"
#include "scriptfeature/actions/action.h"
#include "scriptfeature/actions/sequenceaction.h"
#include "scriptfeature/actions/actionlist.h"

namespace Script
{
__ImplementClass(Script::Dialog, 'SCDI', Core::RefCounted);

using namespace Attr;

//------------------------------------------------------------------------------
/**
*/
Dialog::Dialog() :
    isActive(false),
    isLoaded(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Dialog::~Dialog() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Script::DialogTake>
Dialog::CreateDialogTake()
{
	return Script::DialogTake::Create();
}

//------------------------------------------------------------------------------
/**
	Load the dialog's content from the database.
*/
void 
Dialog::LoadDialogContent(Ptr<DialogDesc> desc)
{	
    n_assert(!this->root.isvalid());

    // clear old list of speakers
    this->speakers.Clear();

    // store the dialog description
    this->dialogData = desc;

	// load all the takes of this dialog
    this->LoadDialogTakes(this->dialogData->GetGuid());

	// load the dialogtake-tree from the database
    this->CreateTakeHierarchy();
    if (this->root.isvalid())
    {
        // set the dialog loaded flag
        this->isLoaded = true;
    }
    else
    {
        this->isLoaded = false;
    }
}

//------------------------------------------------------------------------------
/**
	Activates the current Dialog. That means, we set the pointer to the first valid
    DialogTake in the start take list, where valid means evaluation of take condition returns true. 
    Activation will fail, and return false if dialog has no valid DialogTakes
    If a start take is given, set it as start take.
*/
bool
Dialog::ActivateDialog(const Ptr<DialogTake>& startTake)
{
	n_assert(this->IsLoaded());
    n_assert(!this->IsActive());

    if (startTake != 0)
    {
        // set start take
        if (this->takeIndexMap.Contains(startTake->GetId()))
        {
            this->currentTake = this->takes[this->takeIndexMap[startTake->GetId()]];
            this->LoadAllTakeActions();
			this->isActive = true;
			return true;
        }
        return false;
    }
	// for all Takes of the root
    int i;
	for (i = 0; i < this->root->GetNumChildTakes(); i++)
	{
		// check if the condition of the first take in the subtree evaluates with true
        Ptr<DialogTake> childTake = this->root->GetChildTakes()[i];
        if (!childTake->HasCondition() || childTake->GetCondition()->Evaluate())
		{
            this->currentTake = childTake;
            this->LoadAllTakeActions();
			this->isActive = true;
			return true;
		}
	}
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
Dialog::LoadAllTakeActions()
{
    int i;
    for (i = 0; i < this->takes.Size(); i++)
    {
        Util::Array<Ptr<Actions::Action> > actions;
        if (this->takes[i]->GetActionBock().IsValid() && this->takes[i]->GetActionRef().IsValid())
        {
            actions = this->LoadTakeActionsByGuid(this->takes[i]->GetActionRef(), this->takes[i]->GetActionBock());
        }
        this->takes[i]->SetActionList(actions);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Dialog::DeactivateDialog()
{
    n_assert(this->IsActive());

	this->currentTake = 0;
	this->responseTexts.Clear();
	this->responseIndices.Clear();
	this->dialogTexts.Clear();

	this->isActive = false;
}

//------------------------------------------------------------------------------
/**
	fills the storage array with the DialogTake's
*/
void
Dialog::LoadDialogTakes(const Util::Guid& guid)
{
    n_assert(guid.IsValid());
    n_assert(!this->IsActive());
    n_assert(!this->IsLoaded());

    n_assert(ScriptTemplateManager::HasInstance());
    Ptr<ScriptTemplateManager> sTmpManager = ScriptTemplateManager::Instance();
    n_assert(sTmpManager->HasTable("_Story_DialogTakes"));
    const Util::Array<int>& blockInd = sTmpManager->GetBlockIndices("_Story_DialogTakes", guid);
    
    // db reader which contains takes for the dialog
    Ptr<Db::Reader> dbReader = sTmpManager->GetReader("_Story_DialogTakes");
    n_assert(dbReader != 0);

    SizeT numRows = blockInd.Size();
    IndexT rowIndex;
    this->takes.Reserve(numRows);
    this->takeIndexMap.Reserve(numRows);
    for (rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
        dbReader->SetToRow(blockInd[rowIndex]);

        // create a new take and initialize it from the current db reader row
        Ptr<DialogTake> newTake = this->CreateDialogTake();
        newTake->SetGuid(dbReader->GetGuid(Attr::TakeGUID));
        newTake->SetId(dbReader->GetString(Attr::TakeId));
        newTake->SetType(DialogTake::StringToType(dbReader->GetString(Attr::TakeType)));
        if (dbReader->GetString(Attr::TakeSpeaker).IsValid())
        {
            newTake->SetSpeaker(dbReader->GetString(Attr::TakeSpeaker));
        }
        else
        {
            // no take speaker defined, use dialog speaker
            newTake->SetSpeaker(this->dialogData->GetSpeaker());
        }
        if (dbReader->GetString(Attr::TakeEmote).IsValid())
        {
            newTake->SetEmote(dbReader->GetString(Attr::TakeEmote));
        }
        else
        {
            // no take emote defined set emty string
            newTake->SetEmote("");
        }
        if (newTake->GetId() != "StartState")
        {
            // add localized text
            Util::String localizedText = /*nLocale*/(dbReader->GetGuid(Attr::TakeLocaId).AsString());

            if(!localizedText.IsValid())
            {
				localizedText.Format("Localization failed for take: '%s'!", dbReader->GetGuid(Attr::TakeLocaId).AsString().AsCharPtr());
                n_printf("WARNING: Localization failed for take: '%s'!", dbReader->GetGuid(Attr::TakeLocaId).AsString().AsCharPtr());
            }

            newTake->AddText(localizedText);
        }
        if (newTake->GetId() != "StartState" 
            && dbReader->HasAttr(Attr::TakeShortTextLocaId) 
            && dbReader->GetGuid(Attr::TakeShortTextLocaId).IsValid())
        {
            // add localized text
            Util::String localizedShortText = /*nLocale*/(dbReader->GetGuid(Attr::TakeShortTextLocaId).AsString());

            if(!localizedShortText.IsValid())
            {
                localizedShortText.Format("ShortText localization failed for take: '%s'!", dbReader->GetGuid(Attr::TakeShortTextLocaId).AsString().AsCharPtr());
                n_printf("WARNING: ShortText localization failed for take: '%s'!", dbReader->GetGuid(Attr::TakeShortTextLocaId).AsString().AsCharPtr());
            }
            newTake->SetShortText(localizedShortText);
        }
        
        // read sound 
        if (dbReader->HasAttr(Attr::TakeSound)
            && dbReader->HasAttr(Attr::TakeSoundActiv)
            && dbReader->GetBool(Attr::TakeSoundActiv)
            && dbReader->GetString(Attr::TakeSound).IsValid())
        {
            newTake->SetSound(dbReader->GetString(Attr::TakeSound));
        }
        else
        {
            // no take sound defined set empty string
            newTake->SetSound("");
        }
        // read timestamp
        if (dbReader->HasAttr(Attr::TakeTimeStamp))
        {
            newTake->SetTimeStamp(dbReader->GetFloat(Attr::TakeTimeStamp));
        }

        // load optional condition block
        const Util::Guid& condRefGuid = dbReader->GetGuid(Attr::ConditionRef);
        const Util::Guid& condBlockGuid = dbReader->GetGuid(Attr::ConditionBlock);
        if (condRefGuid.IsValid() && condBlockGuid.IsValid())
        {
            Ptr<Conditions::Condition> cond = ScriptManager::Instance()->LoadCondition(condBlockGuid, condRefGuid);
            n_assert(cond.isvalid());
            newTake->SetCondition(cond);
        }
        
        Util::Guid actionRefGuid = dbReader->GetGuid(Attr::StatementRef);
        Util::Guid actionBlockGuid = dbReader->GetGuid(Attr::StatementBlock);

        if (actionRefGuid.IsValid() && actionBlockGuid.IsValid())
        {
            newTake->SetActionBlock(actionBlockGuid);
            newTake->SetActionRef(actionRefGuid);
        }

        // finally add new take to our takes array
        this->takes.Append(newTake);
        this->takeIndexMap.Add(newTake->GetId(), this->takes.Size() - 1);

        // add take speaker if not in list already:
        if (-1 == this->speakers.FindIndex(newTake->GetSpeaker()))
        {
            this->speakers.Append(newTake->GetSpeaker());
        }
    }
}

//------------------------------------------------------------------------------
/**
    Loads the actions associated with a dialog take.
*/
Util::Array<Ptr<Actions::Action> >
Dialog::LoadTakeActionsByGuid(const Util::Guid& actionRef, const Util::Guid& actionBlock)
{
    n_assert(!this->IsActive());
    n_assert(actionRef.IsValid() && actionBlock.IsValid());

    Util::Array<Ptr<Actions::Action> > actions;
    Ptr<Actions::ActionList> seq = ScriptManager::Instance()->LoadStatement(actionBlock, actionRef);
    n_assert2(seq.isvalid(), "Dialog: Failed to load action sequence from db!");
    actions = seq->GetActionList();

	return actions;
}

//------------------------------------------------------------------------------
/**
    Executes the assert functions of the takes of the current dialog.
    Additionally it collects errors in the info log object and in case of
    errors returns false instead of closing the application.
*/
bool
Dialog::AssertDialog()
{
    const Ptr<Script::InfoLog>& infoLog = Script::InfoLog::Create();

    // build info string for info log section
    Util::String beginInfo = Util::String();
    beginInfo.Format("<Dialog Guid='%s' Id='%s' Name='%s' Speaker='%s' Group='%s'>",
            this->dialogData->GetGuid().AsString().AsCharPtr(),
            this->dialogData->GetId().AsCharPtr(),
            this->dialogData->GetSpeaker().AsCharPtr(),
            this->dialogData->GetGroup().AsCharPtr());
    infoLog->BeginSection(beginInfo);

    // for all takes in the current dialog
    int i;
	for(i = 0; i < this->takes.Size(); i++)
	{
        this->takes[i]->Assert(infoLog);
	}

    infoLog->EndSection("</Dialog>");

    // in case of errors
    if(infoLog->HasInfo())
    {
        infoLog->SetDescription("Assertion of Dialog");
        infoLog->SetSource("Script::Dialog::AssertDialog()");
        infoLog->SetLogLevel(Script::InfoLog::LogLevel(
                Script::InfoLog::Error | Script::InfoLog::LevelDesign));

        // log errors with log manager
        LogManager::Instance()->PutLogEntry(infoLog);

#if __SOFT_ASSERT_WITH_INGAME_MESSAGES_FOR_DIALOG_SUBSYSTEM__

        n_warning("Script::Dialog::AssertDialog():\n\nErrors occured while assertion of dialog:\nUtil::Guid\t= '%s'\nId\t= '%s'\nSpeaker\t= '%s'\nGroup\t= '%s'\n\nFor detailed information see log.\n",
            this->dialogData->GetGuid().AsString().AsCharPtr(),
            this->dialogData->GetId().AsCharPtr(),
            this->dialogData->GetSpeaker().AsCharPtr(),
            this->dialogData->GetGroup().AsCharPtr());

#endif // __SOFT_ASSERT_WITH_INGAME_MESSAGES_FOR_DIALOG_SUBSYSTEM__

        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Creates the take hierarchy defined in the _Story_DialogTakeChildMapping
    table for this dialog.
*/
void
Dialog::CreateTakeHierarchy()
{
    n_assert(!this->root.isvalid());    
    n_assert(ScriptTemplateManager::HasInstance());

    Ptr<ScriptTemplateManager> sTmpManager = ScriptTemplateManager::Instance();
    n_assert(sTmpManager->HasTable("_Story_DialogTakeChildMapping"));
    Ptr<Db::Reader> dbReader = sTmpManager->GetReader("_Story_DialogTakeChildMapping");
    n_assert(dbReader != 0);

    // for each take...
    IndexT takeIndex;
    for (takeIndex = 0; takeIndex < this->takes.Size(); takeIndex++)
    {
        // extract start take
        if (this->takes[takeIndex]->GetType() == DialogTake::StartState)
        {
            if (this->root.isvalid())
            {
                n_error("Dialog::CreateTakeHierarchy(): multiple start takes in dialog '%s'!", this->GetId().AsCharPtr());
            }
            this->root = this->takes[takeIndex];
        }

        if (sTmpManager->HasBlockIndices("_Story_DialogTakeChildMapping", this->takes[takeIndex]->GetGuid()))
        {
            const Util::Array<int>& blockInd = sTmpManager->GetBlockIndices("_Story_DialogTakeChildMapping", this->takes[takeIndex]->GetGuid());
    
            // find all children of this take
			Util::Array<Util::KeyValuePair<int, Ptr<DialogTake> > > childList;
            IndexT rowIndex;
            SizeT numRows = blockInd.Size();
            for (rowIndex = 0; rowIndex < numRows; rowIndex++)
            {
                dbReader->SetToRow(blockInd[rowIndex]);
                Ptr<DialogTake> childTake = this->GetTakeById(dbReader->GetString(Attr::TakeChildId));
                n_assert(childTake != 0);
                
                // ensure the right order
				childList.Append(Util::KeyValuePair<int,Ptr<DialogTake> >(dbReader->GetInt(Attr::TakeChildPosition),childTake));                    
            }   
            // sort takes
            childList.Sort();
        
            // add sorted takes
            int i;
            int numChilds = childList.Size();
            for (i = 0; i < numChilds; i++)
            {
                this->takes[takeIndex]->AddChildTake(childList[i].Value());
            }
        }
    }

    n_assert(this->root.isvalid());
}

//------------------------------------------------------------------------------
/**
*/
Ptr<DialogTake>
Dialog::GetTakeById(const Util::String& id)
{
    n_assert(id.IsValid());
    if (this->takeIndexMap.Contains(id))
    {
        return this->takes[this->takeIndexMap[id]];
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
	navigates to the next layer in the dialog hierarchy
    may return 0 if no following take is found
*/
Ptr<DialogTake>
Dialog::NavigateToNextLayer(const Ptr<DialogTake>& responseTake)
{
    n_assert(0 != responseTake);

	// for all childs of the take
    int i;
	for(i = 0; i < responseTake->GetNumChildTakes(); i++)
	{
		// check if the condition of the child take evaluates with true
        Ptr<DialogTake> curChildTake = responseTake->GetChildTakes()[i];
        if(!curChildTake->HasCondition() || curChildTake->GetCondition()->Evaluate())
		{
			return curChildTake;
		}
	}

	// if we reached this line, there is no valid child
	return 0;
}

//------------------------------------------------------------------------------
/**
	navigates to the next layer in the dialog hierarchy
    may return 0 if  no following take is found
*/
bool
Dialog::HasNextTakeLayer(const Ptr<Script::DialogTake>& responseTake)
{
    n_assert(0 != responseTake);

	// for all childs of the take
    int i;
    for (i = 0; i < responseTake->GetNumChildTakes(); i++)
	{
		// check if the condition of the child take evaluates with true
        Ptr<DialogTake> curChildTake = responseTake->GetChildTakes()[i];
        if (!curChildTake->HasCondition() || curChildTake->GetCondition()->Evaluate())
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
/**
    Updates the response and dialog texts for the current state of the dialog.
*/
void
Dialog::UpdateDialogData()
{
    n_assert(this->IsActive());
    n_assert(0 != this->currentTake);

    this->responseTexts.Clear();
	this->responseIndices.Clear();
	this->shortResponseTexts.Clear();
	this->usedResponses.Clear();
	this->dialogTexts.Clear();
	this->takeSoundID = "";
	this->responseSoundIDs.Clear();

    // get take texts
	this->dialogTexts.AppendArray(this->currentTake->GetTextList());
	// get the cue id of the current take
    this->takeSoundID = this->currentTake->GetSound();
	// get the Speaker of the current take
	this->takeSpeaker = this->currentTake->GetSpeaker();

	// get response texts
    int i;
	for (i = 0; i < this->currentTake->GetChildTakes().Size(); i++)
	{
        Ptr<DialogTake> childTake = this->currentTake->GetChildTakes()[i];
        if (!childTake->HasCondition() || childTake->GetCondition()->Evaluate())
		{
			// check that there is at least one text in the array, if not we give a warning message
            // and append a dummy text
            if (childTake->GetTextList().Size() == 0)
            {
			    n_warning("Empty Response Node!");
                this->responseTexts.Append("Empty Response!");
 				// add the shortresponse of this childtake to the short response array 
				// FIXME: this is a placeholder
			    this->shortResponseTexts.Append("Empty Response!");
            }
            else
            {
                // a Take is allowed to have more than one textline, so we need to melt them to one
                Util::String shortResponse = childTake->GetShortText();
                Util::String responseText = Util::String::Concatenate(childTake->GetTextList(), "");
			    this->responseTexts.Append(responseText);
				// add the shortresponse of this childtake to the short response array 
                if(shortResponse.IsEmpty())
                {
			        this->shortResponseTexts.Append(responseText);
                }
                else
                {
			        this->shortResponseTexts.Append(shortResponse);
                }

            }

			// add the CueId of the childtake to the response CueId array 
            this->responseSoundIDs.Append(childTake->GetSound());
			// add the known flag of this childtake to the used responses array
			this->usedResponses.Append(DialogManager::Instance()->GetDialogTakeState(this->GetId(), childTake->GetId()));
			// add the speaker of this childtake to the array of the response speaker array
			this->responseSpeaker.Append(childTake->GetSpeaker());

            // add the index to the dialog responseIndex array
			this->responseIndices.Append(i);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
const Util::Guid&
Dialog::GetGuid() const
{
    return this->dialogData->GetGuid();
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
Dialog::GetId() const
{
    return this->dialogData->GetId();
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
Dialog::GetSpeaker() const
{
    return this->dialogData->GetSpeaker();
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
Dialog::GetGroup() const
{
    return this->dialogData->GetGroup();
}

//------------------------------------------------------------------------------
/**
*/
bool
Dialog::IsLoaded() const
{
    return this->isLoaded;
}

//------------------------------------------------------------------------------
/**
*/
bool
Dialog::IsActive() const
{
    return this->isActive;
}

//------------------------------------------------------------------------------
/**
*/
void
Dialog::SetCurrentTake(const Ptr<DialogTake>& take)
{
    n_assert(0 != take);
    this->currentTake = take;
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Ptr<DialogTake> >& 
Dialog::GetAllTakes() const
{
    return this->takes;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<DialogTake>&
Dialog::GetCurrentTake() const
{
    return this->currentTake;
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Util::String>&
Dialog::GetDialogTexts() const
{
    n_assert(this->IsActive());
    return this->dialogTexts;
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Util::String>&
Dialog::GetResponseTexts() const
{
    n_assert(this->IsActive());
    return this->responseTexts;
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Util::String>&
Dialog::GetShortResponseTexts() const
{
    n_assert(this->IsActive());
    return this->shortResponseTexts;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
Dialog::GetTakeSoundID() const
{
    n_assert(this->IsActive());
    return this->takeSoundID;
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Util::String>&
Dialog::GetResponseSoundIDs() const
{
    n_assert(this->IsActive());
    return this->responseSoundIDs;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
Dialog::GetTakeSpeaker() const
{
    n_assert(this->IsActive());
    return this->takeSpeaker;
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Util::String>&
Dialog::GetResponseSpeakers() const
{
    n_assert(this->IsActive());
    return this->responseSpeaker;
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<bool>&
Dialog::GetResponsePassedStates() const
{
    n_assert(this->IsActive());
    return this->usedResponses;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<DialogTake>&
Dialog::GetResponseAtIndex(int index) const
{
    n_assert(this->IsActive());
    n_assert(this->currentTake.isvalid());
    n_assert(index < this->responseIndices.Size());
    return this->GetCurrentTake()->GetChildTakes()[this->responseIndices[index]];
}

//------------------------------------------------------------------------------
/**
*/
bool
Dialog::IsSpeaker(const Util::String& name) const
{
    return -1 != this->speakers.FindIndex(name);
}


} // namespace Script