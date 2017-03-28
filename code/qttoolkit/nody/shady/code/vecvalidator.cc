//------------------------------------------------------------------------------
//  vecvalidator.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vecvalidator.h"
#include <QRegExp>
#include <QStringList>

namespace QtToolkit
{

//------------------------------------------------------------------------------
/**
*/
VecValidator::VecValidator(int elements, int decimals, int digits, const Util::Variant::Type& type) :
    elements(elements),
    decimals(decimals),
    digits(digits),
    type(type)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VecValidator::~VecValidator()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
QValidator::State 
VecValidator::validate(QString& string, int& pos) const
{
    QString pattern;
    if (this->type == Util::Variant::Float)
    {
        QString floating = "(\\+|\\-)?[0-9]{1," + QString::number(this->digits) + "}\\.[0-9]{0," + QString::number(this->decimals) + "}";
        pattern = floating + "\\s?(," + floating + "){" + QString::number(this->elements - 1) + "}";
    }
    else if (this->type == Util::Variant::Bool)
    {
        QString boolean = "(true|false)";
        pattern = boolean + "\\s?(," + boolean + "){" + QString::number(this->elements - 1) + "}";
    }
    else if (this->type == Util::Variant::Int)
    {
        QString integer = "(\\+|\\-)?[0-9]{1," + QString::number(this->digits) + "}";
        pattern = integer + "\\s?(," + integer + "){" + QString::number(this->elements - 1) + "}";
    }

    QRegExp exp;
    exp.setPattern(pattern);
    if (!exp.exactMatch(string))
    {
        if (exp.matchedLength() == 0)   return QValidator::Invalid;
        else
        {
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
VecValidator::fixup(QString& string) const
{
    if (string.endsWith(",")) string.truncate(string.length()-1);
    QStringList parts = string.split(",");
    if (this->elements > parts.size())
    {
        int filler = this->elements - parts.size();
        if (this->type == Util::Variant::Float)
        {
            QString floating = ",0.000000";
            int i;
            for (i = 0; i < filler; i++)
            {
                string.append(floating);
            }
        }
        else if (this->type == Util::Variant::Bool)
        {
            QString boolean = ",false";
            int i;
            for (i = 0; i < filler; i++)
            {
                string.append(boolean);
            }
        }
        else if (this->type == Util::Variant::Int)
        {
            QString integer = ",0";
            int i;
            for (i = 0; i < filler; i++)
            {
                string.append(integer);
            }
        }
    }
    else
    {
        // empty string first
        string = "";
        int i;
        for (i = 0; i < this->elements; i++)
        {
            const QString& part = parts[i].trimmed();

            if (this->type == Util::Variant::Float)
            {
                QStringList floater = part.split(".");
                if (floater.size() > 1)
                {
                    QString p1 = floater[0];
                    QString p2 = floater[1];
                    p1.truncate(this->digits);
                    p2.truncate(this->decimals);
                    QString val = p1 + "." + p2;
                    string.append(val + ",");
                }
                else
                {
                    QString p1 = floater[0];
                    QString p2;
                    p2.fill('0', this->decimals);
                    QString val = p1 + "." + p2;
                    string.append(val + ",");
                }                
            }
            else if (this->type == Util::Variant::Bool)
            {
                string.append(part + ",");
            }
            else if (this->type == Util::Variant::Int)
            {
                QString val = part;
                val.truncate(this->digits);
                string.append(val + ",");
            }
        }

        // strip last comma
        string.truncate(string.length()-1);
    }
}

} // namespace QtToolkit