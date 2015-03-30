#pragma once
#include <QString>
#include <QMap>

namespace QtTools
{
	class CommandLineArgs
	{
	public:
		/// constructor
		CommandLineArgs();
		/// constructs from argc and argv
		CommandLineArgs(int argc, char* argv[]);
		/// destructor
		~CommandLineArgs();

		/// sets up the command line args from C++ args
		void Setup(int argc, char* argv[]);
		/// adds an argument
		void AddArgument(const QString& key, const QString& value);

		/// gets a command line argument by identifier
		const QString GetArgument(const QString& key) const;
		/// returns true if argument exists
		const bool HasArgument(const QString& key) const;

		/// gets command line args as string
		const QString AsString() const;
	private:
		QMap<QString, QString> arguments;
	};

	//------------------------------------------------------------------------------
	/**
	*/
	inline const QString
	CommandLineArgs::GetArgument( const QString& key ) const
	{
		Q_ASSERT(this->arguments.contains(key));
		return this->arguments[key];
	}

	//------------------------------------------------------------------------------
	/**
	*/
	inline const bool 
	CommandLineArgs::HasArgument( const QString& key ) const
	{
		return this->arguments.contains(key);
	}
}