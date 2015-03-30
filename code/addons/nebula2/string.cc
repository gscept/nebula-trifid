//------------------------------------------------------------------------------
//  string.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "util/string.h"
#include "mathlib/vector.h"
#include "mathlib/matrix.h"

//------------------------------------------------------------------------------
/**
*/
template<> vector3
Util::String::As() const
{
    Array<String> tokens = this->Tokenize(", \t");
    n_assert(tokens.Size() == 3);
    vector3 v(tokens[0].AsFloat(), tokens[1].AsFloat(), tokens[2].AsFloat());
    return v;
}

//------------------------------------------------------------------------------
/**
*/
template<> vector4
Util::String::As() const
{
    Array<String> tokens = this->Tokenize(", \t");
    n_assert(tokens.Size() == 4);
    vector4 v(tokens[0].AsFloat(), tokens[1].AsFloat(), tokens[2].AsFloat(), tokens[3].AsFloat());
    return v;
}

//------------------------------------------------------------------------------
/**
*/
template<> matrix44
Util::String::As() const
{
    Array<String> tokens(16, 0); 
    this->Tokenize(", \t", tokens);
    n_assert(tokens.Size() == 16);
    matrix44 m(tokens[0].AsFloat(),  tokens[1].AsFloat(),  tokens[2].AsFloat(),  tokens[3].AsFloat(),
               tokens[4].AsFloat(),  tokens[5].AsFloat(),  tokens[6].AsFloat(),  tokens[7].AsFloat(),
               tokens[8].AsFloat(),  tokens[9].AsFloat(),  tokens[10].AsFloat(), tokens[11].AsFloat(),
               tokens[12].AsFloat(), tokens[13].AsFloat(), tokens[14].AsFloat(), tokens[15].AsFloat());
    return m;
}

//------------------------------------------------------------------------------
/**
*/
template<> void
Util::String::Set(const vector3& v)
{
    this->Format("%.6f,%.6f,%.6f", v.x, v.y, v.z);
}

//------------------------------------------------------------------------------
/**
*/
template<> void
Util::String::Set(const vector4& v)
{
    this->Format("%.6f,%.6f,%.6f,%.6f", v.x, v.y, v.z, v.w);
}

//------------------------------------------------------------------------------
/**
*/
template<> void
Util::String::Set(const matrix44& m)
{
    this->Format("%.6f, %.6f, %.6f, %.6f, "
                 "%.6f, %.6f, %.6f, %.6f, "
                 "%.6f, %.6f, %.6f, %.6f, "
                 "%.6f, %.6f, %.6f, %.6f",
                 m.M11, m.M12, m.M13, m.M14,
                 m.M21, m.M22, m.M23, m.M24,
                 m.M31, m.M32, m.M33, m.M34,
                 m.M41, m.M42, m.M43, m.M44);
}

//------------------------------------------------------------------------------
/**
*/
template<> Util::String
Util::String::From(const vector3& v)
{
    Util::String str;
    str.Set<_vector3>(v);
    return str;
}

//------------------------------------------------------------------------------
/**
*/
template<> Util::String
Util::String::From(const vector4& v)
{
    Util::String str;
    str.Set<_vector4>(v);
    return str;
}

//------------------------------------------------------------------------------
/**
*/
template<> Util::String
Util::String::From(const matrix44& m)
{
    Util::String str;
    str.Set<matrix44>(m);
    return str;
}

//------------------------------------------------------------------------------
/**
*/
template<> bool
Util::String::IsValid<vector3>() const
{
    return this->CheckValidCharSet(" \t-+.,e1234567890");
}

//------------------------------------------------------------------------------
/**
*/
template<> bool
Util::String::IsValid<vector4>() const
{
    return this->CheckValidCharSet(" \t-+.,e1234567890");
}

//------------------------------------------------------------------------------
/**
*/
template<> bool
Util::String::IsValid<matrix44>() const
{
    return this->CheckValidCharSet(" \t-+.,e1234567890");
}

//------------------------------------------------------------------------------
/**
*/
template<> void
Util::String::Append(const vector3& v)
{
    this->Append(From<vector3>(v));    
}

//------------------------------------------------------------------------------
/**
*/
template<> void
Util::String::Append(const vector4& v)
{
    this->Append(From<vector4>(v));    
}

//------------------------------------------------------------------------------
/**
*/
template<> void
Util::String::Append(const matrix44& v)
{
    this->Append(From<matrix44>(v));    
}
