#pragma once
//------------------------------------------------------------------------------
/**
	@class Widgets::MaterialHandler
	
	Handles the UI which modifies a single surface material.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/basehandler.h"
namespace Widgets
{
class MaterialHandler : public BaseHandler
{
	Q_OBJECT
	__DeclareClass(MaterialHandler);
public:
	/// constructor
	MaterialHandler();
	/// destructor
	virtual ~MaterialHandler();
};
} // namespace Widgets