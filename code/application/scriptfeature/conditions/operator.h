#pragma once
//------------------------------------------------------------------------------
/**
    @class Conditions::Operator
    
    Template operator class for numeric compare operations in scripts.

    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "util/string.h"

namespace Conditions
{
template<class TYPE>
class Operator
{
public:
    enum OpType
    {
        NotEqual = 0,
        Equal,
        Less,
        LessEqual,
        Greater,
        GreaterEqual,

        InvalidType
    };
    /// constructor
    Operator();    
    /// constructor
    Operator(OpType type);
    
    /// operator evaluation
    bool Evaluate(TYPE first, TYPE second) const;
    /// set type from string
    void FromString(const Util::String& str);

private:
    OpType opType;
};
//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
Operator<TYPE>::Operator() :
    opType(InvalidType)
{
}
//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
Operator<TYPE>::Operator(OpType type):
    opType(type)
{
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
bool Operator<TYPE>::Evaluate(TYPE first, TYPE second) const
{
    switch (this->opType)
    {
    case NotEqual: return first != second;
    case Equal: return first == second;
    case Less: return first < second;
    case LessEqual: return first <= second;
    case Greater: return first > second;
    case GreaterEqual: return first >= second;
    default: n_error("Operator::Invalid Operator type!");
    };

    return false;
}
//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void Operator<TYPE>::FromString(const Util::String& str)
{
    if ("notEqual" == str) this->opType = NotEqual;
    else if ("equal" == str) this->opType = Equal;
    else if ("less" == str) this->opType = Less;
    else if ("lessOrEqual" == str) this->opType = LessEqual;
    else if ("greater" == str) this->opType = GreaterEqual;
    else if ("greaterOrEqual" == str) this->opType = GreaterEqual;
}
}; // namespace Conditions
//------------------------------------------------------------------------------