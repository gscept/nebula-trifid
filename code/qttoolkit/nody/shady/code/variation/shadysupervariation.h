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
	/// set target language string (with version)
	void SetTarget(const Util::String& target);
	/// get target language string
	const Util::String& GetTarget() const;
    /// sets source header for given language
    void SetHeader(const IO::URI& path, const Shady::Language& language);
    /// gets template for given source
    const IO::URI& GetHeader(const Shady::Language& language);

    /// sets defines used by this super variation
    void SetDefines(const Util::String& defs);
    /// get defines
    const Util::String& GetDefines() const;
	
	/// add include directory
	void AddInclude(const Util::String& language, const IO::URI& path);
	/// get include directories
	const Util::Array<IO::URI>& GetIncludes(const Util::String& language) const;

private:
    Util::String defines;
	Util::String target;
	IO::URI templates[NumLanguages];
    IO::URI headers[NumLanguages];
	Util::Dictionary<Util::String, Util::Array<IO::URI>> includeDirectories;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
ShadySuperVariation::SetTemplate(const IO::URI& path, const Shady::Language& language)
{
	this->templates[language] = path;
}

//------------------------------------------------------------------------------
/**
*/
inline const IO::URI&
ShadySuperVariation::GetTemplate(const Shady::Language& language)
{
    return this->templates[language];
}

//------------------------------------------------------------------------------
/**
*/
inline void
ShadySuperVariation::SetTarget(const Util::String& target)
{
	this->target = target;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShadySuperVariation::GetTarget() const
{
	return this->target;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ShadySuperVariation::SetHeader(const IO::URI& path, const Shady::Language& language)
{
    this->headers[language] = path;
}

//------------------------------------------------------------------------------
/**
*/
inline const IO::URI&
ShadySuperVariation::GetHeader(const Shady::Language& language)
{
    return this->headers[language];
}
//------------------------------------------------------------------------------
/**
*/
inline void
ShadySuperVariation::SetDefines(const Util::String& defs)
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

//------------------------------------------------------------------------------
/**
*/
inline void
ShadySuperVariation::AddInclude(const Util::String& language, const IO::URI& path)
{
	if (!this->includeDirectories.Contains(language))
	{
		this->includeDirectories.Add(language, Util::Array<IO::URI>());
	}
	this->includeDirectories[language].Append(path);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<IO::URI>&
ShadySuperVariation::GetIncludes(const Util::String& language) const
{
	n_assert(this->includeDirectories.Contains(language));
	return this->includeDirectories[language];
}

} // namespace Shady
//------------------------------------------------------------------------------