#pragma once
//------------------------------------------------------------------------------
/**
    @class QtToolkit::VecValidator
    
    Validates field as vector of two values.
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QValidator>
#include "util/variant.h"
namespace QtToolkit
{
class VecValidator : public QValidator
{
public:
	/// constructor
    VecValidator(int elements = 1, int decimals = 6, int digits = 6, const Util::Variant::Type& type = Util::Variant::Float);
	/// destructor
	virtual ~VecValidator();

    /// set the vector type
    void SetType(const Util::Variant::Type& type);
    /// set the number of vector elements
    void SetElements(int elements);
    /// set the number of decimals to use for floating types
    void SetDecimals(int decimals);
    /// set the number of digits to be used by the field
    void SetDigits(int digits);

    /// validates field
    QValidator::State validate(QString& string, int& pos) const;
    /// fixes field
    void fixup(QString& string) const;

private:
    int elements;
    int decimals;
    int digits;
    Util::Variant::Type type;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
VecValidator::SetType(const Util::Variant::Type& type)
{
    this->type = type;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
VecValidator::SetElements(int elements)
{
    n_assert(elements > 0);
    this->elements = elements;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
VecValidator::SetDecimals(int decimals)
{
    n_assert(decimals > 0);
    this->decimals = decimals;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
VecValidator::SetDigits(int digits)
{
    n_assert(digits > 0);
    this->digits = digits;
}

} // namespace QtToolkit
//------------------------------------------------------------------------------