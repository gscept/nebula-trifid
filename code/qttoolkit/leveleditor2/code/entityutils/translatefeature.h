#pragma once
//------------------------------------------------------------------------------
/**
    @class Leveleditor2::TranslateFeature
    
    Displays a translate feature to translate objects in 3D space.
    Handles the drag of those features and returns the movement
    the user causes by his mouse move.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/

#include "transformfeature.h"
#include "math/vector.h"
#include "math/line.h"

//------------------------------------------------------------------------------
namespace LevelEditor2
{

class TranslateFeature : public TransformFeature
{

__DeclareClass(TranslateFeature)

public:
    
    /// Constructor
    TranslateFeature();
    /// Destructor
    ~TranslateFeature();
 
    /// begin a drag
    void StartDrag();
    /// update translation feature
    void Drag();
    /// leave dragging
    void ReleaseDrag();
    
    /// sets relative translation mode
    void ActivateRelativeMode(bool activate);
    /// get relative translation mode
    bool IsRelativeModeActivated();

    /// sets if the translation should use the underlying axis for locking instead of translation
    void ActivateAxisLocking(bool activate);
    /// returns if we are in locking mode
    bool IsAxisLockingActivated();

    /// tells the translation util to use a snapping behavior
	void ActivateSnapMode(bool activate);
    /// returns true if tool is using snapping
	bool IsSnapModeActivated();
    /// sets the size of each increment of the snapping behavior
    void SetSnapOffset(float value);

    /// renders handles
    void RenderHandles();

    /// update transform of object(s)
	virtual void UpdateTransform(const Math::matrix44 & transform);        

private:
    
    /// checks current initial matrix and feature translation to compute handle positions in space.
    void UpdateHandlePositions();
    
	/// returns the handle above which the mouse is currently over
	DragMode GetMouseHandle(const Math::line& worldMouseRay);

	Math::point x_handle;
	Math::point y_handle;
	Math::point z_handle;
    Math::point o_handle;
	Math::plane view_plane;

	float snapOffset;
    float handleScale;
    float handleDistance;
    float dragPlaneOffset;
	Math::vector dragStartMouseRayOffset;
	Math::vector dragStart;
	Math::vector lockedDragStart;

    bool relativeMode;
    bool axisLockingMode;
    bool freeModeRequested;
	bool snapMode;
};


//------------------------------------------------------------------------------
/**
*/
inline void
TranslateFeature::ActivateRelativeMode(bool activate)
{
    this->relativeMode = activate;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
TranslateFeature::IsRelativeModeActivated()
{
    return this->relativeMode;
}

//------------------------------------------------------------------------------
/**
*/
inline void
TranslateFeature::ActivateAxisLocking(bool activate)
{
    this->axisLockingMode = activate;	
}

//------------------------------------------------------------------------------
/**
*/
inline bool
TranslateFeature::IsAxisLockingActivated()
{
    return this->axisLockingMode;
}


//------------------------------------------------------------------------------
/**
*/
inline void
TranslateFeature::ActivateSnapMode( bool activate )
{
	this->snapMode = activate;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
TranslateFeature::IsSnapModeActivated()
{
	return this->snapMode;
}

}
