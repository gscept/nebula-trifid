//------------------------------------------------------------------------------
//  script/dialog/dialogtake.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/dialog/dialogtake.h"
#include "scriptfeature/scriptattr/scriptattributes.h"
#include "scriptfeature/log/infolog.h"
#include "scriptfeature/actions/action.h"
#include "scriptfeature/conditions/condition.h"

namespace Script
{
__ImplementClass(Script::DialogTake, 'DITA', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
DialogTake::DialogTake() :
    type(None)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
DialogTake::~DialogTake()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Executes the assert functions from the members of this take.
    Additionally it collects errors in the info log object and returns
    false in case of errors instead of closing the application.
*/
bool 
DialogTake::Assert(const Ptr<Script::InfoLog>& infoLog)
{
    bool noErrors = true;

    // build info string for info log section
    Util::String beginInfo = Util::String();
    beginInfo.Format("<DialogTake GUID='%s' ID='%s' SPEAKER='%s' TYPE='%d'>",
            this->guid.AsString().AsCharPtr(),
            this->id.AsCharPtr(),
            this->speaker.AsCharPtr(),
            this->type);

    infoLog->BeginSection(beginInfo);
    {
        // assert existing actions
        infoLog->BeginSection("<Actions>");
	    for( int i = 0; i < this->actionList.Size(); i++ )
	    {
		    if (!this->actionList[i]->Assert(infoLog))
            {
                noErrors = false;
            }
	    }
        infoLog->EndSection("</Actions>");

        // assert existing conditions
        infoLog->BeginSection("<Conditions>");
        if (this->HasCondition())
        {
	        if (!this->condition->Assert(infoLog))
            {
                noErrors = false;
            }
        }
        infoLog->EndSection("</Conditions>");
    }
    infoLog->EndSection("</DialogTake>");

    return noErrors;
}

//------------------------------------------------------------------------------
/**
*/
void 
DialogTake::ExecuteActions()
{
    int i;
    for (i = 0; i < this->actionList.Size(); i++)
	{
		this->actionList[i]->Execute();
	}
}

//------------------------------------------------------------------------------
/**
*/
DialogTake::Type
DialogTake::StringToType(const Util::String& str)
{
    if (str == "StartState") return StartState;
    else if (str == "Take") return Take;
    else if (str == "Response") return Response;
    else
    {
        n_error("DialogTake::StringToType(): invalid take type string '%s'!", str.AsCharPtr());
        return None;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
DialogTake::AddText(const Util::String& text)
{
	n_assert(text.IsValid());
	this->textList.Append( text );
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Util::String>& 
DialogTake::GetTextList()
{
	return this->textList;
}

//------------------------------------------------------------------------------
/**
*/    
void 
DialogTake::SetTimeStamp(Timing::Time t)
{
    this->timestamp = t;
}

//------------------------------------------------------------------------------
/**
*/    
Timing::Time 
DialogTake::GetTimeStamp() const
{
    return this->timestamp;
}

//------------------------------------------------------------------------------
/**
*/
void
DialogTake::AddAction(const Ptr<Actions::Action>& action)
{
	n_assert(action);
	this->actionList.Append( action );
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Ptr<Actions::Action> >&
DialogTake::GetActionList()
{
	return this->actionList;
}

//------------------------------------------------------------------------------
/**
*/
void
DialogTake::SetActionList(const Util::Array<Ptr<Actions::Action> >& actions)
{
    this->actionList.Clear();
	this->actionList = actions;
}

//------------------------------------------------------------------------------
/**
*/
void
DialogTake::AddChildTake(const Ptr<DialogTake>& child)
{
	n_assert(child);
	this->childTakes.Append( child );
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Ptr<DialogTake> >& 
DialogTake::GetChildTakes()
{
	return this->childTakes;
}

//------------------------------------------------------------------------------
/**
*/
void
DialogTake::SetGuid(const Util::Guid& guid)
{
	n_assert(guid.IsValid());
    this->guid = guid;
}

//------------------------------------------------------------------------------
/**
*/
const Util::Guid&
DialogTake::GetGuid() const
{
    return this->guid;
}

//------------------------------------------------------------------------------
/**
*/
void
DialogTake::SetId(const Util::String& id)
{
	n_assert(id.IsValid());
    this->id = id;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
DialogTake::GetId() const
{
	n_assert(this->id.IsValid());
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
void
DialogTake::SetEmote(const Util::String& emoteStr)
{
    this->emote = emoteStr;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
DialogTake::GetEmote() const
{
    return this->emote;
}

//------------------------------------------------------------------------------
/**
*/
void
DialogTake::SetSpeaker(const Util::String& speaker)
{
	n_assert(speaker.IsValid());
    this->speaker = speaker;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
DialogTake::GetSpeaker() const
{
	n_assert(this->speaker.IsValid());
    return this->speaker;
}

//------------------------------------------------------------------------------
/**
*/
void
DialogTake::SetSound(const Util::String& sound)
{
    this->sound = sound;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
DialogTake::GetSound() const
{
    return this->sound;
}

//------------------------------------------------------------------------------
/**
*/
void
DialogTake::SetShortText(const Util::String& shortText)
{
    this->shortText = shortText;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
DialogTake::GetShortText() const
{
    return this->shortText;
}

//------------------------------------------------------------------------------
/**
*/
void
DialogTake::SetType(const Type type)
{
    this->type = type;
}

//------------------------------------------------------------------------------
/**
*/
DialogTake::Type 
DialogTake::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
int 
DialogTake::GetNumChildTakes() const
{
	return this->childTakes.Size();
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Conditions::Condition>&
DialogTake::GetCondition() const 
{
	return this->condition;
}
//------------------------------------------------------------------------------
/**
*/
bool
DialogTake::HasCondition() 
{
    return this->condition.isvalid();
}
//------------------------------------------------------------------------------
/**
*/
void 
DialogTake::SetCondition(const Ptr<Conditions::Condition>& condition)
{
    n_assert(0 != condition);
    this->condition = condition;
}

//------------------------------------------------------------------------------
/**
*/
void 
DialogTake::SetActionRef(const Util::Guid& guid)
{
    this->actionRef = guid;
}

//------------------------------------------------------------------------------
/**
*/
const Util::Guid& 
DialogTake::GetActionRef() const
{
    return this->actionRef;
}

//------------------------------------------------------------------------------
/**
*/
void 
DialogTake::SetActionBlock(const Util::Guid& guid)
{
    this->actionBlock = guid;
}

//------------------------------------------------------------------------------
/**
*/
const Util::Guid&  
DialogTake::GetActionBock() const
{
    return this->actionBlock;
}

} // namespace Script