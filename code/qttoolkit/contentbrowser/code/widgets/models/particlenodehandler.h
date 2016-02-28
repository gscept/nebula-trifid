#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::ParticleNodeHandler
    
    Handles signals from ParticleNodeFrame so that it updates the actual model node.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "ui_particlenodeinfowidget.h"
#include "modelhandler.h"
#include "particles/emitterattrs.h"
#include "modelnodehandler.h"
#include "resources/managedmesh.h"
namespace Widgets
{
class ParticleNodeHandler : 
	public ModelNodeHandler
{
	Q_OBJECT
	__DeclareClass(ParticleNodeHandler);
public:
	/// constructor
	ParticleNodeHandler();
	/// destructor
	virtual ~ParticleNodeHandler();

	/// sets pointer to ui
	void SetUi(Ui::ParticleNodeInfoWidget* ui);
	/// gets pointer to the ui
	Ui::ParticleNodeInfoWidget* GetUi() const;

	/// constructs internal structure for particle node
	void Setup(const Util::String& resource);

	/// handle soft reset
	virtual void Refresh();

	/// discard handler
	void Discard();

private slots:
	/// called from an envelope frame whenever a value has changed
	void EnvelopeChanged(Particles::EmitterAttrs::EnvelopeAttr attr);

	/// called whenever emission duration has changed
	void EmissionDurationChanged(double f);
	/// called whenever the activity distance has changed
	void ActivityDistanceChanged(double f);
	/// called whenever start rotation min has changed
	void StartRotationMinChanged(double f);
	/// called whenever start rotation max has changed
	void StartRotationMaxChanged(double f);
	/// called whenever the gravity has changed
	void GravityChanged(double f);
	/// called whenever the particle strectch has changed
	void ParticleStretchChanged(double f);
	/// called whenever velocity randomize has changed
	void VelocityRandomizeChanged(double f);
	/// called whenever rotation randomize has changed
	void RotationRandomizeChanged(double f);
	/// called whenever size randomize has changed
	void SizeRandomizeChanged(double f);
	/// called whenever precalc time has changed
	void PrecalcTimeChanged(double f);
	/// called whenever start delay has changed
	void StartDelayChanged(double f);
	/// called whenever texture tile has changed
	void TextureTileChanged(double f);
	/// called whenever phases per second has changed
	void PhasesPerSecondChanged(double f);
	/// called whenever the wind direction has changed
	void WindDirectionChanged();

	/// called whenever looping has changed
	void LoopingChanged(bool b);
	/// called whenever randomize rotation has changed
	void RandomizeRotationChanged(bool b);
	/// called whenever stretch-to-start has changed
	void StretchToStartChanged(bool b);
	/// called whenever render oldest first has changed
	void RenderOldestFirst(bool b);
	/// called whenever view angle fade has changed
	void ViewAngleFadeChanged(bool b);
	/// called whenever billboard has changed
	void BillboardChanged(bool b);

	/// called whenever stretch detail has changed
	void StretchDetailChanged(int i);
	/// called whenever anim phases has changed
	void AnimPhasesChanged(int i);

	/// called whenever the emitter mesh primitive group index has changed
	void PrimitiveGroupIndexChanged(int i);
	/// called whenever the single point emitter mesh button gets pressed
	void SinglePointEmitterPressed();
	/// browse a mesh
	void BrowseMesh();
	/// restart particle
	void Restart();

	/// called when the delete button is pressed
	void OnDeleteParticleNode();

private:

	/// applies modifications
	void Apply();

	bool isUpdatingValues;

	Ui::ParticleNodeInfoWidget* particleUi;
	Particles::EmitterAttrs attrs;
	IndexT primGroupIndex;
	Ptr<CoreGraphics::Mesh> emitterMesh;
	Ptr<Resources::ManagedMesh> managedEmitterMesh;
	Ptr<Particles::ParticleSystemNodeInstance> particleStateNode;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
ParticleNodeHandler::SetUi(Ui::ParticleNodeInfoWidget* ui)
{
	n_assert(ui);
	this->particleUi = ui;
}

//------------------------------------------------------------------------------
/**
*/
inline Ui::ParticleNodeInfoWidget* 
ParticleNodeHandler::GetUi() const
{
	return this->particleUi;
}

} // namespace Widgets
//------------------------------------------------------------------------------