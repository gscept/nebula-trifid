#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::ScaleFeature
    
    Transform feature for scaling 3D transform matrices.
    Provides handles for scale in 3 axis seperate or simultaneous.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/

#include "transformfeature.h"

//------------------------------------------------------------------------------
namespace LevelEditor2
{
class ScaleFeature : public TransformFeature
{
__DeclareClass(ScaleFeature);
//------------------------------------------------------------------------------
public:

    /// Constructor
    ScaleFeature();
    /// Destructor
    ~ScaleFeature();

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

	Math::vector transform_coordMatrix(Math::matrix44& m, const Math::vector& v);
        
    /// returns true, if mouse is pointing on a handle
	bool IsMouseOverHandle(DragMode handle, const Math::line& worldMouseRay);
	/// returns the handle above which the mouse is currently over
	DragMode GetMouseHandle(const Math::line& worldMouseRay);

    /// decomposes the current transform matrix in translation vector, scale vector and rotation matrix.
    void DecomposeInitialMatrix();
    /// checks current initial matrix and feature scale to compute handle positions in space.
    void UpdateHandlePositions();
	

	Math::vector decomposedTranslation;
    Math::vector decomposedScale;
    Math::matrix44 decomposedRotation;


	Math::point xAxis;
	Math::point yAxis;
	Math::point zAxis;
	Math::point origin;

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

