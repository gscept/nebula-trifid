#pragma once
//------------------------------------------------------------------------------
/**
	@class Leveleditor2::RotationFeature

    Transform feature that handles rotation of matrices

    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/

#include "transformfeature.h"

//------------------------------------------------------------------------------
namespace LevelEditor2
{
class RotationFeature : public TransformFeature
{
__DeclareClass(RotationFeature);
public:

    /// Constructor
    RotationFeature();
    /// Destructor
    ~RotationFeature();

    /// Checks the screen position of the mouse and tries to lock mouse input to feature
    void StartDrag();
    /// Recalculate the rotation of the feature
    void Drag();
    /// Leave drag mode and set drag mode to none
    void ReleaseDrag();

    /// Renders the handles for the user input
    void RenderHandles();
	
	/// decompose matrix into angles in all axis
	void GetAnglesFromMat(const Math::matrix44& i_m, float& x, float& y, float& z);

private:
    
    /// Helper method to get the closest intersection point of given line with given sphere
    static Math::vector GetSphereIntersectionPoint(const Math::line& ray,const Math::vector& location, float radius);
    
	/// returns the handle above which the mouse is currently over
	DragMode GetMouseHandle(const Math::line& worldMouseRay);

    /// computes the handles of the feature
    void UpdateHandlePositions();

    /// generate a vector used to compute rotation delta in drag mode.
    Math::vector ComputeDragVector(const Math::line& ray, DragMode mode);

	/// rotate vector in a circle using an axis ad pivot
	void RotateVector(Math::vector &i_v, Math::vector &axis, float angle);
	/// calculate plane intersection boolean between two planes and return the intersecting line
	bool PlaneIntersect(Math::plane &p1, Math::plane &p2, Math::line& l);

	Math::matrix44 lastStartDragDeltaMatrix;

    Math::vector xAxis;             // handle axes
    Math::vector yAxis;
    Math::vector zAxis;
	Math::vector viewAxis;
    Math::point origin;

    float handleScale;
    float outerCircleScale;
    Math::vector startDragOrientation;

    Math::vector xDragStart;
    Math::vector yDragStart;
    Math::vector zDragStart;

    float handleDistance;

    Math::plane xPlane;              // planes orthogonal to the handle axes
    Math::plane yPlane;
    Math::plane zPlane;
    Math::plane viewPlane;
};


}


