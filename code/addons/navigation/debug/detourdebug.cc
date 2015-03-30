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
	vector vec(pos[0],pos[1],pos[2]);
	points.Append(vec);
	unsigned char * c = (unsigned char*)(&color);
	lastColor.set(c[3]/255.0f,c[2]/255.0f,c[1]/255.0f,c[0]/255.0f);
}

void 
DebugDraw::vertex(const float x, const float y, const float z, unsigned int color)
{
	vector vec(x,y,z);
	points.Append(vec);
	unsigned char * c = (unsigned char*)(&color);
	lastColor.set(c[3]/255.0f,c[2]/255.0f,c[1]/255.0f,c[0]/255.0f);
}

void
DebugDraw::vertex(const float* pos, unsigned int color, const float* uv)
{
	vector vec(pos[0],pos[1],pos[2]);
	points.Append(vec);
	unsigned char * c = (unsigned char*)(&color);
	lastColor.set(c[3]/255.0f,c[2]/255.0f,c[1]/255.0f,c[0]/255.0f);
}

void 
DebugDraw::vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
{
	vector vec(x,y,z);
	points.Append(vec);
	unsigned char * c = (unsigned char*)(&color);
	lastColor.set(c[3]/255.0f,c[2]/255.0f,c[1]/255.0f,c[0]/255.0f);
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
			Debug::DebugShapeRenderer::Instance()->DrawPrimitives(Math::matrix44::identity(),CoreGraphics::PrimitiveTopology::PointList,points.Size(),points.Begin(),4,lastColor,CoreGraphics::RenderShape::AlwaysOnTop);
		}
		break;
		case DU_DRAW_LINES:
		{
			Debug::DebugShapeRenderer::Instance()->DrawPrimitives(Math::matrix44::identity(),CoreGraphics::PrimitiveTopology::LineList,points.Size()/2,points.Begin(),4,lastColor,CoreGraphics::RenderShape::AlwaysOnTop);
		}
		break;
		case DU_DRAW_TRIS:
		{
			Debug::DebugShapeRenderer::Instance()->DrawPrimitives(Math::matrix44::identity(),CoreGraphics::PrimitiveTopology::TriangleList,points.Size()/3,points.Begin(),4,lastColor,CoreGraphics::RenderShape::Wireframe);
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