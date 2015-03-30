#pragma once
#include <QString>

namespace Importer
{
class Clip
{

public:

	enum InfinityType
	{
		Constant,
		Cycle
	};

	/// constructor
	Clip();
	/// destructor
	~Clip();

	/// sets the name
	void SetName(const QString& name);
	/// gets the name
	const QString& GetName() const;
	/// sets the start time
	void SetStart(uint start);
	/// gets the start value
	const uint GetStart() const;
	/// sets the end time
	void SetEnd(uint end);
	/// gets the end time
	const uint GetEnd() const;
	/// sets the pre infinity type
	void SetPreInfinity(InfinityType infinityType);
	/// gets the pre infinity type
	const InfinityType GetPreInfinity() const;
	/// sets the post infinity type
	void SetPostInfinity(InfinityType infinityType);
	/// gets the post infinity type
	const InfinityType GetPostInfinity() const;
private:
	QString name;
	uint start;
	uint end;
	InfinityType preInfinity;
	InfinityType postInfinity;

};
}