#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::VariationDatabase
    
    Contains a database of all variations which are easily accessed.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/singleton.h"
#include "core/refcounted.h"
#include "util/dictionary.h"
#include "util/array.h"
#include "variationloader.h"
#include "supervariationloader.h"
#include "variation.h"

namespace Nody
{
class VariationDatabase : public Core::RefCounted
{
	__DeclareClass(VariationDatabase);
	__DeclareSingleton(VariationDatabase);	
public:
	/// constructor
	VariationDatabase();
	/// destructor
	virtual ~VariationDatabase();
	/// open the VariationDatabase
	bool Open();
	/// close the VariationDatabase
	void Close();
	/// return if VariationDatabase is open
	bool IsOpen() const;

	/// set pointer to variation loader
	void SetVariationLoader(const Ptr<VariationLoader>& loader);
	/// get pointer to variation loader
	const Ptr<VariationLoader>& GetVariationLoader() const;
    /// set pointer to super variation loader
    void SetSuperVariationLoader(const Ptr<SuperVariationLoader>& loader);
    /// get pointer to super variation loader
    const Ptr<SuperVariationLoader>& GetSuperVariationLoader() const;

	/// load variations from path to variations directory
	void LoadVariations(const IO::URI& path);
    /// load supernodes from path
    void LoadSuperVariations(const IO::URI& path);

	/// get list of variations for category
	const Util::Array<Ptr<Variation>>& GetVariationsByCategory(const Util::String& category) const;
	/// gets list of variation categories
	Util::Array<Util::String> GetVariationCategories() const;
    /// get variation by name
    const Ptr<Variation>& GetVariationByName(const Util::String& name) const;

    /// get super variation by name
    const Ptr<SuperVariation>& GetSuperVariationByName(const Util::String& name) const;

private:
	bool isOpen;

	Ptr<VariationLoader> variationLoader;
    Ptr<SuperVariationLoader> superVariationLoader;
	Util::Array<Ptr<Variation>> variationsByIndex;
    Util::Dictionary<Util::String, Ptr<Variation>> variationsByName;
	Util::Dictionary<Util::String, Util::Array<Ptr<Variation>>> variationsByCategory;
    Util::Array<Ptr<SuperVariation>> superVariationsByIndex;
    Util::Dictionary<Util::String, Ptr<SuperVariation>> superVariationsByName;
}; 

//------------------------------------------------------------------------------
/**
*/
inline bool
VariationDatabase::IsOpen() const
{
	return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
VariationDatabase::SetVariationLoader( const Ptr<VariationLoader>& loader )
{
	n_assert(loader.isvalid());
	this->variationLoader = loader;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<VariationLoader>& 
VariationDatabase::GetVariationLoader() const
{
	return this->variationLoader;
}
//------------------------------------------------------------------------------
/**
*/
inline void 
VariationDatabase::SetSuperVariationLoader( const Ptr<SuperVariationLoader>& loader )
{
    n_assert(loader.isvalid());
    this->superVariationLoader = loader;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<SuperVariationLoader>& 
VariationDatabase::GetSuperVariationLoader() const
{
    return this->superVariationLoader;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<Variation>>& 
VariationDatabase::GetVariationsByCategory( const Util::String& category ) const
{
    n_assert(this->variationsByCategory.Contains(category));
    return this->variationsByCategory[category];
}

//------------------------------------------------------------------------------
/**
*/
inline Util::Array<Util::String> 
VariationDatabase::GetVariationCategories() const
{
	return this->variationsByCategory.KeysAsArray();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Variation>& 
VariationDatabase::GetVariationByName( const Util::String& name ) const
{
    n_assert(this->variationsByName.Contains(name));
    return this->variationsByName[name];
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<SuperVariation>& 
VariationDatabase::GetSuperVariationByName( const Util::String& name ) const
{
    n_assert(this->superVariationsByName.Contains(name));
    return this->superVariationsByName[name];
}

} // namespace Nody
//------------------------------------------------------------------------------