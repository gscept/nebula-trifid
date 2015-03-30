#pragma once
//------------------------------------------------------------------------------
/**
    @class Models::ModelNodeType
  
    ModelNodeTypes identify a ModelNode for a specific rendering pass. Works
    the same as shader features, there is no hardcoded set of ModelNodeTypes,
    but there is a central registry which converts string into binary indices,
    and guarantees that the returned indices for a name are the same for
    the lifetime of the application.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/types.h"
#include "util/stringatom.h"
#include "util/array.h"
#include "util/dictionary.h"

//------------------------------------------------------------------------------
namespace Models
{
class ModelNodeType
{
public:
    /// human readable name of a ModelNodeType
    typedef Util::StringAtom Name;
    /// binary code for a ModelNodeType
    typedef IndexT Code;

    /// convert from string
    static Code FromName(const Name& name);
    /// convert to string
    static Name ToName(Code c);
    /// maximum number of different ModelNodeTypes
    static const IndexT MaxNumModelNodeTypes = 16;
    /// invalid model node type code
    static const IndexT InvalidModelNodeType = InvalidIndex;

private:
    friend class ModelServer;

    /// constructor
    ModelNodeType();

    Util::Dictionary<Name, IndexT> nameToCode;
    Util::Array<Name> codeToName;
};

} // namespace Models
//------------------------------------------------------------------------------

