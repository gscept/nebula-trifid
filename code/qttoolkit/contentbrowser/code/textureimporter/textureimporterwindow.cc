//------------------------------------------------------------------------------
//  textureimporterwindow.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include <QFileInfo>
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>
#include "textureimporterwindow.h"
#include "progressreporter.h"
#include "io/ioserver.h"
#include "applauncher.h"
#include "graphics/graphicsinterface.h"
#include "qtaddons/remoteinterface/qtremoteclient.h"

using namespace ContentBrowser;
using namespace Util;
using namespace IO;
using namespace Graphics;
namespace TextureImporter
{

//------------------------------------------------------------------------------
/**
*/
TextureImporterWindow::TextureImporterWindow()
{
	// setup ui
	this->ui.setupUi(this);

	// setup signal
	connect(this->ui.importButton, SIGNAL(pressed()), this, SLOT(OnImport()));

	// setup attr table
	this->attrTable.Setup("src:assets");

	// hide load stuff
	this->ui.loadProgress->setVisible(false);
	this->ui.status->setVisible(false);
	this->resourceFolder = "assets";

	// connect signals and slots
	connect(this->ui.widthBox, SIGNAL(valueChanged(int)), this, SLOT(OnWidthChanged(int)));
	connect(this->ui.heightBox, SIGNAL(valueChanged(int)), this, SLOT(OnHeightChanged(int)));
	connect(this->ui.filterBox, SIGNAL(activated(int)), this, SLOT(OnFilterChanged(int)));
	connect(this->ui.qualityBox, SIGNAL(activated(int)), this, SLOT(OnQualityChanged(int)));
	connect(this->ui.formatBoxRgb, SIGNAL(activated(int)), this, SLOT(OnFormatRGBChanged(int)));
	connect(this->ui.formatBoxRgba, SIGNAL(activated(int)), this, SLOT(OnFormatRGBAChanged(int)));
	connect(this->ui.genMipMaps, SIGNAL(toggled(bool)), this, SLOT(OnToggleGenerateMipMaps(bool)));
	connect(this->ui.linearBox, SIGNAL(toggled(bool)), this, SLOT(OnToggleSRGB(bool)));

	// fix size
	this->setFixedSize(this->width(), this->height());

	// spawn import-to-nebula window
	this->dialogUi.setupUi(&this->dialog);
	this->dialog.setFixedSize(this->dialog.width(), this->dialog.height());

	// connect button slot
	connect(this->dialogUi.newCategory, SIGNAL(pressed()), this, SLOT(OnNewCategory()));
}

//------------------------------------------------------------------------------
/**
*/
TextureImporterWindow::~TextureImporterWindow()
{
	this->attrTable.Discard();
}

//------------------------------------------------------------------------------
/**
	First saves batch attributes, then runs exporter
*/
void 
TextureImporterWindow::OnImport()
{
	// get category
	String category = this->path.GetHostAndLocalPath().ExtractLastDirName();

	// get texture
	String tex = this->path.GetHostAndLocalPath().ExtractFileName();

	// get texture extension
	String ext = tex.GetFileExtension();

	// create entry
	String entry = category + "/" + tex;

	// format work file
	String workFile;
	workFile.Format("src:assets/%s", entry.AsCharPtr());
	URI workPath(workFile);

	// check if file is in work
	if (workPath != this->path)
	{
		// setup combo box
		this->SetupCategories(category.AsCharPtr());

		String texNoExt = tex;
		texNoExt.StripFileExtension();

		// setup name
		this->dialogUi.nameEdit->setText(texNoExt.AsCharPtr());

		// open dialog
		int result = this->dialog.exec();

		// if dialog was accepted
		if (result == QDialog::Accepted)
		{
			// reformat category, texture and entry
			category = this->dialogUi.categoryBox->currentText().toUtf8().constData();
			tex.Format("%s.%s", this->dialogUi.nameEdit->text().toUtf8().constData(), ext.AsCharPtr());
			entry = category + "/" + tex;

			// reformat work file
			workFile.Format("src:assets/%s/%s", 
				category.AsCharPtr(), 
				tex.AsCharPtr());

			// now check if this file already exists
			if (IoServer::Instance()->FileExists(workFile))
			{
				// ask if one wants to overwrite the file
				QMessageBox box;
				box.setText("The file: " + QString(tex.AsCharPtr()) + " already exists, do you want to overwrite it?");
				box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				box.setDefaultButton(QMessageBox::Yes);
				result = box.exec();

				if (result == QMessageBox::Yes)
				{
					// copy file to nebula
					IoServer::Instance()->DeleteFile(workFile);
					IoServer::Instance()->CopyFile(this->path, workFile);
				}
				else
				{
					// stop importing
					return;
				}
			}
			else
			{
				// copy file to nebula
				IoServer::Instance()->CopyFile(this->path, workFile);
			}
		}
		else
		{
			// exit importation
			return;
		}
	}

	// set int attribute table
	entry.StripFileExtension();
	this->attrTable.SetEntry(entry, this->currentAttrs);

	// save attribute table
	this->attrTable.Save(entry);

	// tell progress reporter to use our load progress
	ProgressReporter::Instance()->SetProgressBar(this->ui.loadProgress);
	ProgressReporter::Instance()->SetStatusLabel(this->ui.status);
	ProgressReporter::Instance()->BeginProgressReporting();

	// launch exporter to reexport resource
	ToolkitUtil::AppLauncher launcher;
#if __WIN32__
    #if _DEBUG
        launcher.SetExecutable("bin:texturebatcher3.debug.exe");
    #else
        launcher.SetExecutable("bin:texturebatcher3.exe");
    #endif
#else
        launcher.SetExecutable("bin:texturebatcher3");
#endif

	String arguments;
	arguments.Format("-dir %s -file %s -force", category.AsCharPtr(), tex.AsCharPtr());
	launcher.SetArguments(arguments);
	launcher.SetWorkingDirectory("tex:");
	launcher.SetNoConsoleWindow(true);

	String status;
	status.Format("Exporting: %s", (category + "/" + tex).AsCharPtr());

	// update progress
	ProgressReporter::Instance()->ReportProgress(50, status);
	QApplication::processEvents();

	// launch application
	if (launcher.LaunchWait())
	{
		// change extension
		tex.ChangeFileExtension("dds");

		// format resource
		String resource;
		resource.Format("tex:%s/%s", category.AsCharPtr(), tex.AsCharPtr());

		// then tell graphics to reload texture
		Ptr<ReloadResourceIfExists> reloadMessage = ReloadResourceIfExists::Create();
		reloadMessage->SetResourceName(resource);
		GraphicsInterface::Instance()->Send(reloadMessage.upcast<Messaging::Message>());

		// send modification message to remote end
		QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor")->Send(reloadMessage.upcast<Messaging::Message>());
	}

	// update progress
	ProgressReporter::Instance()->ReportProgress(100, "Export done!");
	ProgressReporter::Instance()->EndProgressReporting();

	// emit that importing is done
	emit ImportDone(category + "/" + tex);

	// close window
	this->close();
}

//------------------------------------------------------------------------------
/**
*/
void
TextureImporterWindow::SetUri(const URI& path)
{
	this->path = path;
}

//------------------------------------------------------------------------------
/**
*/
void 
TextureImporterWindow::Open()
{
	if (this->path.IsValid())
	{
		String category = this->path.AsString().ExtractLastDirName();
		String file = this->path.AsString().ExtractFileName();
		String extension = file.GetFileExtension();
		extension.ToUpper();
		file.StripFileExtension();

		// create string which will format our data
		QString data;

		// create pixmap
		QPixmap pixmap(path.GetHostAndLocalPath().AsCharPtr());
		QFileInfo qfile(path.GetHostAndLocalPath().AsCharPtr());
		this->ui.nameLabel->setText(path.GetTail().ExtractFileName().AsCharPtr());

		data.sprintf("%dpx", pixmap.width());
		this->ui.widthLabel->setText(data);
		data.sprintf("%dpx", pixmap.height());
		this->ui.heightLabel->setText(data);
		data.sprintf("%d bit", pixmap.depth());
		this->ui.depthLabel->setText(data);
		if (pixmap.hasAlphaChannel())
		{
			data.sprintf("RGBA");
		}
		else
		{
			data.sprintf("RGB");
		}
		this->ui.channelLabel->setText(data);
		
		this->ui.formatLabel->setText(extension.AsCharPtr());
		data.sprintf("%.2f KB", qfile.size()/1024.0f);
		this->ui.sizeLabel->setText(data);
		pixmap = pixmap.scaledToHeight(300);

		// scale pixmap
		this->ui.textureLabel->setPixmap(pixmap);
		this->ui.status->setText("Loading image...");

		String entry = category + "/" + file;
		if (this->attrTable.HasEntry(entry))
		{
			// get attributes from table
			this->currentAttrs = this->attrTable.GetEntry(entry);
		}
		else
		{
			// set our attrs to be default, then add to attribute table
			this->currentAttrs = this->attrTable.GetDefaultEntry();

			// set default as normal map if it contains the default name
			if (String::MatchPattern(file, "*norm*") ||
				String::MatchPattern(file, "*normal*") ||
				String::MatchPattern(file, "*bump*"))
			{
				this->currentAttrs.SetRGBAPixelFormat(ToolkitUtil::TextureAttrs::DXT5NM);
				this->currentAttrs.SetRGBPixelFormat(ToolkitUtil::TextureAttrs::DXT5NM);
			}
		}

		// setup ui
		this->ui.widthBox->setValue(this->currentAttrs.GetMaxWidth());
		this->ui.heightBox->setValue(this->currentAttrs.GetMaxHeight());
		this->ui.filterBox->setCurrentIndex((uint)this->currentAttrs.GetScaleFilter());
		this->ui.qualityBox->setCurrentIndex((uint)this->currentAttrs.GetQuality());
		this->ui.formatBoxRgb->setCurrentIndex((uint)this->currentAttrs.GetRGBPixelFormat());
		this->ui.formatBoxRgba->setCurrentIndex((uint)this->currentAttrs.GetRGBAPixelFormat());
		this->ui.genMipMaps->setChecked(this->currentAttrs.GetGenMipMaps());
		this->ui.linearBox->setChecked(this->currentAttrs.GetColorSpace() == ToolkitUtil::TextureAttrs::Linear);

		// remove file extension from file
		file.StripFileExtension();

		// update UI
		QApplication::processEvents();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
TextureImporterWindow::OnWidthChanged(int value)
{
	this->currentAttrs.SetMaxWidth(value);
}

//------------------------------------------------------------------------------
/**
*/
void
TextureImporterWindow::OnHeightChanged(int value)
{
	this->currentAttrs.SetMaxHeight(value);
}

//------------------------------------------------------------------------------
/**
*/
void
TextureImporterWindow::OnFilterChanged(int index)
{
	this->currentAttrs.SetMipMapFilter((ToolkitUtil::TextureAttrs::Filter)index);
}

//------------------------------------------------------------------------------
/**
*/
void
TextureImporterWindow::OnQualityChanged(int index)
{
	this->currentAttrs.SetQuality((ToolkitUtil::TextureAttrs::Quality)index);
}

//------------------------------------------------------------------------------
/**
*/
void
TextureImporterWindow::OnFormatRGBChanged(int index)
{
	this->currentAttrs.SetRGBPixelFormat((ToolkitUtil::TextureAttrs::PixelFormat)index);
}

//------------------------------------------------------------------------------
/**
*/
void
TextureImporterWindow::OnFormatRGBAChanged(int index)
{
	this->currentAttrs.SetRGBAPixelFormat((ToolkitUtil::TextureAttrs::PixelFormat)index);
}

//------------------------------------------------------------------------------
/**
*/
void
TextureImporterWindow::OnToggleGenerateMipMaps(bool state)
{
	this->currentAttrs.SetGenMipMaps(state);
}

//------------------------------------------------------------------------------
/**
*/
void
TextureImporterWindow::OnToggleSRGB(bool state)
{
	if (state)	this->currentAttrs.SetColorSpace(ToolkitUtil::TextureAttrs::Linear);
	else		this->currentAttrs.SetColorSpace(ToolkitUtil::TextureAttrs::sRGB);
}

} // namespace TextureImporter