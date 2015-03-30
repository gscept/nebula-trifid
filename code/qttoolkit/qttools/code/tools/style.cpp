#include "stdneb.h"
#include "style.h"
#include <QFile>

namespace QtTools
{


	//------------------------------------------------------------------------------
	/**
	*/
	Style::Style()
	{
		this->Setup();
	}

	//------------------------------------------------------------------------------
	/**
	*/
	Style::~Style()
	{
		this->stylesheet.clear();
	}

	//------------------------------------------------------------------------------
	/**
	*/
	void 
	Style::OverrideStyleSheet( const QString& overrideSheet )
	{
		this->Setup(overrideSheet);
	}

	//------------------------------------------------------------------------------
	/**
	*/
	const QString& 
	Style::GetStyle() const
	{
		return this->stylesheet;
	}

	//------------------------------------------------------------------------------
	/**
		Sets up default style if one isn't supplied, otherwise uses style provided
	*/
	void 
	Style::Setup( const QString& style /*= ""*/ )
	{
		this->stylesheet.clear();
		if (style.isEmpty())
		{
			QFile file(":/Styles/stylesheet.qss");
			file.open(QIODevice::ReadOnly);
			this->stylesheet = QLatin1String(file.readAll());
			Q_ASSERT(this->IsValid());
		}
		else
		{
			QFile file(":/Styles/" + style + ".qss");
			file.open(QIODevice::ReadOnly);
			this->stylesheet = QLatin1String(file.readAll());
			Q_ASSERT(this->IsValid());
		}
	}

	//------------------------------------------------------------------------------
	/**
	*/
	bool 
	Style::IsValid() const
	{
		return !this->stylesheet.isEmpty();
	}
}