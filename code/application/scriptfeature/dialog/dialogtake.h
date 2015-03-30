#pragma once
//------------------------------------------------------------------------------
/**
    @class Script::DialogTake
    
    FIXME: the following description is no longer accurate!!!

    A dialog take is the smallest element of the dialog system. The take contain
	a id, a speaker-name, a type, a conditionhierarchy, a list of actions, 
	a list of texts and a list of children.
	The id identifies the Take in the dialoghierarchy and must unique in the dialog.
	The speaker-name identifies the 'person' witch speaks.
	The type identifies the dialogtake as StartState, as Take or as Response.
	The conditionhierarchy is used to rule if the Take is active or not.
	The actions are executed if the Take is active.
	The list of texts represents the dialogtext witch the speaker is calling or
	the responsetext.
    
    A complete overview of the dialog system can be found here:
    http://gambar/wiki/index.php/DSA_Story_Subsystem

    (C) 2005 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "util/guid.h"
#include "timing/time.h"

namespace Conditions
{
    class Condition;
};

namespace Actions
{
    class Action;
};
//------------------------------------------------------------------------------
namespace Script
{
class InfoLog;
	
class DialogTake : public Core::RefCounted
{
    __DeclareClass(DialogTake);
public:
    /// take types
	enum Type
	{
		None = 0,
		StartState,
		Take,
		Response,
	};

    /// constructor
    DialogTake();
    /// destructor
    virtual ~DialogTake();

    /// set the ID of the take
    void SetGuid(const Util::Guid& guid);
    /// get the ID of the take
    const Util::Guid& GetGuid() const;

    /// set the ID of the take
    void SetId(const Util::String& id);
    /// get the ID of the take
    const Util::String& GetId() const;

    /// set the name of the speaker
    void SetSpeaker(const Util::String& speaker);
    /// get the name of the speaker
    const Util::String& GetSpeaker() const;

    /// set the emote string of the take
    void SetEmote(const Util::String& emote);
    /// get the emote string of the take
    const Util::String& GetEmote() const;

    /// set the type of the Take
    void SetType(const Type type);
    /// get the type of the Take
    Type GetType() const;

    /// set the sound name of the take
    void SetSound(const Util::String& id);
    /// get the sound name of the take
    const Util::String& GetSound() const;

    /// set the short text of the take
    void SetShortText(const Util::String& id);
    /// get the short text of the take
    const Util::String& GetShortText() const;

    /// set timestamp
    void SetTimeStamp(Timing::Time t);
    /// get timestamp
    Timing::Time GetTimeStamp() const;

    /// set action reference guid
    void SetActionRef(const Util::Guid& guid);
    /// get action reference guid
    const Util::Guid& GetActionRef() const;
    /// set action block guid
    void SetActionBlock(const Util::Guid& guid);
    /// get action block guid
    const Util::Guid& GetActionBock() const;

    /// add a text to the textlist
    void AddText(const Util::String& text);
    /// get the textlist
    const Util::Array<Util::String>& GetTextList();
    /// add a action to the actionlist
	void AddAction(const Ptr<Actions::Action>& action);
	/// set the actionlist
	void SetActionList(const Util::Array<Ptr<Actions::Action> >& actions);
    /// get the actionlist
	const Util::Array<Ptr<Actions::Action> >& GetActionList();
    /// add a child to the childlist
	void AddChildTake(const Ptr<DialogTake>& child);
    /// get the childlist
	const Util::Array<Ptr<DialogTake> >& GetChildTakes();
    /// set the condition for this take
    void SetCondition(const Ptr<Conditions::Condition>& condition);
	/// get the condition for this take 
	const Ptr<Conditions::Condition>& GetCondition() const;
    /// return if take has a condition
    bool HasCondition(); 
	/// get the count of children takes
	int GetNumChildTakes() const;
	/// executes the actions of this take
	void ExecuteActions();
    /// executes the assert functions from the members of this take
    bool Assert(const Ptr<Script::InfoLog>& infoLog);
    /// convert string to take type
    static Type StringToType(const Util::String& str);

private:
	Util::Guid guid;
	Util::String id;
	Util::String speaker;
    Util::String emote;
    Util::String sound;
    Util::String shortText;
    Util::Guid locaId;
    Util::Guid shortTextLocaId;
    Timing::Time timestamp;
	Type type;
    Util::Guid actionRef;
    Util::Guid actionBlock;
	Util::Array<Util::String> textList;
	Ptr<Conditions::Condition> condition;
	Util::Array<Ptr<Actions::Action> > actionList;
	Util::Array<Ptr<DialogTake> > childTakes; // must be a c-pointer! there may be circles of takes 
};

} // namespace Script
//------------------------------------------------------------------------------