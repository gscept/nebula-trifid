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

using namespace Math;
namespace LevelEditor2
{
__ImplementClass(LevelEditor2::TransformFeature,'LETF',Core::RefCounted);

//------------------------------------------------------------------------------
/**
    Constructor	
*/
TransformFeature::TransformFeature() :
	currentDragMode(NONE),
	currentHandleHovered(NONE),
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
	// handle in subclass
}


//------------------------------------------------------------------------------
/**
*/
void
TransformFeature::Discard()
{
	IndexT i;
	for (i = 0; i < this->handleGraphicsEntities.Size(); i++)
	{
		this->handleGraphicsEntities[i]->GetStage()->RemoveEntity(this->handleGraphicsEntities[i].upcast<Graphics::GraphicsEntity>());
	}
	this->handleGraphicsEntities.Clear();
	this->handleIndices.Clear();
	this->handleColors.Clear();

	for (i = 0; i < this->vbos.Size(); i++)
	{
		this->vbos[i]->Unload();
	}
	this->vbos.Clear();

	for (i = 0; i < this->ibos.Size(); i++)
	{
		this->ibos[i]->Unload();
	}
	this->ibos.Clear();
}


//------------------------------------------------------------------------------
/**
	Render handles, unique for each transform feature.
*/
void
TransformFeature::RenderHandles()
{
	if (this->currentDragMode == NONE) this->currentHandleHovered = this->GetMouseHandle();
}

//------------------------------------------------------------------------------
/**
*/
TransformFeature::DragEvent
TransformFeature::OnMove(bool move)
{
	Ptr<BaseGameFeature::EnvQueryManager> envQueryManager = BaseGameFeature::EnvQueryManager::Instance();
	Ptr<Graphics::View> defaultView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();

	// get drag mode, only do this once per 'click' so that it sticks
	if (this->currentDragMode == NONE && move) this->currentDragMode = this->currentHandleHovered;

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
	this->currentDragMode = NONE;
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
TransformFeature::GetMouseHandle()
{
	// get mouse position on screen
	const Math::float2& mousePos = Input::InputServer::Instance()->GetDefaultMouse()->GetPixelPosition();
	const float rectSize = 10.0f; // 10 pixels
	const Math::rectangle<float> mouseRect(mousePos.x() - rectSize, mousePos.y() - rectSize, mousePos.x() + rectSize, mousePos.y() + rectSize);
	Util::Array<IndexT> items;
	Util::Array<IndexT> dummy;
	Picking::PickingServer::Instance()->FetchSquare(mouseRect, items, dummy);

	DragMode modes[] = { X_DRAG, Y_DRAG, Z_DRAG, GLOBAL_DRAG };
	DragMode mode = NONE;

	IndexT handleIdx;
	for (handleIdx = 0; handleIdx < this->handleIndices.Size(); handleIdx++)
	{
		bool handleHovered = false;
		IndexT itemIdx;
		for (itemIdx = 0; itemIdx < items.Size(); itemIdx++)
		{
			if (items[itemIdx] == this->handleIndices[handleIdx])
			{
				mode = modes[handleIdx];
				return mode;
			}
		}
	}
	return mode;
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

//------------------------------------------------------------------------------
/**
*/
void
TransformFeature::ComposeInitialMatrix()
{
	this->initialMatrix = matrix44::transformation(float4(0), float4(0), this->scale, float4(0), this->rotation, this->position);
}

//------------------------------------------------------------------------------
/**
*/
void
TransformFeature::OnGainFocus()
{
	IndexT i;
	for (i = 0; i < this->handleGraphicsEntities.Size(); i++)
	{
		this->handleGraphicsEntities[i]->SetVisible(true);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
TransformFeature::OnLoseFocus()
{
	IndexT i;
	for (i = 0; i < this->handleGraphicsEntities.Size(); i++)
	{
		this->handleGraphicsEntities[i]->SetVisible(false);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
TransformFeature::RotateVector(Math::vector &i_v, Math::vector &axis, float angle)
{
	matrix44 rot = matrix44::rotationaxis(axis, angle);
	i_v = matrix44::transform(i_v, rot);
}



}