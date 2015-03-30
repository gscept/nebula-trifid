#pragma once
//------------------------------------------------------------------------------
/**
    @file coregraphics/config.h
    
    Compile time configuration options for the CoreGraphics subsystem.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/types.h"

//------------------------------------------------------------------------------
#if __DX11__ || __DX9__
	#define NEBULA3_USEDIRECT3D9 (1)
	#define NEBULA3_USEDIRECT3D10 (0)

	#define NEBULA3_DIRECT3D_USENVPERFHUD (0)
	#define NEBULA3_DIRECT3D_DEBUG (0)

	#if NEBULA3_DIRECT3D_USENVPERFHUD
	#define NEBULA3_DIRECT3D_DEVICETYPE D3DDEVTYPE_REF
	#else
	#define NEBULA3_DIRECT3D_DEVICETYPE D3DDEVTYPE_HAL
	#endif
#elif __OGL4__
    #ifdef _DEBUG
	    #define NEBULA3_OPENGL4_DEBUG (1)
    #else
        #define	NEBULA3_OPENGL4_DEBUG (0)
    #endif
#endif
//------------------------------------------------------------------------------
