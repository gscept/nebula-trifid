/* QtLua -- Lua bindings for Qt
   Copyright (C) 2011, Jarek Pelczar

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General
   Public License along with this library; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301 USA

*/
#include "stdneb.h"
#include "qluasyntaxhighlighter.h"
#include <QVariant>
#include "scripting/scriptserver.h"
#include "scripting/command.h"

//------------------------------------------------------------------------------
/**
*/
QLuaSyntaxHighlighter::QLuaSyntaxHighlighter(QTextDocument *document) :
    QSyntaxHighlighter(document)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::yellow);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\band\\b" << "\\bbreak\\b" << "\\bdo\\b"
                    << "\\belse\\b" << "\\belseif\\b" << "\\bend\\b"
                    << "\\bfalse\\b" << "\\bfor\\b" << "\\bfunction\\b"
                    << "\\bif\\b" << "\\bin\\b" << "\\blocal\\b"
                    << "\\bnil\\b" << "\\bnot\\b" << "\\bor\\b"
                    << "\\brepeat\\b" << "\\breturn\\b" << "\\bthen\\b"
                    << "\\btrue\\b" << "\\buntil\\b" << "\\bwhile\\b";

	for(IndexT i = 0; i < Scripting::ScriptServer::Instance()->GetNumCommands(); i++)
	{
		Util::String fun;
		fun.Format("\\%s\\b",Scripting::ScriptServer::Instance()->GetCommandByIndex(i)->GetName().AsCharPtr());
		keywordPatterns << fun.AsCharPtr();
	}

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    quotationFormat.setForeground(Qt::white);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

	functionFormat.setFontWeight(QFont::Bold);
    functionFormat.setForeground(Qt::cyan);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

//------------------------------------------------------------------------------
/**
*/
QLuaSyntaxHighlighter::~QLuaSyntaxHighlighter()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void QLuaSyntaxHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                    + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
