#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::PlacementUtil

    Provides a framework to transform a given list of matrices.
    This is used to change the position, scale and rotation of
    objects in 3D space.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/

#include "transformfeature.h"
#include "translatefeature.h"
#include "scalefeature.h"
#include "rotationfeature.h"
#include "math/matrix44.h"
#include "input/key.h"
#include "input/mousebutton.h"
#include "game/entity.h"
#include "math/bbox.h"
#include "core/singleton.h"
#include "actions/transformaction.h"

//------------------------------------------------------------------------------

namespace LevelEditor2
{

class PlacementUtil : public Core::RefCounted
{
    __DeclareClass(PlacementUtil);
	__DeclareSingleton(PlacementUtil);

public: 
    
    /// possible transform modes
    enum TransformMode
    {
        NONE,
        TRANSLATE,
        SCALE,
        ROTATE
    };

    /// possible placement states
    enum PlacementState
    {
        IDLE,
        STARTING,
        TRANSFORMING,
        FINISHING
    };

    /// Constructor
    PlacementUtil();
    /// Destructor
    ~PlacementUtil();

	/// create different transform tools
	void Setup();
	/// discard tools
	void Discard();

    /// Handles the input
    bool HandleInput();

    /// Render features
    void Render();
    
    /// Set the selection with the given transforms
    void SetSelection(const Util::Array<Math::matrix44>& matrices);
    /// Get the list of all transforms, which may have changed by user input
    const Util::Array<Math::matrix44>& GetSelection();
    /// Clears the internal list of selected matrices
    void ClearSelection();
    /// Reset all changes of the current selection and return to the state when selection was set.
    void ResetSelection();	
    
    /// Set transform mode. Is ignored if current transform feature is active
    void ToggleTransformMode(TransformMode mode);
    /// Get current transform mode
    TransformMode GetCurrentTransformMode();

    /// Get current placement state
    PlacementState GetCurrentPlacementState();

	/// Set camera entity
	void SetCameraEntity(Ptr<Game::Entity> i_pCamera);
	// Get camera entity
	Ptr<Game::Entity> GetCameraEntity();

	/// returns angles from transform attribute in game entity
	Math::float4 GetAngleRotation(Ptr<Game::Entity> entity);
	/// returns angles from matrix, same as above except it doesn't need an actual entity to work on
	Math::float4 GetAngleRotation(Math::matrix44& matrix);
	
	/// set the angles at which the rotation should snap
	void SetSnapOffset(float value);

	/// updates the current feature from selection
	void UpdateCurrentFeature();

	/// access to the placement feature objects
	Ptr<TransformFeature> GetPlacementFeature(TransformMode mode);

	/// center pivot (only works if there is a single group node selected)
	void CenterPivot();

private:
    friend class AttributeWidgetManager;

	/// set the matrix of a selected entity
	void SetSelectionMatrix(const IndexT& index, const Math::matrix44& matrix);

    /// Gets the current feature and applies its transformation to the selected matrices.
    void ApplyTransformFeatureToMatrices();
 
	Ptr<Game::Entity> cameraEntity;
	
    TransformMode currentTransformMode;
    PlacementState currentPlacementState;

    Util::Array<Math::matrix44> selectedMatrices;
    Util::Array<Math::matrix44> selectedInitialMatrices;

    Input::MouseButton::Code selectMoveFeatureButton;
    Input::MouseButton::Code freeMoveFeatureButton;

    Input::Key::Code activateTranslateKey;         // input key codes
    Input::Key::Code activateRotateKey;
    Input::Key::Code activateScaleKey;
    Input::Key::Code activateGroundPlacementKey;
    Input::Key::Code activateAxisLockKey;
	Input::Key::Code activateGroupTranslateKey;
	Input::Key::Code activateSnapPlacementKey;
            
    Ptr<TranslateFeature> translateFeature;         // possible transform features
    Ptr<ScaleFeature> scaleFeature;
    Ptr<RotationFeature> rotationFeature;

    Ptr<TransformFeature> currentTransformFeature;

	Ptr<TransformAction> action;
	
	bool groupPivotTranslate;	
};
//------------------------------------------------------------------------------
/**
*/
inline void 
PlacementUtil::SetCameraEntity( Ptr<Game::Entity> i_pCamera )
{
	cameraEntity = i_pCamera;
	if(this->translateFeature.isvalid())
	{
		this->translateFeature->SetCameraEntity(i_pCamera);
	}
	if(this->scaleFeature.isvalid())
	{
		this->scaleFeature->SetCameraEntity(i_pCamera);
	}
	if(this->rotationFeature.isvalid())
	{
		this->rotationFeature->SetCameraEntity(i_pCamera);
	}
}

//------------------------------------------------------------------------------
/**
*/
inline Ptr<Game::Entity> 
PlacementUtil::GetCameraEntity()
{
	return this->cameraEntity;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PlacementUtil::SetSnapOffset( float value )
{
	if(this->translateFeature.isvalid())
	{
		this->translateFeature->SetSnapOffset(value);
	}
}

} // namespace LevelEditor2



