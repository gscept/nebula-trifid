#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::
    
    Used for forced reloading of a modelentity
        
    (C) 2016 Individual contributors, see AUTHORS file
*/
#include "graphics/modelentity.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class ReloadModelEntity : public ModelEntity
{
    __DeclareClass(ReloadModelEntity);
public:
    /// constructor
	ReloadModelEntity();
    /// destructor
    virtual ~ReloadModelEntity();
    
    /// forced reload
	void Reload();
};

} // namespace Graphics
//------------------------------------------------------------------------------    