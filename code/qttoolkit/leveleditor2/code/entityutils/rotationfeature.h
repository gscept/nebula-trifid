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

	/// setup transform feature
	void Setup();

    /// Checks the screen position of the mouse and tries to lock mouse input to feature
    void StartDrag();
    /// Recalculate the rotation of the feature
    void Drag();

    /// Renders the handles for the user input
    void RenderHandles();
	
	/// decompose matrix into angles in all axis
	void GetAnglesFromMat(const Math::matrix44& i_m, float& x, float& y, float& z);

private:
    
    /// Helper method to get the closest intersection point of given line with given sphere
    static Math::vector GetSphereIntersectionPoint(const Math::line& ray,const Math::vector& location, float radius);
    
    /// computes the handles of the feature
    void UpdateHandlePositions();

    /// generate a vector used to compute rotation delta in drag mode.
    Math::vector ComputeDragVector(const Math::line& ray, DragMode mode);

	/// calculate plane intersection boolean between two planes and return the intersecting line
	bool PlaneIntersect(const Math::plane &p1, const Math::plane &p2, Math::line& l);

	struct MeshBundle
	{
		Ptr<CoreGraphics::VertexBuffer> vbo;
		Math::bbox box;
	};

	/// create half circle vbo
	MeshBundle CreateHalfCircle(const Util::Array<CoreGraphics::VertexComponent>& comps);
	/// create full circle vbo
	MeshBundle CreateCircle(const Util::Array<CoreGraphics::VertexComponent>& comps);
	Util::FixedArray<Ptr<Materials::SurfaceInstance>> handleSurfaces;

	Math::matrix44 lastStartDragDeltaMatrix;

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


