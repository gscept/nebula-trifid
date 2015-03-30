#include "stdneb.h"
#include "commandlineargs.h"
#include <QString>

namespace QtTools
{


	//------------------------------------------------------------------------------
	/**
	*/
	CommandLineArgs::CommandLineArgs()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	*/
	CommandLineArgs::CommandLineArgs( int argc, char* argv[] )
	{
		this->Setup(argc, argv);
	}

	//------------------------------------------------------------------------------
	/**
	*/
	CommandLineArgs::~CommandLineArgs()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	*/
	void 
	CommandLineArgs::Setup( int argc, char* argv[] )
	{
		// make sure we have pairs of two
		for (int i = 1; i < argc; i+=2)
		{
			QString key(argv[i]);
			QString value(argv[i+1]);

			this->arguments[key] = value;
		}
	}

	
	//------------------------------------------------------------------------------
	/**
	*/
	void 
	CommandLineArgs::AddArgument( const QString& key, const QString& value )
	{
		this->arguments[key] = value;
	}

	//------------------------------------------------------------------------------
	/**
	*/
	const QString
	CommandLineArgs::AsString() const
	{
		QString stringVal = "";
		for (int i = 0; i < this->arguments.size(); i++)
		{
			stringVal += this->arguments.keys()[i] + " " + this->arguments.values()[i] + " ";
		}
		return stringVal.trimmed();
	}

}