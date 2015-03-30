//------------------------------------------------------------------------------
//  posteffects/posteffectsentity.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "posteffectentity.h"

namespace PostEffect
{
__ImplementClass(PostEffect::PostEffectEntity, 'PEEN', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
PostEffectEntity::PostEffectEntity() :   
    priority(0),
	isDirty(false),
    isAttached(false),
    isEnabled(true),
    defaultEntity(false),
    shapeType(Sphere)
{
	this->params.Init();    
}

//------------------------------------------------------------------------------
/**
*/
PostEffectEntity::~PostEffectEntity()
{
    n_assert(!this->isAttached);

	this->params.Discard();    
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectEntity::OnAttach()
{
    n_assert(!this->isAttached);
    this->isAttached = true;
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectEntity::OnRemove()
{
    n_assert(this->isAttached);
    this->isAttached = false;
}

//------------------------------------------------------------------------------
/**
    This checks if the provided point of interest is inside the entity.
*/
bool
PostEffectEntity::IsInside(const Math::point& pos) const
{
    // transform point into local space, note that the
    // transform's scale can be non-uniform!
    Math::vector localPos = Math::matrix44::transform(pos, this->invTransform);

    // check if inside based on shape type
    if (Sphere == this->shapeType)
    {
        // inside ellipsoid?
        if (localPos.length() <= 1.0f)
        {
            return true;
        }
    }
    else
    {
        // inside box?
        if ((localPos.x() >= -0.5f) && (localPos.x() <= 0.5f) &&
            (localPos.y() >= -0.5f) && (localPos.y() <= 0.5f) &&
            (localPos.z() >= -0.5f) && (localPos.z() <= 0.5f))
        {
            return true;
        }
    }
    // fall through: not inside
    return false;
}

} // namespace PostEffects

