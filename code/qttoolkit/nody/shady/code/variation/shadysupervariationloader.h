#pragma once
//------------------------------------------------------------------------------
/**
    @class Shady::ShadySuperVariationLoader
    
    Loads super variations using the Shady feature set.
    
    (C) 2014 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "variation/supervariationloader.h"
#include "shadysupervariation.h"
#include "shadydefs.h"
#include "io/xmlreader.h"

namespace Shady
{
class ShadySuperVariationLoader : public Nody::SuperVariationLoader
{
	__DeclareClass(ShadySuperVariationLoader);
public:
	/// constructor
	ShadySuperVariationLoader();
	/// destructor
	virtual ~ShadySuperVariationLoader();

	/// load shady variation
	Ptr<Nody::SuperVariation> Load(const Ptr<IO::Stream>& stream);

private:

    /// parse variation
    void ParseVariation(const Ptr<IO::XmlReader>& reader, const Ptr<ShadySuperVariation>& variation);
    /// parse input
    void ParseInput(const Ptr<IO::XmlReader>& reader, const Ptr<ShadySuperVariation>& variation);
    /// parse source
    void ParseSource(const Ptr<IO::XmlReader>& reader, const Ptr<ShadySuperVariation>& variation);
	/// parse include
	void ParseInclude(const Ptr<IO::XmlReader>& reader, const Util::String& language, const Ptr<ShadySuperVariation>& variation);
}; 
} // namespace Shady
//------------------------------------------------------------------------------