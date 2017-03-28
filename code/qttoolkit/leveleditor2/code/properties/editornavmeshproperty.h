#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::EditorNavMeshPropety

    Property for handling navmesh generation in level editor
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/ptr.h"
#include "game/property.h"
#include "attr/attributedefinition.h"
#include "core/rtti.h"
#include "attr/attributecontainer.h"
#include "messaging/staticmessagehandler.h"
#include "navigation/recast/recastutil.h"



//------------------------------------------------------------------------------
namespace LevelEditor2
{
class EditorNavMeshProperty : public Game::Property
{
	__DeclareClass(EditorNavMeshProperty);
    __SetupExternalAttributes();
public:    
	EditorNavMeshProperty();
    /// setup callbacks for this property, call by entity in OnActivate()
    virtual void SetupCallbacks();
    /// setup accepted messages
	virtual void SetupAcceptedMessages();
    /// handle messages
	virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
    /// called before rendering happens
    virtual void OnRender();
    /// calculate navmesh from data
    virtual void UpdateMesh();
    /// save nav mesh blob to file
    virtual void SaveMesh();
    /// called from Entity::ActivateProperties()
    virtual void OnStart();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();

protected:
	friend class Messaging::StaticMessageHandler;   	

};
__RegisterClass(EditorNavMeshProperty);


}; // namespace LevelEditor2
//------------------------------------------------------------------------------

