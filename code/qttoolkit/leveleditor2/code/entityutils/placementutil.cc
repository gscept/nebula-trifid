//------------------------------------------------------------------------------
//  placementutil.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "placementutil.h"
#include "input/mouse.h"
#include "input/keyboard.h"
#include "input/inputserver.h"
#include "debugrender/debugrender.h"
#include "managers/entitymanager.h"
#include "game/entity.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "selectionutil.h"
#include "matrix44controller.h"
#include "managers/attributewidgetmanager.h"
#include "basegamefeature/basegameprotocol.h"
#include "actions/transformaction.h"
#include "actions/actionmanager.h"
#include "leveleditor2entitymanager.h"
#include "properties/editorproperty.h"

namespace LevelEditor2
{
using namespace Input;
using namespace Math;
using namespace BaseGameFeature;

__ImplementClass(LevelEditor2::PlacementUtil,'LEPU', Core::RefCounted);
__ImplementSingleton(LevelEditor2::PlacementUtil);

//------------------------------------------------------------------------------
/**
    Constructor
*/
PlacementUtil::PlacementUtil() :
    currentTransformMode(NONE),
    currentPlacementState(IDLE),	
	groupPivotTranslate(false)
{
	__ConstructSingleton;

    // register input
    this->selectMoveFeatureButton = Input::MouseButton::LeftButton;
    this->freeMoveFeatureButton = Input::MouseButton::MiddleButton;
	
    this->activateTranslateKey = Input::Key::W;
    this->activateRotateKey = Input::Key::E;
    this->activateScaleKey = Input::Key::R;
	this->activateGroupTranslateKey = Input::Key::D;
    this->activateGroundPlacementKey = Input::Key::Shift;
    this->activateAxisLockKey = Input::Key::Control;
	this->activateSnapPlacementKey = Input::Key::X;	
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
PlacementUtil::~PlacementUtil()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    Set the matrices the tool will work on.
    Call GetSelection() to watch changes on the matrices.
*/
void
PlacementUtil::SetSelection(const Util::Array<Math::matrix44> & matrices)
{
    this->selectedMatrices = matrices;
    this->selectedInitialMatrices = matrices;		
    // update current feature from selection
    this->UpdateCurrentFeature();
}

//------------------------------------------------------------------------------
/**
    Get the current work matrices.
    The may differ from the originally given matrices because of user input.
*/
const
Util::Array<Math::matrix44> &
PlacementUtil::GetSelection()
{
    return this->selectedMatrices;
}

//------------------------------------------------------------------------------
/**
    Clears the list of matrices.
*/
void
PlacementUtil::ClearSelection()
{
    this->selectedMatrices.Clear();
    this->selectedInitialMatrices.Clear();	
}

//------------------------------------------------------------------------------
/**
    Set the matrices to the state as they where added by SetSelection().
*/
void
PlacementUtil::ResetSelection()
{
     IndexT i;
	 for (i = 0; i < this->selectedMatrices.Size(); i++)
     {
		 if (i < this->selectedInitialMatrices.Size())
         {
             this->selectedMatrices[i] = this->selectedInitialMatrices[i];
         }
     }
}

//------------------------------------------------------------------------------
/**
    Analyze user input and change the selected matrices.
*/
bool
PlacementUtil::HandleInput()
{
	//TODO: fix that when user transformation is finished, the matrix44 attribute controller is updated

	const Ptr<Input::Mouse>& mouse = InputServer::Instance()->GetDefaultMouse();
	const Ptr<Input::Keyboard>& keyboard = InputServer::Instance()->GetDefaultKeyboard();

	bool retval = false;
    this->currentPlacementState = IDLE;

    if (this->translateFeature.isvalid())
    {
        this->translateFeature->ActivateRelativeMode(keyboard->KeyPressed(this->activateGroundPlacementKey));
        this->translateFeature->ActivateSnapMode(keyboard->KeyPressed(this->activateSnapPlacementKey));
        this->translateFeature->ActivateAxisLocking(keyboard->KeyPressed(this->activateAxisLockKey));
		this->groupPivotTranslate = keyboard->KeyPressed(this->activateGroupTranslateKey);
    }

    if (this->scaleFeature.isvalid())
    {
        this->scaleFeature->ActivateAxisLocking(keyboard->KeyPressed(this->activateAxisLockKey));
    }

    /// only handle any input if anything is selected
	if (this->currentTransformFeature.isvalid())
	{
		this->currentTransformFeature->OnFreeMove(mouse->ButtonPressed(this->freeMoveFeatureButton));
		TransformFeature::DragEvent event = this->currentTransformFeature->OnMove(mouse->ButtonPressed(this->selectMoveFeatureButton));
		retval = event != TransformFeature::DRAG_NONE;

		if (event == TransformFeature::DRAG)
		{
			this->ApplyTransformFeatureToMatrices();
		}		
		else if (event == TransformFeature::DRAG_START)
		{
			this->action = TransformAction::Create();
			this->action->SetEntitiesStart(SelectionUtil::Instance()->GetSelectedEntityIds(), this->selectedMatrices);
		}
		else if (event == TransformFeature::DRAG_END)
		{
			this->action->SetEntitiesEnd(this->selectedMatrices);
			ActionManager::Instance()->PerformAction(this->action.upcast<Action>());

			// reset pivot
			this->UpdateCurrentFeature();
		}
	}

	return retval;
}
//------------------------------------------------------------------------------
/**
	Tries to set the transform mode. If the current transform feature
    is active (i.e. user is dragging a feature), this function will abort
    silently.
*/
void
PlacementUtil::ToggleTransformMode(PlacementUtil::TransformMode mode)
{
    if (this->currentTransformFeature.isvalid())
    {
        if (this->currentTransformFeature->HasValidDrag())
        {
            return;
        }
    }

    if (NONE == mode)
    {
		this->currentTransformMode = NONE;
		if (this->currentTransformFeature.isvalid()) this->currentTransformFeature->OnLoseFocus();
        this->currentTransformFeature = 0;
    }
    else if (TRANSLATE == mode)
    {
        // ignore double activation
        if (TRANSLATE != this->currentTransformMode)
        {
            // activate translate mode
            this->currentTransformMode = TRANSLATE;

            // set current feature            
			if (this->currentTransformFeature.isvalid()) this->currentTransformFeature->OnLoseFocus();
            this->currentTransformFeature = this->translateFeature;
			this->currentTransformFeature->OnGainFocus();

            // updates new feature initial matrix
            this->UpdateCurrentFeature();
        }
    }
    else if(SCALE == mode)
    {
        // ignore double activation
        if (SCALE != this->currentTransformMode)
        {
            IndexT i;
            for (i=0; i<this->selectedMatrices.Size(); i++)
            {
                if(i<this->selectedInitialMatrices.Size())
                {
                    this->selectedInitialMatrices[i] = this->selectedMatrices[i];
                }
            }

            this->currentTransformMode = SCALE;

            //set current feature            
			if (this->currentTransformFeature.isvalid()) this->currentTransformFeature->OnLoseFocus();
            this->currentTransformFeature = this->scaleFeature;
			this->currentTransformFeature->OnGainFocus();

            // updates new feature initial matrix
            this->UpdateCurrentFeature();
        }
    }
    else if (ROTATE == mode)
    {
        // ignore double activation
        if (ROTATE != this->currentTransformMode)
        {
            IndexT i;
            for (i=0; i<this->selectedMatrices.Size(); i++)
            {
                if(i<this->selectedInitialMatrices.Size())
                {
                    this->selectedInitialMatrices[i] = this->selectedMatrices[i];
                }
            }

            this->currentTransformMode = ROTATE;

            //set current feature           
			if (this->currentTransformFeature.isvalid()) this->currentTransformFeature->OnLoseFocus();
            this->currentTransformFeature = this->rotationFeature;
			this->currentTransformFeature->OnGainFocus();

            // updates new feature initial matrix
            this->UpdateCurrentFeature();
        }
    }
}

//------------------------------------------------------------------------------
/**
	Returns current transform mode.
*/
PlacementUtil::TransformMode
PlacementUtil::GetCurrentTransformMode()
{
    return this->currentTransformMode;
}

//------------------------------------------------------------------------------
/**
Returns current placement state.
*/
PlacementUtil::PlacementState
PlacementUtil::GetCurrentPlacementState()
{
    return this->currentPlacementState;
}

//------------------------------------------------------------------------------
/**
    Depending of the current transform feature the matrices were updated
    from the feature transformation.
*/
void
PlacementUtil::ApplyTransformFeatureToMatrices()
{
    if (this->currentTransformFeature.isvalid())
    {
        Math::matrix44 transformDelta = this->currentTransformFeature->GetDeltaMatrix();
        IndexT i;
        for (i = 0; i < this->selectedMatrices.Size(); i++)
        {
            if (i < this->selectedInitialMatrices.Size())
            {
                // At this point it is necessary to handle the transform and rotation delta matrix
                // in a different way, depending on the current transform mode.
                // Without that it is impossible to scale or rotate the current matrix in their
                // local coordinate system.

                if (TRANSLATE == this->currentTransformMode)
                {
					this->selectedMatrices[i] = Math::matrix44::multiply(this->selectedInitialMatrices[i], transformDelta);
                }
                else if (SCALE == this->currentTransformMode)
                {
                    this->selectedMatrices[i] =  Math::matrix44::multiply(transformDelta, this->selectedInitialMatrices[i]);
                }
                else if (ROTATE == this->currentTransformMode)
                {					
					// special way to rotate scaled matrices to prevent sheared results.
					// translate to global origin, rotate, then translate back
					Math::vector translate = this->selectedInitialMatrices[i].get_position();
					this->selectedInitialMatrices[i].translate(translate * (-1));
					this->selectedMatrices[i] = Math::matrix44::multiply(this->selectedInitialMatrices[i], transformDelta);
					this->selectedInitialMatrices[i].translate(translate);
					this->selectedMatrices[i].translate(translate);
                }
            }

			// update the active transform-attribute controller widget
			Ptr<Game::Entity> entity = SelectionUtil::Instance()->GetSelectedEntityAtIndex(i);
			if (AttributeWidgetManager::Instance()->IsCurrentlyViewed(entity))
			{
				QtAttributeControllerAddon::Matrix44Controller* transformController = AttributeWidgetManager::Instance()->GetCurrentTransformController();
				transformController->UpdateWidgetValues(this->selectedMatrices[i]);
			}

			// update matrices
			Ptr<SetTransform> msg = SetTransform::Create();
			msg->SetMatrix(this->selectedMatrices[i]);
			if (this->groupPivotTranslate && this->selectedMatrices.Size() == 1)
			{
				msg->SetDistribute(false);				
			}
			__SendSync(entity, msg);
        }	
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PlacementUtil::Render()
{
    // if anything is selected
    if (this->selectedMatrices.Size() > 0)
    {
        // render transform feature
        if (this->currentTransformFeature.isvalid())
        {
            this->currentTransformFeature->UpdateHandlePositions();
            this->currentTransformFeature->RenderHandles();
        }
    }
}

//------------------------------------------------------------------------------
/**
	Checks current selection and updated current feature from it.
*/
void
PlacementUtil::UpdateCurrentFeature()
{
    if (this->currentTransformFeature.isvalid())
    {
		IndexT i;
		if (!this->selectedMatrices.IsEmpty())
		{
			this->currentTransformFeature->OnGainFocus();
			this->currentTransformFeature->SetInitialMatrix(this->selectedMatrices[0]);
			for (i = 0; i < this->selectedMatrices.Size(); i++)
			{
				if (i < this->selectedInitialMatrices.Size())
				{
					this->selectedInitialMatrices[i] = this->selectedMatrices[i];
				}
			}
		}		
		else
		{
			this->currentTransformFeature->OnLoseFocus();
		}
    }
}

//------------------------------------------------------------------------------
/**
*/
Math::float4 
PlacementUtil::GetAngleRotation( Ptr<Game::Entity> entity )
{
	if (!this->rotationFeature.isvalid())
	{
		TransformMode currMode = this->currentTransformMode;
		this->ToggleTransformMode(ROTATE);
		this->ToggleTransformMode(currMode);
	}

	n_assert(entity.isvalid());
	Math::matrix44 mat = entity->GetMatrix44(Attr::Transform);

	float x, y, z;
	rotationFeature->GetAnglesFromMat(mat, x, y, z);
	x = n_rad2deg(x); y = n_rad2deg(y); z = n_rad2deg(z);
	return float4(x, y, z, 0);
}

//------------------------------------------------------------------------------
/**
*/
Math::float4
PlacementUtil::GetAngleRotation( Math::matrix44& matrix )
{
	if (!this->rotationFeature.isvalid())
	{
		TransformMode currMode = this->currentTransformMode;
		this->ToggleTransformMode(ROTATE);
		this->ToggleTransformMode(currMode);
	}

	float x, y, z;
	rotationFeature->GetAnglesFromMat(matrix, x, y, z);
	x = n_rad2deg(x); y = n_rad2deg(y); z = n_rad2deg(z);
	return float4(x, y, z, 0);

}

//------------------------------------------------------------------------------
/**
*/
void 
PlacementUtil::SetSelectionMatrix(const IndexT& index, const Math::matrix44& matrix)
{
	n_assert(index < this->selectedMatrices.Size());
	
	this->selectedMatrices[index] = matrix;		
}

//------------------------------------------------------------------------------
/**
*/
Ptr<TransformFeature>
PlacementUtil::GetPlacementFeature(TransformMode mode)
{
	switch (mode)
	{
	case ROTATE:
		return this->rotationFeature.cast<TransformFeature>();
		break;
	case TRANSLATE:
		return this->translateFeature.cast<TransformFeature>();
		break;
	case SCALE:
		return this->scaleFeature.cast<TransformFeature>();
		break;
	}
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
PlacementUtil::Setup()
{
	this->rotationFeature = RotationFeature::Create();
	this->rotationFeature->Setup();
	this->translateFeature = TranslateFeature::Create();
	this->translateFeature->Setup();
	this->scaleFeature = ScaleFeature::Create();
	this->scaleFeature->Setup();
}


//------------------------------------------------------------------------------
/**
*/
void
PlacementUtil::Discard()
{
	this->rotationFeature->Discard();
	this->rotationFeature = 0;
	this->translateFeature->Discard();
	this->translateFeature = 0;
	this->scaleFeature->Discard();
	this->scaleFeature = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
PlacementUtil::CenterPivot()
{
	Util::Array<Ptr<Game::Entity>> selection = SelectionUtil::Instance()->GetSelectedEntities();
	if (selection.Size() == 1)
	{
		Ptr<Game::Entity> entity = selection[0];
		if (entity->GetInt(Attr::EntityType) == Group)
		{
			Util::Array<Ptr<Game::Entity>> entities = SelectionUtil::Instance()->GetSelectedEntities(true);
			// this is a bit braindead 
			Util::Array<EntityGuid> guids;
			for (int i = 0;i < entities.Size();i++)
			{
				guids.Append(entities[i]->GetGuid(Attr::EntityGuid));
			}
			Math::bbox box = SelectionUtil::CalculateGroupBox(guids);
			Math::matrix44 curTrans = entity->GetMatrix44(Attr::Transform);
			curTrans.set_position(box.center());
			Ptr<SetTransform> msg = SetTransform::Create();
			msg->SetMatrix(curTrans);
			msg->SetDistribute(false);
			__SendSync(entity, msg);
			this->selectedMatrices[0] = curTrans;
			this->UpdateCurrentFeature();
		}
	}
}


} // namespace LevelEditor2

