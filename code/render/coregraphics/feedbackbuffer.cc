//------------------------------------------------------------------------------
//  feedbackbuffer.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/feedbackbuffer.h"

#if __OGL4__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::FeedbackBuffer, 'FEDB', OpenGL4::OGL4FeedbackBuffer);
}
#else
#error "FeedbackBuffer class not implemented on this platform!"
#endif

