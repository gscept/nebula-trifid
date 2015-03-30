#pragma once
//------------------------------------------------------------------------------
/**
    @class Script::Dialog

    Contains the runtime structure of a complete dialog.

    (C) 2006 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "util/guid.h"

namespace Actions
{
    class Action;
}

//------------------------------------------------------------------------------
namespace Script
{
class DialogTake;
class DialogDesc;

class Dialog : public Core::RefCounted
{
    __DeclareClass(Dialog);
public:
    /// constructor
    Dialog();
    /// destructor
    virtual ~Dialog();

    /// create a Take (override if needed)
	virtual Ptr<DialogTake> CreateDialogTake();    
    /// load dialog content, takes and so on, after that Dialog content is usable (param group is optional )
    void LoadDialogContent(Ptr<DialogDesc> dialogDesc);
    /// activates the dialog
    bool ActivateDialog(const Ptr<DialogTake>& startTake = 0);
    /// deactivates the dialog
    void DeactivateDialog();

    /// get dialog guid
    const Util::Guid& GetGuid() const;
    /// get dialog id
    const Util::String& GetId() const;
    /// get dialog speaker
    const Util::String& GetSpeaker() const;
    /// get dialog speaker group
    const Util::String& GetGroup() const;
    /// return true if a dialog is loaded
    bool IsLoaded() const;
    /// return true if a dialog was activated
    bool IsActive() const;

    /// check if someone is a possible speaker (doesnt need to actually say anything!)
    bool IsSpeaker(const Util::String& name) const;

    /// set current Dialog Take Pointer
    void SetCurrentTake(const Ptr<DialogTake>& take);
    /// get current Dialog Take Pointer, the dialog must be active
    const Ptr<DialogTake>& GetCurrentTake() const;
    /// get current take text array
	const Util::Array<Util::String>& GetDialogTexts() const;
    /// get response text array
	const Util::Array<Util::String>& GetResponseTexts() const;
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
    /// get index of Storage Take in Response 
	const Ptr<DialogTake>& GetResponseAtIndex(int index) const; 
    
    /// has a following layer, an NPC Take, in the dialog hierarchy (FIXME Floh:????)
	bool HasNextTakeLayer(const Ptr<DialogTake>& responseTake);
    /// navigates to the next layer in the dialog hierarchy, returns pointer to next DialogTake
	Ptr<DialogTake> NavigateToNextLayer(const Ptr<DialogTake>& responseTake);  
    /// updates the dialog-data, refresh ResponseTexts, DialogTexts and ResponseIndex
	void UpdateDialogData();

    /// executes the assert function from the take's of the current dialog
	bool AssertDialog();
    /// returns a take by id
    Ptr<DialogTake> GetTakeById(const Util::String& id);

    const Util::Array<Ptr<DialogTake> >& GetAllTakes() const;

private:
    // fills the storage array with the DialogTake's
	void LoadDialogTakes(const Util::Guid& dialogGuids);
    /// returns the root-take and all the subhierarchies of the dialog-tree by GUID
    void CreateTakeHierarchy();
    /// returns an array of actions by take GUID
    Util::Array<Ptr<Actions::Action> > LoadTakeActionsByGuid(const Util::Guid& actionRef, const Util::Guid& actionBlock);
	/// returns the array of child guid's for the given parent guid
	Util::Array<Util::Guid> LoadChildGuidListByGuid(const Util::Guid& guid);
    /// load all take actions
    void LoadAllTakeActions();

    Ptr<DialogDesc> dialogData;         // holds dialog data: id, name, guid, speaker and group
	Util::Array<Ptr<DialogTake> > takes;      // list of DialogTakes, that belong to this Dialog 
    Util::Dictionary<Util::String,IndexT> takeIndexMap;   // map take indices by TakeId
	Ptr<DialogTake> root;               // dialog root (holds only a list of childs)
	Ptr<DialogTake> currentTake;        // pointer to current active Take

	Util::Array<Util::String> dialogTexts;        // current Take texts (of NPC)
	Util::Array<Util::String> responseTexts;      // current Response Texts (Player)

    Util::Array<Util::String> shortResponseTexts; // current short response Texts (Player)
	Util::Array<bool> usedResponses;			// array of responses witch are already used
	Util::String takeSoundID;				// the SoundId of the current take
	Util::Array<Util::String> responseSoundIDs;	// array with the SoundId's of the current responses
	Util::String takeSpeaker;				// the Speaker of the current take
	Util::Array<Util::String> responseSpeaker;	// array with the Speaker of the current responses

    Util::Array<int> responseIndices;        // matching index between response Texts and its index in storage list

    bool isActive;                      // if Dialog started 
	bool isLoaded;                      // if Dialog data loaded

	Util::Array<Util::String> speakers;           // list of all possible speakers (by Attr::Name)
};


} // namespace Script
//------------------------------------------------------------------------------