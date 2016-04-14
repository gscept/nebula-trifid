#pragma once
//------------------------------------------------------------------------------
/**
    @class Navigation::Path3D

    Paths in a 3d environment with several properties.

    (C) 2015-2016 Individual contributors, see AUTHORS file
*/

#include "util/array.h"
#include "math/point.h"
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace Navigation
{
class Path3D : public Core::RefCounted
{
	__DeclareClass(Path3D);
public:
	/// Constructor
	Path3D();
    /// Destruct.
    virtual ~Path3D();
    /// Remove all information.
    void Clear();
    /// Number of path segments.
    int CountSegments() const;
    /// Extend path by point `v'.
	void Extend(const Math::point& v);
    /// Remove an element from the path
    void Remove(int index);
    /// Get point array.
	const Util::Array<Math::point>& GetPoints() const;
	/// set target
	void SetTarget(const Math::point& target);
	/// get target 
	const Math::point& GetTarget() const;
    /// get length
    float GetLength() const;

    /// return true if a path was found
    bool IsValid() const;
    /// return false if no path is available
    void SetValid(bool b);

private:
	Util::Array<Math::point> points;
	Math::point target;	
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
Path3D::Clear()
{
    points.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
Path3D::CountSegments() const
{
    return points.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Path3D::Extend(const Math::point& v)
{
    points.Append(v);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Path3D::Remove(int index)
{
    points.EraseIndex(index);
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::Array<Math::point>&
Path3D::GetPoints() const
{
    return points;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Path3D::SetTarget(const Math::point& target)
{
	this->target = target;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::point&
Path3D::GetTarget() const
{
	return this->target;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Path3D::SetValid(bool b)
{
	this->isValid = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Path3D::IsValid() const
{
	return this->isValid;
}

} // namespace Navigation

//------------------------------------------------------------------------------
