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
    managedMaterial(0),
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
ModelNodeHandler::Setup(const Util::String& resource)
{
	n_assert(resource.IsValid());

    State state = this->itemHandler->GetAttributes()->GetState(this->nodePath);

    // reduce material to category and file
    String surface = state.material;
    surface.StripAssignPrefix();
    surface.StripFileExtension();
    String category = surface.ExtractDirName();
    category.SubstituteString("/", "");
    String file = surface.ExtractFileName();
    String res = String::Sprintf("%s/%s", category.AsCharPtr(), file.AsCharPtr());

    // update UI
    this->ui->surfaceName->setText(state.material.AsCharPtr());
    this->ui->nodeName->setText(this->nodeName.AsCharPtr());

    // update thumbnail
    this->UpdateSurfaceThumbnail();

    // set surface
    this->SetSurface(surface);

    connect(this->ui->surfaceButton, SIGNAL(pressed()), this, SLOT(Browse()));
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::Discard()
{
    disconnect(this->ui->surfaceButton, SIGNAL(pressed()), this, SLOT(Browse()));
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
ModelNodeHandler::SetSurface(const Util::String& sur)
{
    if (this->managedMaterial.isvalid())
    {
        Resources::ResourceManager::Instance()->DiscardManagedResource(this->managedMaterial.upcast<Resources::ManagedResource>());
        this->managedMaterial = 0;
    }

    // update model
    Ptr<ModelEntity> model = ContentBrowserApp::Instance()->GetPreviewState()->GetModel();
    Ptr<Models::StateNodeInstance> node = RenderUtil::NodeLookupUtil::LookupStateNodeInstance(model, this->nodePath);
    this->managedMaterial = Resources::ResourceManager::Instance()->CreateManagedResource(SurfaceMaterial::RTTI, String::Sprintf("sur:%s.sur", sur.AsCharPtr()), NULL, true).downcast<Materials::ManagedSurfaceMaterial>();
    node->SetMaterial(this->managedMaterial->GetMaterial());
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

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::Browse()
{
    // get sender
    QObject* sender = this->sender();

    // must be a button
    QPushButton* button = static_cast<QPushButton*>(sender);

    // pick a surface
    int res = ResourceBrowser::AssetBrowser::Instance()->Execute("Set surface", ResourceBrowser::AssetBrowser::Surfaces);
    if (res == QDialog::Accepted)
    {
        // convert to nebula string
        String surface = ResourceBrowser::AssetBrowser::Instance()->GetSelectedTexture().toUtf8().constData();
        surface.StripAssignPrefix();
        surface.StripFileExtension();
        String category = surface.ExtractDirName();
        category.SubstituteString("/", "");
        String file = surface.ExtractFileName();
        String res = String::Sprintf("%s/%s", category.AsCharPtr(), file.AsCharPtr());

        // set text of item
        this->ui->surfaceName->setText(res.AsCharPtr());
        Ptr<ModelAttributes> attrs = this->itemHandler->GetAttributes();
        State state = attrs->GetState(this->nodePath);
        state.material = "sur:" + res;
        attrs->SetState(this->nodePath, state);

        // update thumbnail
        this->UpdateSurfaceThumbnail();

        // update surface
        this->SetSurface(surface);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::UpdateSurfaceThumbnail()
{
    Ptr<ModelAttributes> attrs = this->itemHandler->GetAttributes();
    State state = attrs->GetState(this->nodePath);
    String surface = state.material;

    // create pixmap which will be used to set the icon of the browsing button
    QPixmap pixmap;
    surface += "_thumb";
    surface.ChangeAssignPrefix("tex");
    surface.ChangeFileExtension("dds");
    IO::URI texFile = surface;
    pixmap.load(texFile.LocalPath().AsCharPtr());
    pixmap = pixmap.scaled(QSize(100, 100), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QPalette palette;
    palette.setBrush(this->ui->surfaceButton->backgroundRole(), QBrush(pixmap));
    this->ui->surfaceButton->setPalette(palette);
    this->ui->surfaceButton->setMask(pixmap.mask());
}

} // namespace Widgets