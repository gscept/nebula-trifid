//------------------------------------------------------------------------------
//  textureitemhandler.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include <QPixmap>
#include <QLabel>
#include <QFileInfo>
#include "texturehandler.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "io/uri.h"
#include "textureitem.h"
#include "applauncher.h"
#include "qtaddons/remoteinterface/qtremoteclient.h"

using namespace Graphics;
using namespace IO;
using namespace Util;
using namespace ToolkitUtil;
namespace Widgets
{
__ImplementClass(Widgets::TextureHandler, 'TEIH', Widgets::BaseHandler);

//------------------------------------------------------------------------------
/**
*/
TextureHandler::TextureHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
TextureHandler::~TextureHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
TextureHandler::OnReload()
{
	// launch exporter to reexport resource
	AppLauncher launcher;
#if _DEBUG
	launcher.SetExecutable("bin:texturebatcher3.debug.exe");
#else
	launcher.SetExecutable("bin:texturebatcher3.exe");
#endif

	String exportTarget = this->texture;
	exportTarget.StripFileExtension();

	String path;
	path.Format("src:assets/%s/", this->category.AsCharPtr());

	// find texture in work
	Array<String> origTex = IoServer::Instance()->ListFiles(path, exportTarget + "*"); 

	// we should only be able to find one
	if (origTex.Size() == 1)
	{
		String arguments;
		arguments.Format("-dir %s -file %s -force", this->category.AsCharPtr(), origTex[0].AsCharPtr());
		launcher.SetArguments(arguments);
		launcher.SetWorkingDirectory("tex:");
		launcher.SetNoConsoleWindow(true);

		// finally launch exporter
		if (launcher.LaunchWait())
		{
			// format resource
			String resource;
			resource.Format("tex:%s/%s", this->category.AsCharPtr(), this->texture.AsCharPtr());

			// then tell graphics to reload texture
			Ptr<ReloadResourceIfExists> reloadMessage = ReloadResourceIfExists::Create();
			reloadMessage->SetResourceName(resource);
			GraphicsInterface::Instance()->Send(reloadMessage.upcast<Messaging::Message>());

			// send modification message to remote end
			QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor")->Send(reloadMessage.upcast<Messaging::Message>());

			// reload the texture in the UI
			this->LoadTexture();
		}
	}	
}

//------------------------------------------------------------------------------
/**
*/
bool
TextureHandler::LoadTexture()
{
	String file = this->texture;
	String dir = this->category;
	URI path = "tex:" + dir + "/" + file;

	// set texture name in ui
	this->ui->textureName->setText(file.AsCharPtr());

	bool res = true;
	QPixmap pixmap;
	
	if (!pixmap.load(path.GetHostAndLocalPath().AsCharPtr()))
	{
		pixmap = QPixmap(":/Nebula/nebulalogo.png");
		res = false;
	}

	// set labels
	this->ui->widthLabel->setText(QString::number(pixmap.width()) + "px");
	this->ui->heightLabel->setText(QString::number(pixmap.height()) + "px");
	this->ui->depthLabel->setText(QString::number(pixmap.depth()) + " bit");

	// scale image
	pixmap = pixmap.scaledToWidth(300);

	// set pixmap
	this->ui->texturePixmapLabel->setPixmap(pixmap);

	QString format;
	String extension = file.GetFileExtension();
	if (extension == "tga")
	{
		format = "TGA (Targa)";
	}
	else if (extension == "psd")
	{
		format = "PSD (Photoshop Image)";
	}
	else if (extension == "dds")
	{
		format = "DDS (DirectDraw Surface)";
	}
	else if (extension == "png")
	{
		format = "PNG (PNG Not Gif)";
	}

	// set format label
	this->ui->formatLabel->setText(format);

	// get file info
	QFileInfo info(path.GetHostAndLocalPath().AsCharPtr());

	// create and format string
	QString size;
	size.sprintf("%.2f KB", info.size() / 1024.0f);

	// format size
	this->ui->sizeLabel->setText(size);
	return res;
}


} // namespace Widgets