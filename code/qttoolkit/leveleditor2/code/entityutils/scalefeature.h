#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::ScaleFeature
    
    Transform feature for scaling 3D transform matrices.
    Provides handles for scale in 3 axis seperate or simultaneous.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/

#include "transformfeature.h"

//------------------------------------------------------------------------------
namespace LevelEditor2
{
class ScaleFeature : public TransformFeature
{
	__DeclareClass(ScaleFeature);
public:

    /// Constructor
    ScaleFeature();
    /// Destructor
    ~ScaleFeature();

	/// setup transform feature
	virtual void Setup();

    /// Checks the screen position of the mouse and tries to lock mouse input to feature
    void StartDrag();
    /// Recalculate the scale of the feature
    void Drag();
    /// Leave drag mode and set drag mode to none
    void ReleaseDrag();

    /// Renders the handles for the user input
    void RenderHandles();

    /// Sets lock axis mode. Should only called outside StartDrag() and ReleaseDrag().
    void ActivateAxisLocking(bool activate);
    /// Returns true if axis locking is activated
    bool IsAxisLockingActivated();
	/// Update transform from outside
	virtual void UpdateTransform(const Math::matrix44 & transform);


private:

    /// checks current initial matrix and feature scale to compute handle positions in space.
    void UpdateHandlePositions();

	struct MeshBundle
	{
		Ptr<CoreGraphics::VertexBuffer> vbo;
		Ptr<CoreGraphics::IndexBuffer> ibo;
		Math::bbox box;
	};

	/// create box with line handle
	MeshBundle CreateHandle(const Util::Array<CoreGraphics::VertexComponent>& comps);
	/// create box
	MeshBundle CreateBox(const Util::Array<CoreGraphics::VertexComponent>& comps);

	struct SurfaceBundle
	{
		Ptr<Materials::SurfaceInstance> box;
	};
	Util::FixedArray<SurfaceBundle> handleSurfaces;

    float handleScale;

    Math::vector dragStartMouseRayOffset;
    Math::float2 dragStartMousePosition;
    float handleDistance;

    Math::vector scale;

    Math::matrix44 lastStartDragDeltaMatrix;

    bool axisLocking;

};

//------------------------------------------------------------------------------
/**
Activates axis locking. This should only called outside of
StartDrag and ReleaseDrag.
*/
inline void
ScaleFeature::ActivateAxisLocking(bool activate)
{
	this->axisLocking = activate;
}

//------------------------------------------------------------------------------
/**
Returns true if axis locking is activated
*/
inline bool
ScaleFeature::IsAxisLockingActivated()
{
	return this->axisLocking;
}

} // namespace LevelEditor

