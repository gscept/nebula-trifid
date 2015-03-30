#pragma once
#include <QString>

namespace QtTools
{
	class Style
	{
	public:
		/// constructor
		Style();
		/// destructor
		~Style();

		/// overrides default style sheet
		void OverrideStyleSheet(const QString& overrideSheet);
		/// returns style as string
		const QString& GetStyle() const;

		/// returns true if style is valid
		bool IsValid() const;
	private:
		/// sets up style
		void Setup(const QString& style = "");

		QString stylesheet;
	};
}