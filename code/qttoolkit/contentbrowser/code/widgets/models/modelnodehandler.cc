//------------------------------------------------------------------------------
//  modelnodeitemhandler.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include <QFileDialog>
#include <QLineEdit>
#include <QToolButton>
#include <QGroupBox>
#include <QBitmap>
#include <QColorDialog>
#include "modelnodehandler.h"
#include "modelnodeframe.h"
#include "materials/materialdatabase.h"
#include "io/ioserver.h"
#include "math/float4.h"
#include "io/uri.h"
#include "resources/managedtexture.h"
#include "ui_materialinfowidget.h"
#include "contentbrowserapp.h"
#include "graphics/modelentity.h"
#include "renderutil/nodelookuputil.h"
#include "resources/resourcemanager.h"


using namespace Materials;
using namespace ToolkitUtil;
using namespace Util;
using namespace ContentBrowser;
using namespace IO;
using namespace Math;
using namespace Graphics;

namespace Widgets
{
__ImplementClass(Widgets::ModelNodeHandler, 'MNIH', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ModelNodeHandler::ModelNodeHandler() :
	mainLayout(0),
	itemHandler(0),
	actionUpdateMode(true)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ModelNodeHandler::~ModelNodeHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::Setup(const Util::String& material, const Util::String& resource)
{
	n_assert(resource.IsValid());
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::Discard()
{

}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::HardRefresh(const Util::String& resource)
{
    // override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::SoftRefresh(const Util::String& resource)
{
    // override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeHandler::ClearFrame( QLayout* layout )
{
	if (layout)
	{
		QLayoutItem* item;
		QLayout* subLayout;
		QWidget* widget;
		while (item = layout->takeAt(0))
		{
			if (subLayout = item->layout()) this->ClearFrame(subLayout);
			else if (widget = item->widget()) { widget->hide(); delete widget; }
			else delete item;
		}
		if (layout != this->mainLayout->layout())
		{
			delete layout;
		}		
	}
}

} // namespace Widgets