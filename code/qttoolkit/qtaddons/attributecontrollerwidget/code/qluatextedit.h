#pragma once
//------------------------------------------------------------------------------
/**    
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/


#include <QPlainTextEdit>
#include "qluasyntaxhighlighter.h"

namespace QtAttributeControllerAddon
{

class QLuaTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit QLuaTextEdit(QWidget *parent = 0);
    ~QLuaTextEdit();
private:
	QLuaSyntaxHighlighter * highLight;
};
}