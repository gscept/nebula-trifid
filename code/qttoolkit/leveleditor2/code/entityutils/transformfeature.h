#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::TransformFeature

    Defines a standard transform feature object.
    Provides methods for dragging and rendering features.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/

#include "core/refcounted.h"
#include "game/entity.h"
#include "graphics/meshentity.h"

//------------------------------------------------------------------------------
namespace LevelEditor2
{
class TransformFeature : public Core::RefCounted
{
__DeclareClass(TransformFeature);

public:

	enum DragMode
	{
		NONE,
		X_AXIS,
		Y_AXIS,
		Z_AXIS,
		VIEW_AXIS,
		ORIGIN
	};

	enum DragEvent
	{
		DRAG_NONE,
		DRAG_START,
		DRAG_END,
		DRAG
	};
    
    /// Constructor
    TransformFeature();
    /// Destructor
    ~TransformFeature();

	/// setup transform feature
	virtual void Setup();

	/// handle ordinary move from mouse
	virtual DragEvent OnMove(bool move);
	/// handle free move from mouse
	virtual void OnFreeMove(bool move);

    /// Checks the screen position of the mouse and tries to lock mouse input to feature
    virtual void StartDrag();
    /// Updates the feature. Normally this is used to compute the actual transform of the feature.
    virtual void Drag();
    /// Cancel drag status of feature. This will stop the feature listen to the mouse position.
    virtual void ReleaseDrag();
    
    /// Can only be true if StartDrag() returns in a valid drag and ReleaseDrag was not called.
    bool HasValidDrag();
    
    /// Set the matrix of the feature
	virtual void SetInitialMatrix(Math::matrix44& mat);
    /// Gets the transform the feature has done since initial matrix set.
    virtual Math::matrix44 GetDeltaMatrix();
    
    /// Renders the handles for the user input
    virtual void RenderHandles();
	Math::vector FindOrtho( Math::vector& v );

	/// Sets the camera entity
	void SetCameraEntity(Ptr<Game::Entity> camera);

	/// updates the transform
	virtual void UpdateTransform(const Math::matrix44 & transform);	

	/// compute handle positions
	virtual void UpdateHandlePositions();

protected:

	/// returns the handle above which the mouse is currently over
	virtual DragMode GetMouseHandle(const Math::line& worldMouseRay);

	/// decomposes the current transform matrix in translation vector, scale vector and rotation matrix.
	void DecomposeInitialMatrix();

	Math::vector decomposedTranslation;
	Math::vector decomposedScale;
	Math::matrix44 decomposedRotation;
    
	Ptr<Game::Entity> cameraEntity;
    bool isInDragMode;
	bool isInFreeDragMode;
    Math::matrix44 initialMatrix;
    Math::matrix44 deltaMatrix;
    Math::matrix44 startDragMatrix;
	DragMode currentDragMode;

	Util::FixedArray<Ptr<Graphics::MeshEntity>> handleGraphicsEntities;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
TransformFeature::SetCameraEntity( Ptr<Game::Entity> camera )
{
	this->cameraEntity = camera;
}

} // namespace LevelEditor


