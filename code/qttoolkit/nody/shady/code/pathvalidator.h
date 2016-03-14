#pragma once
//------------------------------------------------------------------------------
/**
    @class QtToolkit::PathValidator
    
    Implements a QValidator for Nebula paths.
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QValidator>
namespace QtToolkit
{
class PathValidator : public QValidator
{
public:
	/// constructor
	PathValidator(const QString& extension, const QString& prefix);
	/// destructor
	virtual ~PathValidator();

    /// set the validator prefix, this is for example the tex: or mdl: part of the path
    void SetPrefix(const QString& string);
    /// set the file extension to match against
    void SetExtension(const QString& string);

    /// validates field
    QValidator::State validate(QString& string, int& pos) const;
    /// fixes field
    void fixup(QString& string) const;

private:
    QString extension;
    QString prefix;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
PathValidator::SetPrefix(const QString& string)
{
    this->prefix = string;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PathValidator::SetExtension(const QString& string)
{
    this->extension = string;
}

} // namespace QtToolkit
//------------------------------------------------------------------------------