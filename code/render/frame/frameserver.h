#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FrameServer
    
    Server object of the frame subsystem. Factory for FrameShaders.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "resources/resourceid.h"
#include "frame/frameshader.h"

//------------------------------------------------------------------------------
namespace Frame
{
class FrameServer : public Core::RefCounted
{
    __DeclareClass(FrameServer);
    __DeclareSingleton(FrameServer);
public:
    /// constructor
    FrameServer();
    /// destructor
    virtual ~FrameServer();
    /// open the frame server (loads all frame shaders)
    bool Open();
    /// close the frame server
    void Close();
    /// return true if open
    bool IsOpen() const;
	/// returns true if frame shader exists
	bool HasFrameShader(const Resources::ResourceId& name);
    /// gain access to a frame shader by name, shader will be loaded if not happened yet
    Ptr<FrameShader> LookupFrameShader(const Resources::ResourceId& name);
	/// discards frame shader
	void DiscardFrameShader(const Ptr<FrameShader>& frameShader);

	/// called when resize is done
	void DisplayResized(SizeT width, SizeT height);

private:
    /// load a frame shader
    void LoadFrameShader(const Resources::ResourceId& name);

    Util::Dictionary<Resources::ResourceId, Ptr<FrameShader> > frameShaders;
    bool isOpen;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
FrameServer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
FrameServer::HasFrameShader( const Resources::ResourceId& name )
{
	return this->frameShaders.Contains(name);
}

} // namespace Frame
//------------------------------------------------------------------------------

    