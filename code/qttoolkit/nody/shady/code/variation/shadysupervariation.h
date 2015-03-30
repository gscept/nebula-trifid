#pragma once
//------------------------------------------------------------------------------
/**
    @class Shady::ShadySuperVariation
    
    Implements a specialized super variation which is to be used for shader generation.
    
    (C) 2014 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "variation/supervariation.h"
#include "shadydefs.h"
#include "io/uri.h"
namespace Shady
{
class ShadySuperVariation : public Nody::SuperVariation
{
	__DeclareClass(ShadySuperVariation);
public:
	/// constructor
	ShadySuperVariation();
	/// destructor
	virtual ~ShadySuperVariation();

	/// sets source code for given shader language
	void SetTemplate(const IO::URI& path, const Shady::Language& language);
    /// gets template for given source
    const IO::URI& GetTemplate(const Shady::Language& language);
    /// sets source header for given language
    void SetHeader(const IO::URI& path, const Shady::Language& language);
    /// gets template for given source
    const IO::URI& GetHeader(const Shady::Language& language);

    /// sets defines used by this super variation
    void SetDefines(const Util::String& defs);
    /// get defines
    const Util::String& GetDefines() const;

private:
    Util::String defines;
	IO::URI templates[NumLanguages];
    IO::URI headers[NumLanguages];
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
ShadySuperVariation::SetTemplate( const IO::URI& path, const Shady::Language& language )
{
	this->templates[language] = path;
}

//------------------------------------------------------------------------------
/**
*/
inline const IO::URI& 
ShadySuperVariation::GetTemplate( const Shady::Language& language )
{
    return this->templates[language];
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ShadySuperVariation::SetHeader( const IO::URI& path, const Shady::Language& language )
{
    this->headers[language] = path;
}

//------------------------------------------------------------------------------
/**
*/
inline const IO::URI& 
ShadySuperVariation::GetHeader( const Shady::Language& language )
{
    return this->headers[language];
}
//------------------------------------------------------------------------------
/**
*/
inline void 
ShadySuperVariation::SetDefines( const Util::String& defs )
{
    this->defines = defs;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
ShadySuperVariation::GetDefines() const
{
    return this->defines;
}
} // namespace Shady
//------------------------------------------------------------------------------