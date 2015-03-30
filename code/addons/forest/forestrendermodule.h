#pragma once
//------------------------------------------------------------------------------
/**
    @class Forest::ForestRenderModule

    The client-side object for the forest subsystem

    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/singleton.h"
#include "rendermodules/rendermodule.h"
#include "util/stringatom.h"
#include "math/matrix44.h"
#include "math/bbox.h"

namespace Forest
{
class ForestRenderModule : public RenderModules::RenderModule
{
    __DeclareClass(ForestRenderModule);
    __DeclareSingleton(ForestRenderModule);
    
public:
    /// Constructor
    ForestRenderModule();
    /// Destructor
    virtual ~ForestRenderModule();

    /// Setup the render module (call once at startup to register rendermodule)
    virtual void Setup();
    /// Discard the render module
    virtual void Discard();
    /// Set the bounding box of the level to compute the depth of the quadtree 
    virtual void LoadLevel(const Math::bbox& boundingBox);
    /// Reset the tree server, cleanup all trees and removes the quadtree
    virtual void ExitLevel();
    /// Create a tree instance and add it to a treecluster
    virtual void CreateTreeInstance(const Util::StringAtom& resourceId, const Util::StringAtom& id, const Math::matrix44& transform, const Util::StringAtom& collideFile);
    /// Creates one collide mesh for all added trees. Call this once after all trees are added.
    virtual void SetupTreeCollide();
};

} // namespace Forest