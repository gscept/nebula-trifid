#pragma once
//------------------------------------------------------------------------------
/**
    @class Godrays::GodrayServer
    
    Render-thread side server which handles the sun.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "core/singleton.h"
#include "coregraphics/shaderinstance.h"
#include "internalgraphics/internalmodelentity.h"
#include "frame/frameshader.h"
#include "coregraphics/shadervariable.h"
#include "resources/managedtexture.h"

namespace Godrays
{
class GodrayServer : public Core::RefCounted
{
	__DeclareSingleton(GodrayServer);	
	__DeclareClass(GodrayServer);
public:
	/// constructor
	GodrayServer();
	/// destructor
	virtual ~GodrayServer();
	/// open the GodrayServer
	bool Open();
	/// close the GodrayServer
	void Close();
	/// return if GodrayServer is open
	bool IsOpen() const;
	
	/// sets the sun relative position flag
	void SetSunLightRelative(bool b);
	/// gets the sun relative position flag
	bool GetSunLightRelative() const;
	/// sets the sun color
	void SetSunColor(const Math::float4& color);
	/// gets the sun color
	const Math::float4& GetSunColor() const;
	/// sets the sun position
	void SetSunPosition(const Math::float4& pos);
	/// gets the sun position
	const Math::float4& GetSunPosition() const;
	/// sets the sun texture
	void SetSunTexture(const Resources::ResourceId& res);
	/// gets the sun texture
	const Resources::ResourceId& GetSunTexture() const;

	/// perform before rendering
	void OnRenderBefore();
	/// perform after rendering
	void OnRenderAfter();

	/// updates camera object
	void UpdateCamera();
private:
	bool isOpen;

	bool firstFrame;
	bool sunLightRelative;
	Math::float4 sunPosition;
	Math::float4 sunColor;
	Resources::ResourceId sunTexture;
	bool colorDirty;
	bool textureDirty;

	Ptr<InternalGraphics::InternalCameraEntity> camera;
	Ptr<Frame::FrameShader> godRayFrameShader;
	Ptr<InternalGraphics::InternalModelEntity> sunEntity;
	Ptr<Resources::ManagedTexture> whiteMap;
	Ptr<Resources::ManagedTexture> currentTexture;

	Ptr<CoreGraphics::ShaderInstance> godRayShader;
	Ptr<CoreGraphics::ShaderVariable> godRayTex;
	Ptr<CoreGraphics::ShaderVariable> godRayLightPos;
	Ptr<CoreGraphics::ShaderVariable> godRayDensity;
	Ptr<CoreGraphics::ShaderVariable> godRayDecay;
	Ptr<CoreGraphics::ShaderVariable> godRayWeight;
	Ptr<CoreGraphics::ShaderVariable> godRayExposure;

	Ptr<CoreGraphics::Texture> unshadedBuffer;

	Ptr<CoreGraphics::ShaderInstance> sunShader;
	Ptr<CoreGraphics::ShaderVariableInstance> sunTextureVar;
	Ptr<CoreGraphics::ShaderVariableInstance> sunColorVar;
}; 

//------------------------------------------------------------------------------
/**
*/
inline bool
GodrayServer::IsOpen() const
{
	return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
GodrayServer::SetSunLightRelative( bool b )
{
	this->sunLightRelative = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
GodrayServer::GetSunLightRelative() const
{
	return this->sunLightRelative;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
GodrayServer::SetSunColor( const Math::float4& color )
{
	this->sunColor = color;
	this->colorDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4& 
GodrayServer::GetSunColor() const
{
	return this->sunColor;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
GodrayServer::SetSunPosition( const Math::float4& pos )
{
	this->sunPosition = pos;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4& 
GodrayServer::GetSunPosition() const
{
	return this->sunPosition;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
GodrayServer::SetSunTexture( const Resources::ResourceId& res )
{
	this->sunTexture = res;
	this->textureDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId& 
GodrayServer::GetSunTexture() const
{
	return this->sunTexture;
}

} // namespace Godrays
//------------------------------------------------------------------------------