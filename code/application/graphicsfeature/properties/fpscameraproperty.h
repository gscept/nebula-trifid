#pragma once
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::FpsCameraProperty

	based on https://github.com/GameProj2015/nebula-trifid/blob/master/code/application/corefeature/properties/fpscameraproperty.h
    
	(C) Patrik Nyman, Mariusz Waclawek
    (C) 2016 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "util/stringatom.h"
#include "graphics/modelentity.h"
#include "cameraproperty.h"


namespace Graphics
{
    class CameraEntity;
    class View;
    class Stage;
}

namespace GraphicsFeature
{
class FpsCameraProperty : public GraphicsFeature::CameraProperty
{
	__DeclareClass(FpsCameraProperty);
	__SetupExternalAttributes();
public:
    /// constructor
	FpsCameraProperty();
    /// destructor
    virtual ~FpsCameraProperty();    
    
    /// setup callbacks for this property
    virtual void SetupCallbacks();    
    /// called from within Entity::OnStart() after OnLoad when the complete world exist
    virtual void OnStart();    	
    /// called before rendering happens
    virtual void OnBeginFrame();        
	/// setup accepted messages
	virtual void SetupAcceptedMessages();
	/// handle a single message
	virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
	/// called when camera focus is obtained
	virtual void OnObtainCameraFocus();
	/// called when camera focus is lost
	virtual void OnLoseCameraFocus();
protected:

    
	///Get joint position by index
	const Math::matrix44 & GetJointPos(IndexT index) const;
	
	Ptr<Graphics::ModelEntity> modelEntity;
	float rotx;	
	float sensitivity;
	float ylimit;		
	Util::String head;	
	Util::String skin;
	IndexT headIndex;	
};
__RegisterClass(FpsCameraProperty);

}; // namespace GraphicsFeature
//------------------------------------------------------------------------------
