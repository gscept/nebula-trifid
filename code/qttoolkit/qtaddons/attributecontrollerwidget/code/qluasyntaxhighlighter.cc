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

	// start with identifiers, everything else that matches will override this
	identifierFormat.setForeground(QBrush(qRgb(240, 200, 130)));
	rule.pattern = QRegExp("\\b[A-Za-z0-9_]+");
	rule.format = identifierFormat;
	highlightingRules.append(rule);

	keywordFormat.setForeground(QBrush(qRgb(110, 110, 230)));
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


	// string literals
	quotationFormat.setForeground(QBrush(qRgb(255, 230, 230)));
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

	// numbers
	numberFormat.setForeground(QBrush(qRgb(240, 240, 255)));
	rule.pattern = QRegExp("[0-9]+");
	rule.format = numberFormat;
	highlightingRules.append(rule);

	// function headers and calls
	functionFormat.setFontWeight(QFont::Bold);
	functionFormat.setForeground(QBrush(qRgb(250, 210, 10)));
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

	// operators
	operatorFormat.setForeground(QBrush(qRgb(255, 255, 255)));
	rule.pattern = QRegExp("[(){}*/+-,.\\^]");
	rule.format = operatorFormat;
	highlightingRules.append(rule);

	// add comments last
	singleLineCommentFormat.setForeground(QBrush(qRgb(0, 200, 0)));
	rule.pattern = QRegExp("--[^\n]*");
	rule.format = singleLineCommentFormat;
	highlightingRules.append(rule);

	// add these for multiline comments
	multiLineCommentFormat.setForeground(QBrush(qRgb(0, 200, 0)));
    commentStartExpression = QRegExp("--\\[\\[");
    commentEndExpression = QRegExp("\\]\\]");
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
