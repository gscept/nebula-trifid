#pragma once
//------------------------------------------------------------------------------
/** 
    @pragma once

    Special billboard renderer for the lowest LOD level of a tree.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "forest/tree.h"
#include "forest/treeinstance.h"
#include "forest/treebillboardgraphicsentity.h"

//------------------------------------------------------------------------------
namespace Forest
{
class TreeBillboardRenderer : public Core::RefCounted
{
    __DeclareClass(TreeBillboardRenderer);
    __DeclareSingleton(TreeBillboardRenderer);
public:
    /// constructor
    TreeBillboardRenderer();
    /// destructor
    virtual ~TreeBillboardRenderer();

    /// setup the tree renderer
    void Setup();
    /// discard the tree renderer
    void Discard();
    /// return true if object is valid
    bool IsValid() const;
    /// must be called when a new level is loaded with the level's bounding box
    void LoadLevel();
    /// must be called when exiting the current level
    void ExitLevel();

    /// begin adding trees
    void Begin();
    /// add a tree instance
    void AddTreeInstance(const Ptr<TreeInstance>& treeInstance);
    /// end adding trees, and render them
    void End();

private:
    /// update dynamic mesh with billboard geometry
    void UpdateDynamicMesh();
    /// cleanup trees, call this method when a level is left
    void CleanupTrees();
    /// render the accumulated tree instances
    void Render();

    static const SizeT MaxNumVertices = (1<<16);
    static const SizeT MaxNumIndices = (1<<16) * 2;

    Math::point viewerPos;
    Util::Array<Util::KeyValuePair<float, TreeInstance*> > treeInstances;
    Ptr<CoreGraphics::VertexBuffer> vertexBuffer;
	Ptr<CoreGraphics::IndexBuffer> indexBuffer;
	Ptr<CoreGraphics::VertexLayout> vertexLayout;
	CoreGraphics::PrimitiveGroup group;
    Ptr<TreeBillboardGraphicsEntity> graphicsEntity;
    bool isValid;
    bool inBegin;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
TreeBillboardRenderer::IsValid() const
{
    return this->isValid;
}

} // namespace Forest
//------------------------------------------------------------------------------




