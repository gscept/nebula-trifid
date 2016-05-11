//------------------------------------------------------------------------------
//  transformfeature.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "transformfeature.h"
#include "managers/envquerymanager.h"
#include "graphicsfeatureunit.h"
#include "input/inputserver.h"
#include "input/mouse.h"

//------------------------------------------------------------------------------
namespace LevelEditor2
{
__ImplementClass(LevelEditor2::TransformFeature,'LETF',Core::RefCounted);

//------------------------------------------------------------------------------
/**
    Constructor	
*/
TransformFeature::TransformFeature() :
	currentDragMode(NONE),
    isInDragMode(false)
{
	this->deltaMatrix = Math::matrix44::identity();
}

//------------------------------------------------------------------------------
/**
    Destructor	
*/
TransformFeature::~TransformFeature()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
TransformFeature::Setup()
{

}

//------------------------------------------------------------------------------
/**
	Render handles, unique for each transform feature.
*/
void
TransformFeature::RenderHandles()
{
    // override in subclass...
}

//------------------------------------------------------------------------------
/**
*/
TransformFeature::DragEvent
TransformFeature::OnMove(bool move)
{
	Ptr<BaseGameFeature::EnvQueryManager> envQueryManager = BaseGameFeature::EnvQueryManager::Instance();
	Ptr<Graphics::View> defaultView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();

	// get mouse position on screen
	const Math::float2& mousePos = Input::InputServer::Instance()->GetDefaultMouse()->GetScreenPosition();

	// get mouse ray
	const float rayLength = 5000.0f;
	Math::line worldMouseRay = envQueryManager->ComputeMouseWorldRay(mousePos, rayLength, defaultView);

	// get drag mode, only do this once per 'click' so that it sticks
	if (this->currentDragMode == NONE && move)	this->currentDragMode = this->GetMouseHandle(worldMouseRay);

	if (this->currentDragMode != NONE)
	{
		if (!this->isInDragMode && move)
		{
			this->StartDrag();
			return DRAG_START;
		}
		else if (this->isInDragMode && !move)
		{
			this->ReleaseDrag();

			// free the drag mode too, this makes the drag persistent between clicks!
			this->currentDragMode = NONE;
			return DRAG_END;
		}
		else if (this->isInDragMode && move)
		{
			this->Drag();
			return DRAG;
		}
	}
	return DRAG_NONE;
}

//------------------------------------------------------------------------------
/**
*/
void
TransformFeature::OnFreeMove(bool move)
{
	this->isInFreeDragMode = move;
}

//------------------------------------------------------------------------------
/**
	Tries to start dragging the feature.
    Analyzes the mouse position and sets the drag mode flag
    if a valid drag occurred.
    Implement that in subclasses!
    Updates the startDragMatrix.
*/
void
TransformFeature::StartDrag()
{
	this->startDragMatrix = Math::matrix44::identity();
	this->startDragMatrix = Math::matrix44::multiply(this->startDragMatrix, this->initialMatrix);
    this->startDragMatrix = Math::matrix44::multiply(this->startDragMatrix, this->deltaMatrix);
	this->isInDragMode = true;
}

//------------------------------------------------------------------------------
/**
	Override this in subclasses to calculate the new transform of the feature
    depending of the user input.
*/
void
TransformFeature::Drag()
{
    // empty
	n_assert(this->isInDragMode);
}

//------------------------------------------------------------------------------
/**
	Stops the current drag that was eventually started by the StartDrag() method.
    Sets the drag flag to False.
*/
void
TransformFeature::ReleaseDrag()
{
    this->isInDragMode = false;
}

//------------------------------------------------------------------------------
/**
    Sets the matrix of the feature. This describes the initial placement of
    the feature. The initial matrix should never changed between the calls
    of StartDrag() and ReleaseDrag()
*/
void
TransformFeature::SetInitialMatrix(Math::matrix44& mat)
{
    this->initialMatrix = mat;

    // reset delta matrix
	this->deltaMatrix = Math::matrix44::identity();
}

//------------------------------------------------------------------------------
/**
	Returns the delta matrix. The delta matrix describes the transform of the
    feature since initial matrix was set.
*/
Math::matrix44
TransformFeature::GetDeltaMatrix()
{
    return this->deltaMatrix;
}

//------------------------------------------------------------------------------
/**
	Return the status of the drag flag.
    A valid drag only happens, if at StartDrag the mouse position was over
    a valid drag point.
*/
bool
TransformFeature::HasValidDrag()
{
    return this->isInDragMode;
}

//------------------------------------------------------------------------------
/**
*/
void
TransformFeature::UpdateTransform(const Math::matrix44 & transform)
{
	this->initialMatrix = transform;
	this->deltaMatrix = Math::matrix44::identity();	
}

//------------------------------------------------------------------------------
/**
*/
Math::vector 
TransformFeature::FindOrtho( Math::vector& v )
{
	if (0.0 != v.x())
	{
		return Math::vector((-v.y() - v.z()) / v.x(), 1.0, 1.0);
	}
	else if (0.0 != v.y())
	{
		return Math::vector(1.0, (-v.x() - v.z()) / v.y(), 1.0);
	}
	else if (0.0 != v.z())
	{
		return Math::vector(1.0, 1.0, (-v.x() - v.y()) / v.z());
	}
	else
	{
		return Math::vector(0.0, 0.0, 0.0);
	}
}

//------------------------------------------------------------------------------
/**
*/
TransformFeature::DragMode
TransformFeature::GetMouseHandle(const Math::line& worldMouseRay)
{
	// implement in subclass...
	return NONE;
}

//------------------------------------------------------------------------------
/**
*/
void
TransformFeature::UpdateHandlePositions()
{
	// implement in subclass...
}

//------------------------------------------------------------------------------
/**
*/
void
TransformFeature::DecomposeInitialMatrix()
{
	Math::quaternion quat;
	this->initialMatrix.decompose(this->decomposedScale, quat, this->decomposedTranslation);
	this->decomposedRotation = Math::matrix44::rotationquaternion(quat);
}

}