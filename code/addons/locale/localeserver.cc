//------------------------------------------------------------------------------
//  locale/localeserver_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "locale/localeserver.h"
#include "locale/localeattributes.h"
#include "db/reader.h"
#include "db/sqlite3/sqlite3database.h"
#include "db/sqlite3/sqlite3factory.h"
#include "io/filestream.h"
#include "io/ioserver.h"
#include "io/textreader.h"
#include "io/textwriter.h"

namespace Locale
{
__ImplementClass(Locale::LocaleServer, 'LOCA', Core::RefCounted);
__ImplementSingleton(Locale::LocaleServer);

using namespace Db;
using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
LocaleServer::LocaleServer() :
    isOpen(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
LocaleServer::~LocaleServer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
LocaleServer::Open()
{
    n_assert(!this->IsOpen());

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
String
LocaleServer::ParseText(const String& text)
{
    // replace \n and \t
    String str(text);
    str.SubstituteString("\\t", "\t");
    str.SubstituteString("\\n", "\n");
    return str;
}

//------------------------------------------------------------------------------
/**
*/
void
LocaleServer::Close()
{
    n_assert(this->IsOpen());
    
    this->dictionary.Clear();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
String
LocaleServer::GetLocaleText(const String& id)
{
    n_assert(this->IsOpen());
    if(!id.IsValid())
    {
        return "";
    }
    
    String idStr(id);
    if (this->dictionary.Contains(idStr))
    {
        return this->dictionary[idStr];
    }
    else
    {
        n_printf("WARNING: Locale::LocaleServer: id string '%s' not found!!\n", id.AsCharPtr());
        
        static String msg;
        msg = "LOCALIZE: '";
        msg.Append(idStr);
        msg.Append("'");
        return msg;
    }
}

//------------------------------------------------------------------------------
/**
*/
String
LocaleServer::GetLocaleTextFemale(const String& id)
{
    n_assert(this->IsOpen());
    if(!id.IsValid())
    {
        return "";
    }

    String idStr = id;
    idStr.Append("-female");
    if (this->dictionary.Contains(idStr))
    {
        return this->dictionary[idStr];
    }
    else
    {
        return this->GetLocaleText(id);
    }
}

//------------------------------------------------------------------------------
/**
    Analyzes argument statements of id which can be:

    "...{<ArgIdx>:s}..." for string arguments
    "...{<ArgIdx>:l}..." for string arguments which also will be localized
    "...{<ArgIdx>:i}..." for integer arguments

    The argument index must be of type integer ([0-9]*).

    Gets the locale text with id and replaces the argument statements in the
    locale text with the arguments.

    Additionally it's possible to specify a plural choice statement:

    "...{<ArgIdx>:p:<plural choice>}..." (see PluralChoice(...))
*/
String __cdecl
LocaleServer::GetLocaleTextArgs(const char* id, ...)
{
    n_assert(this->IsOpen());

    // id string
    String idStr(id);
    n_assert(idStr.IsValid());
    // replaced id
    String text = "";
    // localized text (text with replaced argument statements)
    String localeText = "";

    // analyze argument statements
    if (!this->AnalyzeArgumentStatements(id))
    {
        n_error("Locale::LocaleServer: %s", this->errMsg.AsCharPtr());
        return "Localization Error:" + idStr;
    }

    // get arguments and interpret them with help of 'argTypes'
    va_list vArgList;
    va_start(vArgList, id);
    this->InterpretArguments(vArgList);
    va_end(vArgList);

    // get text with id
    text = this->GetLocaleText(idStr);

    // build localized text by replacing argument statements in text with arguments
    localeText = this->BuildLocalizedText(text);
    if (localeText.IsValid())
    {
        return localeText;
    }
    else
    {
        n_error("Locale::LocaleServer: %s", this->errMsg.AsCharPtr());
        return "Localization Error:" + idStr;
    }
}

//------------------------------------------------------------------------------
/**
    Analyzes argument statements and puts results to 'argTypes'.

    Will return 'false' if there are errors while analyzing process (e.g. syntax
    errors). Will place error message in 'errMsg'.

    Type 'l' for localisation will be handled as 's' (string).
    Type 'p' for plural choice will be handled as 'i' (integer).
*/
bool 
LocaleServer::AnalyzeArgumentStatements(const String& str)
{
    n_assert(str.IsValid());

    this->argTypes.Clear();

    IndexT idxOpenBracket = 0; // index of last found open bracket
    IndexT idxColon = 0; // index of last found colon

    String argIdxStr = ""; // argument index (between open bracket and colon)
    IndexT argIdx = InvalidIndex; // argument index as integer

    char argType = '\0';    // argument type (character after colon)

    while (true)
    {
        // find open bracket
        idxOpenBracket = str.FindCharIndex('{', idxColon);
        // if there is no open bracket: stop
        if (idxOpenBracket == InvalidIndex) break;

        // find colon
        idxColon = str.FindCharIndex(':', idxOpenBracket);
        // if there is no colon: stop
        if (idxColon == InvalidIndex)
        {
            this->errMsg.Format("Syntax error in id string '%s'! Colon missing.", str);
            return false;
        }

        // get argument index as string
        argIdxStr = str.ExtractRange(idxOpenBracket + 1, idxColon - idxOpenBracket - 1);
        // check if argument index string is valid for converting to integer
        SizeT argIdxStrLength = argIdxStr.Length();
        IndexT idx;
        for (idx = 0; idx < argIdxStrLength; idx++)
        {
            if (argIdxStr[idx] < '0' || argIdxStr[idx] > '9')
            {
                this->errMsg.Format("Syntax error in id string '%s'! Argument index at position '%d' invalid.", str, idxOpenBracket);
                return false;
            }
        }
        // convert argument index string to integer
        argIdx = argIdxStr.AsInt();

        // get argument type
        argType = str[idxColon + 1];
        // argument type l (localize) also is of type string
        if ('l' == argType)
        {
            argType = 's';
        }
        // argument type p (plural choice) also is of type integer
        if ('p' == argType)
        {
            argType = 'i';
        }
        // interpret argument type
        switch (argType)
        {
        case 's': // string type
        case 'i': // integer type
            // add argument index and type to argTypes dictionary
            if (!argTypes.Contains(argIdx))
            {
                // argument index is not in direcory: add index and type to dictionary
                argTypes.Add(argIdx, argType);
            }
            else if (argType != argTypes[argIdx])
            {
                // argument index is in direcory: generate error message if types are not equal (type of argument is ambiguous)
                this->errMsg.Format("Parsing error in id string '%s'! Different types for argument %d specified.", str, argIdx);
                return false;
            }
            break;

        default:
            this->errMsg.Format("Syntax error in id string '%s'! Type '%c' of argument %d invalid.", str, argType, argIdx);
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Gets arguments and interprets them with help of 'argTypes' storing arguments
    to 'argInts' and 'argStrs'.
*/
void
LocaleServer::InterpretArguments(va_list vArgList)
{
    IndexT argIdx = 0; // argument index
    char argType = '\0'; // argument type

    String strArg = "";

    this->argStrs.Clear();
    this->argInts.Clear();

    while(true)
    {
        if (argTypes.Contains(argIdx))
        {
            // get argument type
            argType = argTypes[argIdx];

            // interpret argument type
            switch (argType)
            {
            case 's': // string type
                // get next argument, interpret as string and add to argStrs dictionary
                strArg = va_arg(vArgList, char*);
                argStrs.Add(argIdx, strArg);
                break;

            case 'i': // integer type
                // get next argument, interpret as integer and add to argInts dictionary
                argInts.Add(argIdx, va_arg(vArgList, int));
                break;

            default: // should be impossible
                n_error("Locale::LocaleServer: Internal error!\nType '%c' of argument %d invalid.", argType, argIdx);
                break;
            }

            argIdx++;
        }
        else
        {
            // stop searching for arguments if argument index cannot be found in argTypes dictionary
            break;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Builds the localized text by replacing argument statements with arguments.

    Returns "" if there are errors.
*/
String
LocaleServer::BuildLocalizedText(const String& str)
{
    n_assert(str.IsValid());

    IndexT idxOpenBracket = 0; // index of last found open bracket
    IndexT idxAfterCloseBracket = 0; // index of last found close bracket + 1
    IndexT idxColon = 0; // index of last found colon

    String argIdxStr = ""; // argument index (between open bracket and colon)
    IndexT argIdx = InvalidIndex; // argument index as integer

    char argType = '\0'; // argument type (character after colon)
    String localizedText; // localized text (text with replaced argument statements)

    while (idxAfterCloseBracket < str.Length())
    {
        // find open bracket
        idxOpenBracket = str.FindCharIndex('{', idxAfterCloseBracket);
        // if there is no open bracket stop
        if (idxOpenBracket == InvalidIndex)
        {
            // add rest of text to localized text
            localizedText += str.ExtractToEnd(idxAfterCloseBracket);
            break;
        }

        // add text between close and open bracket to localized text
        localizedText += str.ExtractRange(idxAfterCloseBracket, idxOpenBracket - idxAfterCloseBracket);

        // find colon
        idxColon = str.FindCharIndex(':', idxOpenBracket);
        // if there is no colon: stop
        if (idxColon == InvalidIndex)
        {
            this->errMsg.Format("Syntax error in localized text string '%s'! Colon missing.", str);
            return "";
        }

        // find close bracket
        idxAfterCloseBracket = str.FindCharIndex('}', idxColon) + 1;

        // get argument index as string
        argIdxStr = str.ExtractRange(idxOpenBracket + 1, idxColon - idxOpenBracket - 1);
        // check if argument index string is valid for converting to integer
        SizeT argIdxStrLength = argIdxStr.Length();
        IndexT idx;
        for (idx = 0; idx < argIdxStrLength; idx++)
        {
            if (argIdxStr[idx] < '0' || argIdxStr[idx] > '9')
            {
                this->errMsg.Format("Syntax error in localized text string '%s'! Argument index at position '%d' invalid.", str, idxOpenBracket);
                return "";
            }
        }
        // convert argument index string to integer
        argIdx = argIdxStr.AsInt();

        // get argument type
        argType = str[idxColon + 1];
        // interpret argument type
        switch (argType)
        {
        case 's': // string type
            // check if argument index is in string directory
            if (!argStrs.Contains(argIdx))
            {
                // argument index is not in string direcory: generate error message
                this->errMsg.Format("Argument error in localized text string '%s'! Argument %d of type string was not available in id string.", str, argIdx);
                return "";
            }
            else
            {
                // add argument to localized text
                localizedText += argStrs[argIdx];
            }
            break;
        case 'l': // string type (argument string have be localized, too)
            // check if argument index is in string directory
            if (!argStrs.Contains(argIdx))
            {
                // argument index is not in string direcory: generate error message
                this->errMsg.Format("Argument error in localized text string '%s'! Argument %d of type string was not available in id string. For localization string type is needed.", str, argIdx);
                return "";
            }
            else
            {
                // add argument to localized text
                localizedText += this->GetLocaleText(argStrs[argIdx]);
            }
            break;
        case 'i': // integer type
            // check if argument index is in integer directory
            if (!argInts.Contains(argIdx))
            {
                // argument index is not in integer direcory: generate error message
                this->errMsg.Format("Argument error in localized text string '%s'! Argument %d of type integer was not available in id string.", str, argIdx);
                return "";
            }
            else
            {
                // add argument to localized text
                localizedText += String::FromInt(argInts[argIdx]);
            }
            break;

        case 'p': // plural choice
            // check if argument index is in integer directory
            if (!argInts.Contains(argIdx))
            {
                // argument index is not in integer direcory: generate error message
                this->errMsg.Format("Argument error in localized text string '%s'! Argument %d of type integer was not available in id string. For plural choice integer type is needed.", str, argIdx);
                return "";
            }
            else
            {
                // add plural choice to localized text
                localizedText += this->PluralChoice(argInts[argIdx], str.ExtractRange(idxColon + 3, idxAfterCloseBracket - idxColon - 4));
            }
            break;

        default:
            this->errMsg.Format("Argument error in localized text string '%s'! Type '%c' of argument %d invalid.", str, argType, argIdx);
            return "";
        }
    }

    return localizedText;
}

//------------------------------------------------------------------------------
/**
    Chooses the part of the plural choice string which fits best to the amount.
    
    The plural choice string must match to this pattern:

    [<is zero text>|]<is one text>|<else text>

    Example 1:

    PluralChoice(1, "Child|Children"); // Child
    PluralChoice(4, "Child|Children"); // Children

    Example 2:

    PluralChoice(0, "no Children|one Child|many Children"); // no Children
    PluralChoice(1, "no Children|one Child|many Children"); // one Child
    PluralChoice(5, "no Children|one Child|many Children"); // many Children
*/
String
LocaleServer::PluralChoice(int amount, const String& pluralChoice)
{
    n_assert(pluralChoice.IsValid());

    Array<String> choices = pluralChoice.Tokenize("|");

    switch (choices.Size())
    {
    case 2:

        if      ( 1 == amount)  return choices[0]; // is one text
        else                    return choices[1]; // else text

        break;

    case 3:

        if      ( 0 == amount)  return choices[0]; // is zero text
        else if ( 1 == amount)  return choices[1]; // is one text
        else                    return choices[2]; // else text

        break;
    
    default:

        n_error("Locale::LocaleServer: Syntax error in plural choice '%s'!", pluralChoice.AsCharPtr());
        return pluralChoice;

        break;
    }
}

//------------------------------------------------------------------------------
/**
    Add a locale table from db
*/
void
LocaleServer::AddLocaleDBTable(const String& tableName, const IO::URI& dbUri)
{
    LocaPath newPath;
    newPath.path = dbUri.AsString();
    newPath.tableName = tableName;
    this->fileList.Append(newPath);

    this->LoadLocaleDataFromDB(tableName, dbUri);
}

//------------------------------------------------------------------------------
/**
    Clears the complete locale data dictionary
*/
void
LocaleServer::ClearLocaleData()
{
    this->dictionary.Clear();
    this->fileList.Clear();
}

//------------------------------------------------------------------------------
/**
    reload locaDB
*/
void
LocaleServer::ReloadLocaDB()
{
    this->dictionary.Clear();
    
    SizeT numFilePaths = this->fileList.Size();
    IndexT idx;
    for(idx = 0; idx < numFilePaths; idx++)
    {
        this->LoadLocaleDataFromDB(this->fileList[idx].tableName, this->fileList[idx].path);
    }
}

//------------------------------------------------------------------------------
/**
    Add a locale table from db
*/
void
LocaleServer::LoadLocaleDataFromDB(const String& tableName, const IO::URI& dbUri)
{
    n_assert(dbUri.IsValid());
    __MEMORY_CHECKPOINT("> Locale::LocaleServer::LoadLocaleDataFromDB()");

    Ptr<Sqlite3Factory> dbFactory = 0;
    if(!Sqlite3Factory::HasInstance())
    {
        dbFactory = Sqlite3Factory::Create();
    }
    else
    {
        dbFactory = Sqlite3Factory::Instance();
    }
    
    Ptr<Sqlite3Database> db = dbFactory->CreateDatabase().downcast<Sqlite3Database>();
    db->SetURI(dbUri);
    db->SetAccessMode(Database::ReadOnly);
    db->SetExclusiveMode(true);
    db->SetIgnoreUnknownColumns(false);
    if (db->Open())
    {
        if (db->HasTable(tableName))
        {
            Ptr<Reader> reader = Reader::Create();   
            reader->SetDatabase(db.cast<Database>());
            reader->SetTableName(tableName);
            if (reader->Open())
            {
                SizeT numRows = reader->GetNumRows();
                this->dictionary.Reserve(numRows);
                IndexT rowIndex;
                for (rowIndex = 0; rowIndex < numRows; rowIndex++)
                {
                    reader->SetToRow(rowIndex);
                    this->dictionary.Add(reader->GetString(Attr::LocaId),  reader->GetString(Attr::LocaText));
                }
            }
            reader->Close();
        }
        db->Close();
    }
    __MEMORY_CHECKPOINT("< Locale::LocaleServer::LoadLocaleDataFromDB()");
}

}; // namespace Locale
//----------------------------------------------------------------------------