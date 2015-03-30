#pragma once
#include <QList>
#include "outputstream.h"

namespace QtTools
{
	class OutputManager
	{
	public:

		/// returns singleton instance
		static OutputManager* Instance();

		/// adds an output stream
		void AddOutputStream(OutputStream* stream);
		/// writes message to all streams
		void Write(const QString& string);
		/// writes warning to all streams
		void WriteWarning(const QString& string);
		/// writes error to all streams
		void WriteError(const QString& string);
		/// writes message to specific stream
		void WriteSpecific(const QString& string, OutputStream* stream);
		/// writes warning to specific streams
		void WriteSpecificWarning(const QString& string, OutputStream* stream);
		/// writes error to specific streams
		void WriteSpecificError(const QString& string, OutputStream* stream);

		/// blocks output from being written
		void StartBlock();
		/// reenables outputs
		void StopBlock();

		/// clears all streams
		void Clear();
		/// clears specific stream
		void ClearSpecific(OutputStream* stream);

	private:
		/// constructor
		OutputManager();
		/// destructor
		~OutputManager();

		static OutputManager* instance;
		bool isBlocking;

		QList<OutputStream*> streams;
	};
}