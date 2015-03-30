#pragma once
//------------------------------------------------------------------------------
/**
    @class Locale:Server
    @ingroup Locale

    The nLocaleServer object simplifies localization of Nebula2 applications
    to international markets. It reads a "locale table", which is an 
    Excel spreadsheet in XML format, which contains 2 columns for original
    text and localized text. Actual translation is done through the 
    nLocale() macro.

    It's also possible to add a variable argument list by using the
    nLocaleArgs() macro. Argument statements in the texts will be replaced by
    these arguments. The argument statements allow you to specify which argument
    has to be used at the statement's position and how the argument has to be
    interpreted at this position (e.g. interger or string).
    
    Possible statements:

    "...{<ArgIdx>:s}..." for string arguments
    "...{<ArgIdx>:l}..." for string arguments which also will be localized
    "...{<ArgIdx>:i}..." for integer arguments
    "...{<ArgIdx>:p:<plural choice>}..." (see PluralChoice(...))

    As id string l will be interpreted as string (s) and p will be interpreted
    as interger (i). In the localized text string the 'l' and the 'p' statement
    will be handled specific. Therefore in the id string can be specified 's'
    for instance and in the localized text string the translator can decide if
    the argument has to be translated too by using 'l' instead of 's'.
*/
#include "util/dictionary.h"
#include "io/uri.h"
#include "core/ptr.h"
#include "core/refcounted.h"
#include "core/singleton.h"

//------------------------------------------------------------------------------
namespace Locale
{
class LocaleServer : public Core::RefCounted
{
    __DeclareClass(LocaleServer);
    __DeclareSingleton(LocaleServer);
    
public:
    /// constructor
    LocaleServer();
    /// destructor
    virtual ~LocaleServer();
    /// open the server
    bool Open();
    /// close server, cleanup all
    void Close();
    /// is server open?
    bool IsOpen() const;
    /// get localized text for id
    Util::String GetLocaleText(const Util::String& id);
    /// get localized text for id in female version if possible
    Util::String GetLocaleTextFemale(const Util::String& id);
    /// get localized text for id with optional arguments
    Util::String __cdecl GetLocaleTextArgs(const char* id, ...);
    /// load locale data from locale Server
    void AddLocaleDBTable(const Util::String& tableName, const IO::URI& dbUri);
    /// clears the complete locale data dictionary
    void ClearLocaleData();
    /// reload loca db (db only)
    void ReloadLocaDB();
    
private:
    /// load locale data from locale Server
    void LoadLocaleDataFromDB(const Util::String& tableName, const IO::URI& dbUri);
    /// replace special characters
    Util::String ParseText(const Util::String& text);
    /// analyzes argument statements and puts results to 'argTypes'
    bool AnalyzeArgumentStatements(const Util::String& str);
    /// gets arguments and interprets them with help of 'argTypes' storing arguments to 'argInts' and 'argStrs'
    void InterpretArguments(va_list argList);
    /// builds the localized text by replacing argument statements with arguments
    Util::String BuildLocalizedText(const Util::String& str);
    /// chooses the part of the plural choice string which fits best to the amount
    static Util::String PluralChoice(int n, const Util::String& pluralChoiceText);
    // argument indices and argument types
    Util::Dictionary<int, char> argTypes;
    // argument indices and integer values of integer arguments
    Util::Dictionary<int, int> argInts;
    // argument indices and string values of string arguments
    Util::Dictionary<int, Util::String> argStrs;

    struct LocaPath
    {
        Util::String path;
        Util::String tableName;
    };

    // string with error message (will be set if error occures in sub method)
    Util::String errMsg;

    bool isOpen;
    Util::Array<LocaPath> fileList; // list of loaded loca paths
    Util::Dictionary<Util::String, Util::String> dictionary;
};

//------------------------------------------------------------------------------
/**
    defines the short form of the text lookup
*/
#define __Locale(text) Locale::LocaleServer::Instance()->GetLocaleText(text).AsCharPtr()
#define __LocaleF(text) Locale::LocaleServer::Instance()->GetLocaleTextFemale(text).AsCharPtr()
#define __LocaleArgs(text, ...) Locale::LocaleServer::Instance()->GetLocaleTextArgs(text, ## __VA_ARGS__).AsCharPtr()

//------------------------------------------------------------------------------
/**
*/
inline
bool
LocaleServer::IsOpen() const
{
    return this->isOpen;    
}

} // namespace Locale
//------------------------------------------------------------------------------
