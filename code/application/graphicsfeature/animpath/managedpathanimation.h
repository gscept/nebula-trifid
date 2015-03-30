#pragma once
//------------------------------------------------------------------------------
/**
	@class GraphicsFeature::ManagedPathAnimation
	
	Managed resource type of the PathAnimation class.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "resources/managedresource.h"
#include "pathanimation.h"
namespace GraphicsFeature
{
class ManagedPathAnimation : public Resources::ManagedResource
{
	__DeclareClass(ManagedPathAnimation);

public:
	const Ptr<GraphicsFeature::PathAnimation>& GetPathAnimation() const;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<GraphicsFeature::PathAnimation>&
ManagedPathAnimation::GetPathAnimation() const
{
	return this->GetLoadedResource().downcast<GraphicsFeature::PathAnimation>();
}
} // namespace GraphicsFeature