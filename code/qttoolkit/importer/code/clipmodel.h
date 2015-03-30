#pragma once
#include <QStandardItemModel>

namespace Importer
{
class ClipModel : public QStandardItemModel
{
public:
	/// constructor
	ClipModel(void);
	/// destructor
	~ClipModel(void);
};
}