#pragma once
//------------------------------------------------------------------------------
/**
    @class Properties::EnvironmentGraphicsProperty

    This is a specialized graphics property which handles all the static
    environment graphics. Probably 90% of all objects in a level will 
    just be static environment objects, so it makes sense to create
    an optimized class just for that.

    NOTE: usually you don't need to care about this class. The level loader
    will automatically put all static environment objects into a single
    game entity which has a StaticGraphicsProperty attached.
        
    (C) 2005 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "graphics/modelentity.h"
#include "util/dictionary.h"

//------------------------------------------------------------------------------
namespace GraphicsFeature
{
class EnvironmentGraphicsProperty : public Game::Property
{
	__DeclareClass(EnvironmentGraphicsProperty);
public:
    /// constructor
    EnvironmentGraphicsProperty();
    /// destructor
    virtual ~EnvironmentGraphicsProperty();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// add a graphics entity group 
    void AddGraphicsEntities(const Util::String& id, const Math::matrix44& worldMatrix, const Util::Array<Ptr<Graphics::ModelEntity> >& gfxEntities);
    /// delete graphics entity group
    void DeleteGraphicsEntities(const Util::String& id);
    /// return true if a graphics entity group exists by id
    bool HasGraphicsEntities(const Util::String& id) const;
    /// get pointer to graphics entity by name
    const Util::Array<Ptr<Graphics::ModelEntity> >& GetGraphicsEntities(const Util::String& id) const;
    /// get the localized graphics entity matrizes associated with an id (needed for Live level editing)
    const Util::Array<Math::matrix44>& GetLocalMatrices(const Util::String& id) const;

private:
    struct Entry
    {
        Util::Array<Ptr<Graphics::ModelEntity> > graphicsEntities;
        Util::Array<Math::matrix44> localMatrices;
    };
    Util::Dictionary<Util::String, Entry> entries;
};
__RegisterClass(EnvironmentGraphicsProperty);

}; // namespace GraphicsFeature
//------------------------------------------------------------------------------




