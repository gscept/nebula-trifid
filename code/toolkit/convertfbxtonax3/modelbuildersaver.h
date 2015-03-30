#pragma once

#include "io/uri.h"
#include "modelbuilder.h"
#include "toolkitutil/platform.h"
#include "io/stream.h"
#include "system/byteorder.h"

//------------------------------------------------------------------------------
namespace ToolkitUtil
{
	class ModelBuilderSaver
	{
	public:
		/// save n3 file
		static bool SaveN3(const IO::URI& uri, ModelBuilder& modelBuilder, Platform::Code platform);
	private:

	};



} // namespace ToolkitUtil