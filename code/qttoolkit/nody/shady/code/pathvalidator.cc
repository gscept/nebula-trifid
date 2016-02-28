//------------------------------------------------------------------------------
//  pathvalidator.h
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "pathvalidator.h"
#include <QRegExp>
#include <QStringList>

namespace QtToolkit
{

//------------------------------------------------------------------------------
/**
*/
PathValidator::PathValidator(const QString& extension, const QString& prefix) :
    extension(extension),
    prefix(prefix)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PathValidator::~PathValidator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
QValidator::State 
PathValidator::validate(QString& string, int& pos) const
{
	if (string.indexOf('/') == -1) return QValidator::Invalid;
	else
	{
		if (string.startsWith(this->prefix) && string.endsWith(this->extension))	return QValidator::Acceptable;
		else																		return QValidator::Intermediate;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PathValidator::fixup(QString& string) const
{
	// okay, so both tex: and .dds cannot exist if we get here, so if any do, we set the other
	if	(string.startsWith(this->prefix))		string.append(this->extension);
	else if (string.endsWith(this->extension))	string.prepend(this->prefix);
	else
	{
		string.prepend(this->prefix);
		string.append(this->extension);
	}
}
} // namespace QtToolkit