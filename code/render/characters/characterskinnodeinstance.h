#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::CharacterMaterialSkinNodeInstance
    
    Implements per-instance functionality of a character skin node using materials.
    
    (C) 2011-2013 Individual contributors, see AUTHORS file
*/
#include "models/nodes/shapenodeinstance.h"
#include "characters/characterinstance.h"
#include "characters/skinnedmeshrenderer.h"
#include "coregraphics/feedbackbuffer.h"
#include "coregraphics/shaderbuffer.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharacterSkinNodeInstance : public Models::ShapeNodeInstance
{
    __DeclareClass(CharacterSkinNodeInstance);
public:
    /// constructor
    CharacterSkinNodeInstance();
    /// destructor
    virtual ~CharacterSkinNodeInstance();

    /// called when attached to ModelInstance
    virtual void Setup(const Ptr<Models::ModelInstance>& inst, const Ptr<Models::ModelNode>& node, const Ptr<Models::ModelNodeInstance>& parentNodeInst);
    /// called when removed from ModelInstance
    virtual void Discard();
    /// called from ModelEntity::OnNotifyCullingVisible
    virtual void OnNotifyCullingVisible(IndexT frameIndex, Timing::Time time);
	/// called before rendering will be done
	virtual void OnRenderBefore(IndexT frameIndex, Timing::Time time);
	/// applies state
	virtual void ApplyState();
    /// perform rendering
    virtual void Render();
    /// perform instanced rendering
    virtual void RenderInstanced(SizeT numInstances);

protected:
	Ptr<CoreGraphics::ShaderInstance> skinningShader;
	Ptr<CoreGraphics::ShaderVariable> skinningJointPaletteVar;
	Ptr<CoreGraphics::FeedbackBuffer> feedbackBuffer;
	Ptr<CoreGraphics::ShaderBuffer> jointBuffer;
    Ptr<CharacterInstance> characterInstance;
    SkinnedMeshRenderer::DrawHandle drawHandle;
};

} // namespace Characters
//------------------------------------------------------------------------------
    