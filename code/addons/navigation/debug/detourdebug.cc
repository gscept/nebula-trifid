#include "stdneb.h"
#include "detourdebug.h"
#include "math/matrix44.h"
#include "coregraphics/primitivetopology.h"
#include "coregraphics/rendershape.h"
#include "debugrender/debugshaperenderer.h"

using namespace Math;
namespace Navigation
{

void 
DebugDraw::begin(duDebugDrawPrimitives prim, float size)
{
	points.Clear();
	current =  prim;
}

void 
DebugDraw::vertex(const float* pos, unsigned int color)
{
    unsigned char * c = (unsigned char*)(&color);
    CoreGraphics::RenderShape::RenderShapeVertex vert;
    vert.pos = vector(pos[0], pos[1], pos[2]);
    vert.color = float4(c[3] / 255.0f, c[2] / 255.0f, c[1] / 255.0f, c[0] / 255.0f);
	points.Append(vert);
}

void 
DebugDraw::vertex(const float x, const float y, const float z, unsigned int color)
{
    unsigned char * c = (unsigned char*)(&color);
    CoreGraphics::RenderShape::RenderShapeVertex vert;
    vert.pos = vector(x,y,z);
    vert.color = float4(c[3] / 255.0f, c[2] / 255.0f, c[1] / 255.0f, c[0] / 255.0f);
    points.Append(vert);
}

void
DebugDraw::vertex(const float* pos, unsigned int color, const float* uv)
{
    unsigned char * c = (unsigned char*)(&color);
    CoreGraphics::RenderShape::RenderShapeVertex vert;
    vert.pos = vector(pos[0], pos[1], pos[2]);
    vert.color = float4(c[3] / 255.0f, c[2] / 255.0f, c[1] / 255.0f, c[0] / 255.0f);
    points.Append(vert);
}

void 
DebugDraw::vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
{
    unsigned char * c = (unsigned char*)(&color);
    CoreGraphics::RenderShape::RenderShapeVertex vert;
    vert.pos = vector(x, y, z);
    vert.color = float4(c[3] / 255.0f, c[2] / 255.0f, c[1] / 255.0f, c[0] / 255.0f);
	points.Append(vert);
}

void 
DebugDraw::end()
{
	if(points.Size() == 0)
	{
		return;
	}	
	switch(current)
	{
		case DU_DRAW_POINTS:
		{		
			Debug::DebugShapeRenderer::Instance()->DrawPrimitives(Math::matrix44::identity(), CoreGraphics::PrimitiveTopology::PointList, points.Size(), points.Begin(), float4(1), CoreGraphics::RenderShape::AlwaysOnTop, NULL);
		}
		break;
		case DU_DRAW_LINES:
		{			
            Debug::DebugShapeRenderer::Instance()->DrawPrimitives(Math::matrix44::identity(), CoreGraphics::PrimitiveTopology::LineList, points.Size() / 2, points.Begin(), float4(1), CoreGraphics::RenderShape::AlwaysOnTop, NULL);
		}
		break;
		case DU_DRAW_TRIS:
		{
            Debug::DebugShapeRenderer::Instance()->DrawPrimitives(Math::matrix44::identity(), CoreGraphics::PrimitiveTopology::TriangleList, points.Size() / 3, points.Begin(), float4(1), CoreGraphics::RenderShape::AlwaysOnTop, NULL);
		}
		break;
		case DU_DRAW_QUADS:
		{
			n_assert(false);
		}
		break;			
	}

}


}