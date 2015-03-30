//------------------------------------------------------------------------------
//  pathvalidator.h
//  (C) 2015 Individual contributors, see AUTHORS file
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
    QString pattern;
    pattern = this->prefix + "\\:" + "([A-z]?[A-z|0-9]*)" + "(/[A-z]?[A-z|0-9]*)*" + "\\." + this->extension;
    QRegExp exp;
    exp.setPattern(pattern);
    if (!exp.exactMatch(string))
    {
        if (exp.matchedLength() == 0) return QValidator::Invalid;
        else
        {
            int nummatched = exp.matchedLength();
            this->fixup(string);
            return QValidator::Intermediate;
        }
    }
    else return QValidator::Acceptable;
}

//------------------------------------------------------------------------------
/**
*/
void 
PathValidator::fixup(QString& string) const
{
    QStringList path = string.split("/");
    string = "";
    QString result;
    int i;
    for (i = 0; i < path.size(); i++)
    {
        QString fragment = path[i];
        int index = fragment.indexOf(":");
        if (index != -1) fragment = fragment.mid(index+1);
        index = fragment.indexOf(".");
        if (index != -1) fragment = fragment.left(index);
        string.append(fragment);
    }
    string = this->prefix + ":" + string + "." + this->extension;
}
} // namespace QtToolkit