#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::View
  
    A graphics View is used to render a Stage through a CameraEntity into a 
    RenderTarget. Any number of views can be associated with the same Stage.
    Views may depend on other views. When a View is rendered, it will
    first ask the Views it depends on to render themselves. Subclasses of
    View may implement their own rendering strategies.

    Views can also use a resolve rectangle, which allows them to render to a 
    square in their display.

    FIXME:
        Views should be attached to displays.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/    
#include "core/refcounted.h"
#include "util/stringatom.h"
#include "graphics/stage.h"
#include "graphics/cameraentity.h"
#include "coregraphics/texture.h"
#include "coregraphics/rendertarget.h"
#include "rendermodules/rt/rtpluginregistry.h"
#include "frame/frameshader.h"
#include "debug/debugtimer.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class View : public Core::RefCounted
{
    __DeclareClass(View);
public:
    /// constructor
    View();
    /// destructor
    virtual ~View();

    /// return true if currently attached to graphics server
    bool IsAttachedToServer() const;
    /// get human-readable name
    const Util::StringAtom& GetName() const;
	/// get window id used for this view, -1 if not rendering to a window
	const IndexT GetWindowId() const;
	/// set the stage this View is associated with
	void SetStage(const Ptr<Stage>& stage);
    /// get the stage this View is associated with
    const Ptr<Stage>& GetStage() const;
    /// set the CameraEntity this View looks through
    void SetCameraEntity(const Ptr<CameraEntity>& camera);
    /// get the CameraEntity this View looks through
    const Ptr<CameraEntity>& GetCameraEntity() const;
    /// set the view's frame shader 
    void SetFrameShader(const Ptr<Frame::FrameShader>& frameShader);
    /// get the view's frame shader
    const Ptr<Frame::FrameShader>& GetFrameShader() const;
	/// sets the view's resolve rect
	void SetResolveRect(const Math::rectangle<int>& rect);
	/// gets the view's resolve rect
	const Math::rectangle<int>& GetResolveRect() const;
    /// returns true if view should use resolve rect
    const bool UseResolveRect() const;
	/// sets if this view should be updated automatically every frame
	void SetUpdatePerFrame(bool b);
	/// gets if this view should be updated every frame
	const bool GetUpdatePerFrame() const;

    /// add a view which this view depends on
    void AddDependency(const Ptr<View>& view);
    /// get all dependency views
    const Util::Array<Ptr<View> >& GetDependencies() const;

    /// update the visibility links for this view 
    virtual void UpdateVisibilityLinks();
    /// apply camera settings
    void ApplyCameraSettings();
    /// render the view into its render target
    virtual void Render(IndexT frameIndex);
    /// render a debug view of the world
    virtual void RenderDebug();
	/// handle on frame callback from main rendering pipeline
	virtual void OnFrame(const Ptr<RenderModules::RTPluginRegistry>& pluginRegistry, Timing::Time curTime, Timing::Time globalTimeFactor, bool renderDebug, bool updateCamera = true);

protected:
    friend class GraphicsServer;
	friend class ViewDisplayHandler;

    /// set a human-readable name of the view
    void SetName(const Util::StringAtom& name);
	/// set window id used by view
	void SetWindowId(const IndexT index);
    /// called when attached to graphics server
    virtual void OnAttachToServer();
    /// called when detached from graphics server
    virtual void OnRemoveFromServer();
    /// resolve visible lights
    void ResolveVisibleLights(IndexT frameIndex);
    /// resolve visibility for optimal batch rendering
    void ResolveVisibleModelNodeInstances(IndexT frameIndex);
	/// resolve visibility for shadow casting entities
	void ResolveVisibleShadowCasters(IndexT frameIndex);

	/// handle window resizing
	virtual void OnWindowResized(IndexT windowId) const;

	IndexT windowId;
	bool shouldUpdatePerFrame;
    bool isAttachedToServer;
    Util::StringAtom name;
    Ptr<Stage> stage;
    Ptr<CameraEntity> camera;
    Ptr<Frame::FrameShader> frameShader;
    Util::Array<Ptr<View> > dependencies;
	bool resolveRectValid;
	Math::rectangle<int> resolveRect;
    
	_declare_timer(resolveVisibleShadowCasters);
    _declare_timer(resolveVisibleModelNodeInstances);
	_declare_timer(updateShadowBuffers);
	_declare_timer(picking);
	_declare_timer(render);

	_declare_timer(ViewEndFrame);
	_declare_timer(ViewRender);
	_declare_timer(ViewUpdateVisibilityLinks);
	_declare_timer(ViewUpdateLightLinks);
};

//------------------------------------------------------------------------------
/**
*/
inline bool
View::IsAttachedToServer() const
{
    return this->isAttachedToServer;
}

//------------------------------------------------------------------------------
/**
*/
inline void
View::SetName(const Util::StringAtom& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
View::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline void
View::SetWindowId(const IndexT index)
{
	this->windowId = index;
}

//------------------------------------------------------------------------------
/**
*/
inline const IndexT
View::GetWindowId() const
{
	return this->windowId;
}

//------------------------------------------------------------------------------
/**
*/
inline void
View::SetStage(const Ptr<Stage>& s)
{
    this->stage = s;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Stage>&
View::GetStage() const
{
    return this->stage;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CameraEntity>&
View::GetCameraEntity() const
{
    return this->camera;
}

//------------------------------------------------------------------------------
/**
*/
inline void
View::AddDependency(const Ptr<View>& depView)
{
    this->dependencies.Append(depView);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<View> >&
View::GetDependencies() const
{
    return this->dependencies;
}

//------------------------------------------------------------------------------
/**
*/
inline void
View::SetFrameShader(const Ptr<Frame::FrameShader>& shd)
{
    this->frameShader = shd;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Frame::FrameShader>&
View::GetFrameShader() const
{
    return this->frameShader;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
View::SetResolveRect( const Math::rectangle<int>& rect )
{
	this->resolveRect = rect;
	this->resolveRectValid = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::rectangle<int>& 
View::GetResolveRect() const
{
	return this->resolveRect;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
View::UseResolveRect() const
{
    return this->resolveRectValid;
}

//------------------------------------------------------------------------------
/**
*/
inline void
View::SetUpdatePerFrame(bool b)
{
	this->shouldUpdatePerFrame = b;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
View::GetUpdatePerFrame() const
{
	return this->shouldUpdatePerFrame;
}

} // namespace Graphics
//------------------------------------------------------------------------------
