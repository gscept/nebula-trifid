#pragma once
//------------------------------------------------------------------------------
/**
    @class Algorithm::AlgorithmBase
    
    Base class for render algorithms. 
    Defines inputs as textures and outputs as textures.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "algorithmhandler.h"
#include "coregraphics/texture.h"
#include "coregraphics/rendertarget.h"
#include "algorithm/algorithmprotocol.h"

namespace Algorithm
{
class AlgorithmBase : public Core::RefCounted
{
    __DeclareClass(AlgorithmBase);
public:
    /// constructor
    AlgorithmBase();
    /// destructor
    virtual ~AlgorithmBase();

    /// sets up algorithm, override to supply input/output initialization
    virtual void Setup();
    /// discards algorithm, override to clean up render targets and inputs/outputs
    virtual void Discard();

    /// enabled/disable algorithm, remember to call this in subclass!
    virtual void SetEnabled(bool b);

    /// adds input
    void AddInput(const Ptr<CoreGraphics::Texture>& tex);
    /// adds output, this will result in a render target with the name provided
    void AddOutput(const Util::String& name);
	/// adds render target, this is assumed to be fed to an algorithm which writes to an already existing buffer
	void AddRenderTarget(const Ptr<CoreGraphics::RenderTarget>& rt);

    /// prepares algorithm, perform before-frame stuff
    virtual void Prepare();
    /// executes algorithm, runs on-frame stuff
    virtual void Execute();
    /// finishes algorithm, perform after-frame stuff
    virtual void Finish();

    /// handles display resizing, since algorithms are completely dependent on frame shader textures, this is vital
	virtual void OnDisplayResized(SizeT width, SizeT height);

    /// handle message
    virtual bool HandleMessage(const Ptr<Messaging::Message>& msg);

protected:
    /// enabled algorithm
    virtual void Enable();
    /// disable algorithm
    virtual void Disable();

    Ptr<AlgorithmHandler> handler;
    Util::Array<Util::String> outputNames;
    Util::Array<Ptr<CoreGraphics::Texture>> inputs;
	Util::Array<Ptr<CoreGraphics::RenderTarget>> targets;
    bool enabled;
    bool deferredSetup;
}; 


//------------------------------------------------------------------------------
/**
*/
inline void 
AlgorithmBase::AddInput( const Ptr<CoreGraphics::Texture>& tex )
{
	n_assert(tex.isvalid());
	this->inputs.Append(tex);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AlgorithmBase::AddOutput( const Util::String& name )
{
	this->outputNames.Append(name);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AlgorithmBase::AddRenderTarget( const Ptr<CoreGraphics::RenderTarget>& rt )
{
	n_assert(rt.isvalid());
	this->targets.Append(rt);
}

} // namespace Algorithm
//------------------------------------------------------------------------------