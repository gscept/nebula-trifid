#include "stdneb.h"
#include "outputstream.h"
#include <QScrollBar>

namespace QtTools
{
//------------------------------------------------------------------------------
/**
*/
OutputStream::OutputStream() :
	stream(0),
	edit(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
OutputStream::~OutputStream()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
OutputStream::Setup( QTextStream* stream )
{
	Q_ASSERT(stream);
	if (this->edit)
	{
		qErrnoWarning("OutputStream is already setup as a QTextEdit!");
	}
	else
	{
		this->stream = stream;
	}		
}

//------------------------------------------------------------------------------
/**
*/
void
OutputStream::Setup( QTextEdit* textEdit )
{
	Q_ASSERT(textEdit);
	if (this->stream)
	{
		qErrnoWarning("OutputStream is already setup as a QTextStream!");
	}
	else
	{
		this->edit = textEdit;
	}
	
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputStream::Setup( QPlainTextEdit* textEdit )
{
	Q_ASSERT(textEdit);
	if (this->stream)
	{
		qErrnoWarning("OutputStream is already setup as a QPlainTextStream!");
	}
	else
	{
		this->plainEdit = textEdit;
	}
}
//------------------------------------------------------------------------------
/**
*/
void
OutputStream::Write( const QString& string )
{
	if (this->stream)
	{
		*this->stream << string << "\n";
		this->stream->flush();
	}
	else if (this->edit)
	{
        this->edit->insertHtml(string);
		QScrollBar* sb = this->edit->verticalScrollBar();
		sb->setSliderPosition(sb->maximum());
	}
	else if (this->plainEdit)
	{
		this->plainEdit->appendPlainText(string);
		QScrollBar* sb = this->plainEdit->verticalScrollBar();
		sb->setSliderPosition(sb->maximum());
	}
	else
	{
		qErrnoWarning("OutputStream is not setup!");
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputStream::WriteWarning( const QString& string )
{
	if (this->stream)
	{
		*this->stream << string << "\n";
		this->stream->flush();
	}
	else if (this->edit)
	{
		this->edit->insertHtml("<a style=\"background-color: yellow; color: black\">" + string + "</a><br>");
		QScrollBar* sb = this->edit->verticalScrollBar();
		sb->setSliderPosition(sb->maximum());
	}
	else if (this->plainEdit)
	{
		this->plainEdit->appendPlainText(string);
		QScrollBar* sb = this->plainEdit->verticalScrollBar();
		sb->setSliderPosition(sb->maximum());
	}
	else
	{
		qErrnoWarning("OutputStream is not setup!");
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputStream::WriteError( const QString& string )
{
	if (this->stream)
	{
		*this->stream << string << "\n";
		this->stream->flush();
	}
	else if (this->edit)
	{
		this->edit->insertHtml("<a style=\"background-color: red; color: black\">" + string + "</a><br>");
		QScrollBar* sb = this->edit->verticalScrollBar();
		sb->setSliderPosition(sb->maximum());
	}
	else if (this->plainEdit)
	{
		this->plainEdit->appendPlainText(string);
		QScrollBar* sb = this->plainEdit->verticalScrollBar();
		sb->setSliderPosition(sb->maximum());
	}
	else
	{
		qErrnoWarning("OutputStream is not setup!");
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
OutputStream::Clear()
{
	if (this->stream)
	{	
		this->stream->device()->reset();
	}
	else if (this->edit)
	{
		this->edit->clear();
	}
	else if (this->plainEdit)
	{
		this->plainEdit->clear();
	}
	else
	{
		qErrnoWarning("OutputStream is not setup!");
	}
}
}