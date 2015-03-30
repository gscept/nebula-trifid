//------------------------------------------------------------------------------
//  modelnodematerial.cc
//  (C) 2011-2013 Individual contributors, see AUTHORS file Luleå Tekniska Universitet
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "models/modelnodematerial.h"
#include "models/modelserver.h"

namespace Models
{
using namespace Util;

//------------------------------------------------------------------------------
/**
	Private constructor, only the ModelServer may create the central 
	ModelNodeMaterial registry.
*/
ModelNodeMaterial::ModelNodeMaterial()
{
	this->nameToCode.Reserve(MaxNumModelNodeMaterials);
	this->codeToName.Reserve(MaxNumModelNodeMaterials);
}

//------------------------------------------------------------------------------
/**
*/
ModelNodeMaterial::Code
ModelNodeMaterial::FromName(const Name& name)
{
	ModelNodeMaterial& registry = ModelServer::Instance()->modelNodeMaterialRegistry;
	IndexT index = registry.nameToCode.FindIndex(name);
	if (InvalidIndex != index)
	{
		return registry.nameToCode.ValueAtIndex(index);
	}
	else
	{
		// material hasn't been registered yet
		registry.codeToName.Append(name);
		Code code = registry.codeToName.Size() - 1;
		registry.nameToCode.Add(name, code);
		return code;
	}
}

//------------------------------------------------------------------------------
/**
*/
ModelNodeMaterial::Name
ModelNodeMaterial::ToName(Code c)
{
	ModelNodeMaterial& registry = ModelServer::Instance()->modelNodeMaterialRegistry;
	return registry.codeToName[c];	
}

} // namespace Models
