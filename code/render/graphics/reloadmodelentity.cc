//------------------------------------------------------------------------------
//  reloadmodelentity.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/reloadmodelentity.h"

namespace Graphics
{
__ImplementClass(Graphics::ReloadModelEntity, 'RMDE', Graphics::ModelEntity);

using namespace Models;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
ReloadModelEntity::ReloadModelEntity()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ReloadModelEntity::~ReloadModelEntity()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ReloadModelEntity::Reload()
{
	if (this->modelInstance.isvalid())
	{
		if (this->modelInstance->IsValid())
		{
			// hmm... this doesn't look very elegant...
			this->modelInstance->GetModel()->DiscardInstance(this->modelInstance);
		}
	}	
	this->charInst = 0;		
	this->modelInstance = 0;
	this->isValid = false;	
}

} // namespace Graphics
