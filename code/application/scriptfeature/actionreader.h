#pragma once
//------------------------------------------------------------------------------
/**
    @class Script::ActionReader

    Provides functionality to read and write masterEvents and conditions from 
    and to strings.

    Note: Be aware that SetString will already tokenize the given string and therefore
    a lot of the work is done here (dont call it too often).

    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "scriptfeature/actions/action.h"
#include "scriptfeature/conditions/condition.h"
#include "math/vector.h"

//------------------------------------------------------------------------------
namespace Script
{
    const Util::String LIMITER = ";";
    
class ActionReader : public Core::RefCounted
{
    __DeclareClass(ActionReader);

public:
    /// constructor
    ActionReader();
    /// destructor
    virtual ~ActionReader();

    /// @group read mode
    //@{
    /// set the string to be parsed (read mode)
    void SetString(const Util::String& actionString);
    /// generate an action from the string (read mode)
    Ptr<Actions::Action> GetAction();
    /// generate an action from the string (read mode)
    Ptr<Conditions::Condition> GetCondition();

    /// get classname
    Util::String GetClass();
    /// get an int
    int GetInt();
    /// get a bool
    bool GetBool();
    /// get a float
    float GetFloat();
    /// get a string
    Util::String GetString();
    /// get an entity (might be 0)
    Ptr<Game::Entity> GetEntity();
    /// get a vector3
	Math::vector GetVector();
    /// get a vector4
	Math::float4 GetFloat4();

    /// return content version of the action string
    int Version() const;
    //@}

    /// @group write mode
    //@{
    /// get the action string (write mode)
    const Util::String& GetActionString() const;

    /// put the classname
    void PutClass(Ptr<Core::RefCounted>& refcounted);
    /// put an int
    void PutInt(int value);
    /// put a bool
    void PutBool(bool value);
    /// put a float
    void PutFloat(float value);
    /// put a string
    void PutString(const Util::String& text);
    /// put an entity (might be 0)
    void PutEntity(Ptr<Game::Entity> entity);
    /// put a vector3
	void PutVector(const Math::vector& vec);
    /// put a vector4
	void PutFloat4(const Math::float4& vec);
    /// put 
    void PutVersion(int version);
    //@}

protected:
    /// helper function to extract data from string until limiter
    Util::String NextToken(const Util::String& sourceString, int& startIndex, char limiter);
    /// helper function to extract data from string until " " or ";"
    Util::String NextToken(const Util::String& sourceString, int& startIndex);

    Util::Array<Util::String> tokens;
    int currToken;          
    int actionStringVersion;
    Util::String actionString;    
};

//------------------------------------------------------------------------------
/**
*/
inline
void
ActionReader::SetString(const Util::String& actionString)
{
    this->actionString = actionString;
    this->currToken = 0;

    tokens = actionString.Tokenize(LIMITER);

    this->actionStringVersion = -1;
    if (2 < tokens.Size())
    {
        const Util::String& versionTag = tokens[0];
        if (versionTag == "v*")
        {
            this->actionStringVersion = tokens[1].AsInt();
            this->currToken = 2;
        }
    }
#if DSA_DEBUG
    printf("ActionReader::SetString: num tokens: %d\n", tokens.Size());
#endif
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
ActionReader::GetActionString() const
{
    return this->actionString;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ActionReader::PutInt(int value)
{
    Util::String valueStr;
    valueStr.Format("%d", value);
    this->actionString.Append(valueStr);
    this->actionString.Append(LIMITER);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ActionReader::PutBool(bool value)
{
    if (value)
    {
        this->actionString.Append("true");
    }
    else
    {
        this->actionString.Append("false");
    }
    this->actionString.Append(LIMITER);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ActionReader::PutFloat(float value)
{
    Util::String valueStr;
    valueStr.Format("%f", value);
    this->actionString.Append(valueStr);
    this->actionString.Append(LIMITER);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ActionReader::PutString(const Util::String& text)
{
    this->actionString.Append(text);
    this->actionString.Append("*"); // needed to allow empty strings (tokenizer!)
    this->actionString.Append(LIMITER);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ActionReader::GetInt()
{
    n_assert2(this->currToken < this->tokens.Size(), "action reader: no tokens left");
    this->currToken++;
    return this->tokens[this->currToken-1].AsInt();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ActionReader::GetBool()
{
    n_assert2(this->currToken < this->tokens.Size(), "action reader: no tokens left");
    this->currToken++;
    return this->tokens[this->currToken-1].AsBool();
}

//------------------------------------------------------------------------------
/**
*/
inline
float
ActionReader::GetFloat()
{
    n_assert2(this->currToken < this->tokens.Size(), "action reader: no tokens left");
    this->currToken++;
    return this->tokens[this->currToken-1].AsFloat();
}

//------------------------------------------------------------------------------
/**
*/
inline
Util::String
ActionReader::GetString()
{
    n_assert2(this->currToken < this->tokens.Size(), "action reader: no tokens left");
    this->currToken++;
    // remove appended "*" from string (see PutString)
    Util::String quotedString = this->tokens[this->currToken-1];
    quotedString.TerminateAtIndex(quotedString.Length()-1);
    return quotedString;
}

//------------------------------------------------------------------------------
/**
*/
inline
Util::String
ActionReader::GetClass()
{
    n_assert2(this->currToken < this->tokens.Size(), "action reader: no tokens left");
    this->currToken++;
    return this->tokens[this->currToken-1];
}

//------------------------------------------------------------------------------
/**
*/
inline
Math::vector
ActionReader::GetVector()
{
    float x = this->GetFloat();
    float y = this->GetFloat();
    float z = this->GetFloat();
	Math::vector result(x,y,z);
    return result;
}

//------------------------------------------------------------------------------
/**
*/
inline
Math::float4
ActionReader::GetFloat4()
{
    float x = this->GetFloat();
    float y = this->GetFloat();
    float z = this->GetFloat();
    float w = this->GetFloat();
	Math::float4 result(x,y,z,w);
    return result;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ActionReader::PutVector(const Math::vector& vec)
{
    this->PutFloat(vec.x());
    this->PutFloat(vec.y());
    this->PutFloat(vec.z());
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ActionReader::PutFloat4(const Math::float4& vec)
{
    this->PutFloat(vec.x());
    this->PutFloat(vec.y());
    this->PutFloat(vec.z());
    this->PutFloat(vec.w());
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ActionReader::PutVersion(int version)
{
    n_assert(this->actionString.IsEmpty());
    this->actionStringVersion = version;

    this->PutString("v");
    this->PutInt(version);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ActionReader::Version() const
{
    return this->actionStringVersion;
}

}; // namespace Actions
//------------------------------------------------------------------------------
