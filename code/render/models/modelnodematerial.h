#pragma once
//------------------------------------------------------------------------------
/**
    @class Models::ModelNodeMaterial
  
  ModelNodeMaterial defines a material for a model, which is then used in a render pass. 
  There is no hardcoded set of ModelNodeMaterial, but there is a central registry which 
  converts string into binary indices, and guarantees that the returned indices for a 
  name are the same for the lifetime of the application. Very much like the ModelNodeType.
    
    (C) 2011-2013 Individual contributors, see AUTHORS file Luleå Tekniska Högskola
*/
#include "core/types.h"
#include "util/stringatom.h"
#include "util/array.h"
#include "util/dictionary.h"

//------------------------------------------------------------------------------
namespace Models
{
class ModelNodeMaterial
{
public:
	/// readable name of ModelNodeMaterial
	typedef Util::StringAtom Name;
	/// binary code of ModelNodeMaterial
	typedef IndexT Code;

	/// convert from string
	static Code FromName(const Name& name);
	/// convert to string
	static Name ToName(Code c);
	/// maximum number of different ModelNodeMaterials
	static const IndexT MaxNumModelNodeMaterials = 1024;
	/// invalid model node material code
	static const IndexT InvalidModelNodeMaterial = InvalidIndex;

private:
	friend class ModelServer;

	/// constructor
	ModelNodeMaterial();

	Util::Dictionary<Name, IndexT> nameToCode;
	Util::Array<Name> codeToName;
};

} // namespace Models
//------------------------------------------------------------------------------
