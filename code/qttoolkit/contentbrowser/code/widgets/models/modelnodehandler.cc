//------------------------------------------------------------------------------
//  modelnodeitemhandler.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
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
	modelHandler(0),
    managedSurface(0),
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

    State state = this->modelHandler->GetAttributes()->GetState(this->nodePath);

    // reduce material to category and file
    String surface = state.material;
    surface.StripAssignPrefix();
    surface.StripFileExtension();
    String category = surface.ExtractDirName();
    category.SubstituteString("/", "");
    String file = surface.ExtractFileName();
    String res = String::Sprintf("%s/%s", category.AsCharPtr(), file.AsCharPtr());

    // update UI
	this->ui->surfacePath->setText(res.AsCharPtr());
	//this->ui->surfaceName->setText(res.AsCharPtr());
    this->ui->nodeName->setText(this->nodeName.AsCharPtr());

    // update thumbnail
    this->UpdateSurfaceThumbnail();

    // set surface
    this->SetSurface(surface);

    connect(this->ui->surfaceButton, SIGNAL(pressed()), this, SLOT(Browse()));
	connect(this->ui->surfacePath, SIGNAL(textEdited(const QString&)), this, SLOT(OnSurfaceTextEdited(const QString&)));
	connect(this->ui->editSurfaceButton, SIGNAL(pressed()), this, SLOT(EditSurface()));
	connect(&this->thumbnailWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(OnThumbnailFileChanged()));
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::Discard()
{
    disconnect(this->ui->surfaceButton, SIGNAL(pressed()), this, SLOT(Browse()));
	disconnect(this->ui->editSurfaceButton, SIGNAL(pressed()), this, SLOT(EditSurface()));
	disconnect(&this->thumbnailWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(OnThumbnailFileChanged()));

	// discard surface in preview state
	if (this->managedSurface.isvalid())
	{
		this->surfaceInstance = 0;
		Resources::ResourceManager::Instance()->DiscardManagedResource(this->managedSurface.upcast<Resources::ManagedResource>());
		this->managedSurface = 0;
	}

	delete this->ui;
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::Refresh()
{
	Ptr<ModelAttributes> attrs = this->modelHandler->GetAttributes();
	State state = attrs->GetState(this->nodePath);

	// remove previous surface
	this->surfaceInstance = 0;

	// save previous managed surface
	Ptr<Materials::ManagedSurface> prevManagedSurface = this->managedSurface;

	// update model
	Ptr<ModelEntity> model = ContentBrowserApp::Instance()->GetPreviewState()->GetModel();
	Ptr<Models::StateNodeInstance> node = RenderUtil::NodeLookupUtil::LookupStateNodeInstance(model, this->nodePath);
	this->managedSurface = Resources::ResourceManager::Instance()->CreateManagedResource(Surface::RTTI, state.material, NULL, true).downcast<Materials::ManagedSurface>();
    this->surfaceInstance = this->managedSurface->GetSurface()->CreateInstance();
	node->SetSurfaceInstance(this->surfaceInstance);

	// once last instance is discarded by node, unload previous managed surface
	if (prevManagedSurface.isvalid())
	{
		Resources::ResourceManager::Instance()->DiscardManagedResource(prevManagedSurface.upcast<Resources::ManagedResource>());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::SetSurface(const Util::String& sur)
{
	// save previous managed surface
	Ptr<Materials::ManagedSurface> prevManagedSurface = this->managedSurface;

    // update model
    Ptr<ModelEntity> model = ContentBrowserApp::Instance()->GetPreviewState()->GetModel();
    Ptr<Models::StateNodeInstance> node = RenderUtil::NodeLookupUtil::LookupStateNodeInstance(model, this->nodePath);
    this->managedSurface = Resources::ResourceManager::Instance()->CreateManagedResource(Surface::RTTI, String::Sprintf("sur:%s.sur", sur.AsCharPtr()), NULL, true).downcast<Materials::ManagedSurface>();
    this->surfaceInstance = this->managedSurface->GetSurface()->CreateInstance();
    node->SetSurfaceInstance(this->surfaceInstance);

	// once last instance is discarded by node, unload previous managed surface
	if (prevManagedSurface.isvalid())
	{
		Resources::ResourceManager::Instance()->DiscardManagedResource(prevManagedSurface.upcast<Resources::ManagedResource>());
	}

	Ptr<ModelAttributes> attrs = this->modelHandler->GetAttributes();
	State state = attrs->GetState(this->nodePath);
    state.material = this->managedSurface->GetSurface()->GetResourceId().AsString();
	state.material.StripFileExtension();
	attrs->SetState(this->nodePath, state);
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
	button->setStyleSheet("border: 2px solid red;");
    int res = ResourceBrowser::AssetBrowser::Instance()->Execute("Set surface", ResourceBrowser::AssetBrowser::Surfaces);
	button->setStyleSheet("");
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
		this->ui->surfacePath->setText(res.AsCharPtr());
        //this->ui->surfaceName->setText(res.AsCharPtr());
        Ptr<ModelAttributes> attrs = this->modelHandler->GetAttributes();
        State state = attrs->GetState(this->nodePath);
		state.material = String::Sprintf("sur:%s", res.AsCharPtr());
        attrs->SetState(this->nodePath, state);

        // update thumbnail
        this->UpdateSurfaceThumbnail();

        // update surface
        this->SetSurface(surface);
		this->modelHandler->OnModelModified();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::EditSurface()
{
	Ptr<ModelAttributes> attrs = this->modelHandler->GetAttributes();
	State state = attrs->GetState(this->nodePath);
	String surface = state.material;
	surface.ChangeFileExtension("sur");

	// open in content browser...
	ContentBrowserApp::Instance()->GetWindow()->OnSurfaceSelected(surface.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::OnThumbnailFileChanged()
{
	this->UpdateSurfaceThumbnail();
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::OnSurfaceTextEdited(const QString& path)
{
	// convert to string
	String surface = path.toUtf8().constData();

	// set text of item
	Ptr<ModelAttributes> attrs = this->modelHandler->GetAttributes();
	State state = attrs->GetState(this->nodePath);
	state.material = "sur:" + surface;
	attrs->SetState(this->nodePath, state);

	// update thumbnail
	this->UpdateSurfaceThumbnail();

	// update surface
	this->SetSurface(surface);
	this->modelHandler->OnModelModified();
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeHandler::UpdateSurfaceThumbnail()
{
    Ptr<ModelAttributes> attrs = this->modelHandler->GetAttributes();
    State state = attrs->GetState(this->nodePath);
    String surface = state.material;

    // create pixmap which will be used to set the icon of the browsing button
    QPixmap pixmap;
	surface.StripAssignPrefix();
	surface.StripFileExtension();
	surface = String::Sprintf("src:assets/%s_sur.thumb", surface.AsCharPtr());
    IO::URI texFile = surface;
    pixmap.load(texFile.LocalPath().AsCharPtr());
    pixmap = pixmap.scaled(QSize(48, 48), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	this->thumbnailWatcher.WatchFile(texFile.LocalPath());

	QPixmap res(48, 48);
	res.fill(Qt::transparent);
	QPainter painter(&res);
	painter.setBrush(QBrush(pixmap));
	painter.setPen(Qt::transparent);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.drawRoundedRect(0, 0, 48, 48, 2, 2);
	QIcon icon(res);
	this->ui->surfaceButton->setFixedSize(50, 50);
	this->ui->surfaceButton->setIcon(icon);
	this->ui->surfaceButton->setIconSize(QSize(48, 48));

	/*
	// set background color
    QPalette palette;
    palette.setBrush(this->ui->surfaceButton->backgroundRole(), QBrush(pixmap));
	
    this->ui->surfaceButton->setPalette(palette);
    this->ui->surfaceButton->setMask(pixmap.mask());
	*/
}

} // namespace Widgets