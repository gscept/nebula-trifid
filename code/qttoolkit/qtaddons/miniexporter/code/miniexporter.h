#pragma once
//------------------------------------------------------------------------------
/**
    @class MiniExporterAddon::MiniExporter
    
    Implements an initializing exporter which exports shaders, system textures and system graphics.
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include <QDialog>

namespace Ui
{
	class MiniExporterDialog;
}
namespace MiniExporterAddon
{
class MiniExporter : public Core::RefCounted
{
	__DeclareClass(MiniExporter);
public:
	/// constructor
	MiniExporter();
	/// destructor
	virtual ~MiniExporter();

	/// runs exported
	void Run();

	/// sets the force flag
	void SetForce(bool b);

private:
	bool force;
	Ui::MiniExporterDialog* ui;
	QDialog dialog;
}; 


//------------------------------------------------------------------------------
/**
*/
inline void 
MiniExporter::SetForce( bool b )
{
	this->force = b;
}
} // namespace MiniExporterAddon
//------------------------------------------------------------------------------