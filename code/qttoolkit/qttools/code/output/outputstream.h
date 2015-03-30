#pragma once
#include <QTextStream>
#include <QTextEdit>
#include <QPlainTextEdit>

namespace QtTools
{
	class OutputStream
	{
	public:
		/// constructor
		OutputStream();
		/// destructor
		~OutputStream();

		/// sets up from text stream
		void Setup(QTextStream* stream);
		/// sets up from a QTextEdit
		void Setup(QTextEdit* textEdit);
		/// sets up from a QPlainTextEdit
		void Setup(QPlainTextEdit* textEdit);

		/// writes a string to the stream/text edit
		void Write(const QString& string);
		/// writes a string to the stream/text edit formatted as a warning
		void WriteWarning(const QString& string);
		/// writes a string to the stream/text edit formatted as an error
		void WriteError(const QString& string);

		/// clears stream
		void Clear();
	private:
		QTextStream* stream;
		QTextEdit* edit;
		QPlainTextEdit* plainEdit;
	};
}