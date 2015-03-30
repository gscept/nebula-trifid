//------------------------------------------------------------------------------
//  navigation/path3d.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "navigation/path3d.h"

namespace Navigation
{
__ImplementClass(Navigation::Path3D, 'NVP3',Core::RefCounted);


//------------------------------------------------------------------------------
/**
*/
Path3D::Path3D() : 	
    isValid(true)
{
}

//------------------------------------------------------------------------------
/**
*/
Path3D::~Path3D()
{
}

//------------------------------------------------------------------------------
/**
    return the length of the path. 
    A path with one node will return 0 length
*/
float
Path3D::GetLength() const
{
	Util::Array<Math::point>::Iterator iter = this->points.Begin();
    float length = 0.f;

    int idxPoint;
    for(idxPoint = 0; idxPoint + 1 < this->points.Size(); idxPoint++)
    {
        length += (this->points[idxPoint+1] - this->points[idxPoint]).length3();
    }

    return length;
}

} // namespace Navigation