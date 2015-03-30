//------------------------------------------------------------------------------
//  xmlwriter.cc
//  (C) 2010 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

// nebula3 includes
#include "io/xmlwriter.h"
#include "util/string.h"

// nebula2 includes
#include "mathlib/vector.h"
#include "util/nstring.h"

using namespace Util;

//------------------------------------------------------------------------------
/**
    Set the provided attribute from a nebula2 string
*/
template<>
void
IO::XmlWriter::Set(const String& name, const nString& value)
{
    n_assert(this->IsOpen());
    n_assert(0 != this->curNode);
    n_assert(name.IsValid());
    this->curNode->SetAttribute(name.AsCharPtr(), value.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
    Set the provided attribute from a const char*
*/
template<>
void
IO::XmlWriter::Set(const String& name, const char* const& value)
{
    n_assert(this->IsOpen());
    n_assert(0 != this->curNode);
    n_assert(name.IsValid());
    this->curNode->SetAttribute(name.AsCharPtr(), value);
}

//------------------------------------------------------------------------------
/**
    Set the provided attribute from a nebula2 vector3
*/
template<>
void
IO::XmlWriter::Set(const String& name, const vector3 &value)
{
    String s;
    s.Format("%.6f,%.6f,%.6f", value.x, value.y, value.z);
    this->SetString(name, s);
}

//------------------------------------------------------------------------------
/**
    Set the provided attribute from a nebula2 vector4
*/
template<>
void
IO::XmlWriter::Set(const String& name, const vector4 &value)
{
    String s;
    s.Format("%.6f,%.6f,%.6f,%.6f", value.x, value.y, value.z, value.w);
    this->SetString(name, s);
}
