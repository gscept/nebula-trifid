#pragma once
//------------------------------------------------------------------------------
/**
    @class Particles::ParticleSystemMaterialNodeInstance
    
    Wraps a ParticleSystemInstance into a ModelNodeInstance using materials.
    
    (C) 2011-2013 Individual contributors, see AUTHORS file
*/
#include "models/nodes/statenodeinstance.h"
#include "particles/particlesysteminstance.h"
#include "coregraphics/shadervariableinstance.h"
#include "coregraphics/shaderinstance.h"
#include "materials/materialvariable.h"

//------------------------------------------------------------------------------
namespace Particles
{
class ParticleSystemNodeInstance : public Models::StateNodeInstance
{
    __DeclareClass(ParticleSystemNodeInstance);
public:
    /// constructor
    ParticleSystemNodeInstance();
    /// destructor
    virtual ~ParticleSystemNodeInstance();

    /// called from ModelEntity::OnRenderBefore
    virtual void OnRenderBefore(IndexT frameIndex, Timing::Time time);
    /// called during visibility resolve
    virtual void OnVisibilityResolve(IndexT resolveIndex, float distanceToViewer);
    /// apply per-instance state prior to rendering
    virtual void ApplyState(IndexT frameIndex, const Frame::BatchGroup::Code& group, const Ptr<CoreGraphics::Shader>& shader);
    /// perform rendering
    virtual void Render();

    /// get the node's particle system instance
    const Ptr<ParticleSystemInstance>& GetParticleSystemInstance() const;

protected:
    /// called when attached to ModelInstance
    virtual void Setup(const Ptr<Models::ModelInstance>& inst, const Ptr<Models::ModelNode>& node, const Ptr<Models::ModelNodeInstance>& parentNodeInst);
    /// called when removed from ModelInstance
    virtual void Discard();
    /// render node specific debug shape
    virtual void RenderDebug();
    /// called when the node becomes visible with current time
    virtual void OnShow(Timing::Time time);
    /// called when the node becomes invisible
    virtual void OnHide(Timing::Time time);

    Ptr<ParticleSystemInstance> particleSystemInstance;    

	Ptr<Materials::SurfaceConstant> emitterOrientation;
    Ptr<Materials::SurfaceConstant> billBoard;
    Ptr<Materials::SurfaceConstant> bboxCenter;
    Ptr<Materials::SurfaceConstant> bboxSize;
    Ptr<Materials::SurfaceConstant> time;
    Ptr<Materials::SurfaceConstant> animPhases;
    Ptr<Materials::SurfaceConstant> animsPerSec;
    Ptr<Materials::SurfaceConstant> depthBuffer;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ParticleSystemInstance>&
ParticleSystemNodeInstance::GetParticleSystemInstance() const
{
    return this->particleSystemInstance;
}

} // namespace Particles
//------------------------------------------------------------------------------
    