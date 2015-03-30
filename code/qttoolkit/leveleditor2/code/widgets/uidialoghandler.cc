//------------------------------------------------------------------------------
//  uidialoghandler.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "leveleditor2app.h"
#include "uidialoghandler.h"
#include "faudio/audiodevice.h"
#include "ui/base/uiserverbase.h"
#include "qcombobox.h"
#include "qmessagebox.h"
#include "game/templateexporter.h"

using namespace Ui;
using namespace Util;
using namespace IO;
namespace LevelEditor2
{

typedef struct
{
	Util::String resource;
	Util::String name;
	bool autoload;
}LayoutEntry;


typedef struct
{
	Util::String resource;
	Util::String family;
	UI::FontStyle style;
	UI::FontWeight weight;
	bool autoload;
}FontEntry;

typedef struct
{
	Util::String resource;	
	bool autoload;
}ScriptEntry;

bool
ParseUIProperties(Util::Dictionary<Util::String, LayoutEntry> & layouts, Util::Dictionary<Util::String, FontEntry> & fonts, Util::Dictionary<Util::String, ScriptEntry> & scripts)
{
	
	if (IO::IoServer::Instance()->FileExists("root:data/tables/ui.xml"))
	{
		Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream("root:data/tables/ui.xml");
		stream->SetAccessMode(IO::Stream::ReadAccess);
		Ptr<IO::XmlReader> reader = IO::XmlReader::Create();
		{
			reader->SetStream(stream);
			if (reader->Open())
			{
				if (reader->HasNode("/Layouts"))
				{
					reader->SetToNode("/Layouts");
					if(reader->SetToFirstChild())
					{
						do
						{
							LayoutEntry entry;
							entry.resource = reader->GetString("file");
							entry.name = reader->GetString("name");
							entry.autoload = reader->GetOptBool("autoload", false);
							layouts.Add(entry.resource,entry);
							
						} while (reader->SetToNextChild());
					}
				}
				if (reader->HasNode("/Fonts"))
				{
					reader->SetToNode("/Fonts");
					if (reader->SetToFirstChild())
					{
						do
						{
							FontEntry entry;
							entry.resource = reader->GetString("file");
							entry.family = reader->GetString("family");
							entry.style = (UI::FontStyle)reader->GetInt("style");
							entry.weight = (UI::FontWeight)reader->GetInt("weight");
							entry.autoload = reader->GetOptBool("autoload", false);
							fonts.Add(entry.resource, entry);

						} while (reader->SetToNextChild());
					}
				}
				if (reader->HasNode("/Scripts"))
				{
					reader->SetToNode("/Scripts");
					if (reader->SetToFirstChild())
					{
						do
						{
							ScriptEntry entry;
							entry.resource = reader->GetString("file");		
							entry.autoload = reader->GetOptBool("autoload", false);
							scripts.Add(entry.resource, entry);

						} while (reader->SetToNextChild());
					}
				}
				reader->Close();
			}
		}
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
/**
*/
UIDialogHandler::UIDialogHandler(QWidget* parent) :
    QDialog(parent)	
{
	this->ui.setupUi(this);
			
	connect(this->ui.buttonBox, SIGNAL(accepted()),this, SLOT(SaveUIProperties()));  
	connect(this->ui.buttonBox, SIGNAL(rejected()), this, SLOT(CloseDialog()));
}


//------------------------------------------------------------------------------
/**
*/
UIDialogHandler::~UIDialogHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
UIDialogHandler::SetupDialog()
{	
	Util::Dictionary<Util::String, LayoutEntry>  layouts;
	Util::Dictionary < Util::String, FontEntry > fonts;
	Util::Dictionary < Util::String, ScriptEntry > scripts;
	ParseUIProperties(layouts, fonts, scripts);

	Util::Array<Util::String> directories = IO::IoServer::Instance()->ListDirectories(IO::URI("gui:"), "*");
	
	IndexT dirIndex;
	for (dirIndex = 0; dirIndex < directories.Size(); dirIndex++)
	{
		String directory = directories[dirIndex];
		{			
			Array<String> files = IoServer::Instance()->ListFiles(IO::URI("gui:" + directory), "*.rml");
			IndexT fileIndex;
			for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
			{
				String file = files[fileIndex];
				String resource = "gui:" + directory + "/" + file;
				this->ui.layoutWidget->insertRow(fileIndex);
				QTableWidgetItem * fname = new QTableWidgetItem(resource.AsCharPtr());
				fname->setFlags(Qt::NoItemFlags);
				this->ui.layoutWidget->setItem(fileIndex, 0, fname);
				String name;
				bool autoload;
				if (layouts.Contains(resource))
				{
					name = layouts[resource].name;
					autoload = layouts[resource].autoload;
				}
				else
				{
					name = resource.ExtractFileName();
					name.StripFileExtension();
					autoload = false;
				}
				QTableWidgetItem * iname = new QTableWidgetItem(name.AsCharPtr());
				this->ui.layoutWidget->setItem(fileIndex, 1, iname);
				iname->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

				QCheckBox * autoloadcheck = new QCheckBox;
				autoloadcheck->setChecked(autoload);
				this->ui.layoutWidget->setCellWidget(fileIndex, 2, autoloadcheck);
			}
		}
		{
			Array<String> files = IoServer::Instance()->ListFiles(IO::URI("gui:" + directory), "*.otf");
			files.AppendArray(IoServer::Instance()->ListFiles(IO::URI("gui:" + directory), "*.ttf"));
			IndexT fileIndex;
			for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
			{
				String file = files[fileIndex];
				String resource = "gui:" + directory + "/" + file;
				this->ui.fontWidget->insertRow(fileIndex);
				QTableWidgetItem * fname = new QTableWidgetItem(resource.AsCharPtr());
				fname->setFlags(Qt::NoItemFlags);
				this->ui.fontWidget->setItem(fileIndex, 0, fname);
				
				FontEntry entry;
				if (fonts.Contains(resource))
				{
					entry = fonts[resource];					
				}
				else
				{
					entry.weight = UI::NormalWeight;
					entry.style = UI::NormalStyle;
					entry.autoload = false;					
				}

				QTableWidgetItem * family = new QTableWidgetItem(entry.family.AsCharPtr());
				this->ui.fontWidget->setItem(fileIndex, 1, family);
				family->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

				QComboBox * styleBox = new QComboBox();
				styleBox->insertItem(0, "Normal");
				styleBox->insertItem(1, "Italic");
				styleBox->setCurrentIndex((int)entry.style);
				this->ui.fontWidget->setCellWidget(fileIndex, 2, styleBox);

				QComboBox * weightBox = new QComboBox();
				weightBox->insertItem(0, "Normal");
				weightBox->insertItem(1, "Bold");
				weightBox->setCurrentIndex((int)entry.weight);
				this->ui.fontWidget->setCellWidget(fileIndex, 3, weightBox);


				QCheckBox * autoloadcheck = new QCheckBox;
				autoloadcheck->setChecked(entry.autoload);
				this->ui.fontWidget->setCellWidget(fileIndex, 4, autoloadcheck);
			}
		}
		{
			Array<String> files = IoServer::Instance()->ListFiles(IO::URI("gui:" + directory), "*.lua");
			IndexT fileIndex;
			for (fileIndex = 0; fileIndex < files.Size(); fileIndex++)
			{
				String file = files[fileIndex];
				String resource = "gui:" + directory + "/" + file;
				this->ui.scriptWidget->insertRow(fileIndex);
				QTableWidgetItem * fname = new QTableWidgetItem(resource.AsCharPtr());
				fname->setFlags(Qt::NoItemFlags);
				this->ui.scriptWidget->setItem(fileIndex, 0, fname);
				String name;
				bool autoload;
				if (scripts.Contains(resource))
				{
					name = scripts[resource].resource;
					autoload = scripts[resource].autoload;
				}
				else
				{
					name = resource.ExtractFileName();
					name.StripFileExtension();
					autoload = false;
				}

				QCheckBox * autoloadcheck = new QCheckBox;
				autoloadcheck->setChecked(autoload);
				this->ui.scriptWidget->setCellWidget(fileIndex, 1, autoloadcheck);
			}
		}
		
	}
	this->ui.layoutWidget->resizeColumnsToContents();
	this->ui.fontWidget->resizeColumnsToContents();
	this->ui.scriptWidget->resizeColumnsToContents();
}	



//------------------------------------------------------------------------------
/**
*/
void
UIDialogHandler::CloseDialog()
{
	ui.layoutWidget->clearContents();
	int rows = ui.layoutWidget->rowCount();
	for (int i = 0; i < rows; i++)
	{
		ui.layoutWidget->removeRow(0);
	}
	ui.fontWidget->clearContents();
	rows = ui.fontWidget->rowCount();
	for (int i = 0; i < rows; i++)
	{
		ui.fontWidget->removeRow(0);
	}
	rows = ui.scriptWidget->rowCount();
	for (int i = 0; i < rows; i++)
	{
		ui.scriptWidget->removeRow(0);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
UIDialogHandler::SaveUIProperties()
{
	Dictionary<String, bool> nameCheck;
	// sanity check font and layout names
	int rows = ui.layoutWidget->rowCount();
	for (int i = 0; i < rows; i++)
	{
		Util::String name(ui.layoutWidget->item(i, 1)->text().toLatin1().constData());
		QCheckBox * box = (QCheckBox*)ui.layoutWidget->cellWidget(i, 2);
		if (box->isChecked())
		{
			if (nameCheck.Contains(name))
			{
				QMessageBox::critical(LevelEditor2App::Instance()->GetWindow(), "Layout name error", "All auto-loaded layouts must have unique names");
				this->show();
				this->raise();
				return;
			}
			else
			{
				nameCheck.Add(name, true);
			}
		}				
	}

	
	Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream("root:data/tables/ui.xml");
	stream->SetAccessMode(IO::Stream::WriteAccess);
	Ptr<IO::XmlWriter> writer = IO::XmlWriter::Create();
	writer->SetStream(stream);
	
	writer->Open();

	writer->BeginNode("Layouts");
	rows = ui.layoutWidget->rowCount();
	for (int i = 0; i < rows; i++)
	{
		Util::String fname(ui.layoutWidget->item(i, 0)->text().toLatin1().constData());
		Util::String name(ui.layoutWidget->item(i, 1)->text().toLatin1().constData());
		QCheckBox * box = (QCheckBox*)ui.layoutWidget->cellWidget(i, 2);
		writer->BeginNode("Layout");
		writer->SetString("file", fname);
		writer->SetString("name", name);
		writer->SetBool("autoload", box->isChecked());
		writer->EndNode();
	}
	writer->EndNode();

	writer->BeginNode("Fonts");
	rows = ui.fontWidget->rowCount();
	for (int i = 0; i < rows; i++)
	{
		Util::String fname(ui.fontWidget->item(i, 0)->text().toLatin1().constData());
		Util::String family(ui.fontWidget->item(i, 1)->text().toLatin1().constData());
		QComboBox * style = (QComboBox *)ui.fontWidget->cellWidget(i, 2);
		QComboBox * weight = (QComboBox *)ui.fontWidget->cellWidget(i, 3);
		QCheckBox * box = (QCheckBox*)ui.fontWidget->cellWidget(i, 4);
		writer->BeginNode("Font");
		writer->SetString("file", fname);
		writer->SetString("family", family);
		writer->SetInt("style", style->currentIndex());
		writer->SetInt("weight", weight->currentIndex());
		writer->SetBool("autoload", box->isChecked());
		writer->EndNode();
	}
	writer->EndNode();

	writer->BeginNode("Scripts");
	rows = ui.scriptWidget->rowCount();
	for (int i = 0; i < rows; i++)
	{
		Util::String fname(ui.scriptWidget->item(i, 0)->text().toLatin1().constData());		
		QCheckBox * box = (QCheckBox*)ui.scriptWidget->cellWidget(i, 1);
		writer->BeginNode("Script");
		writer->SetString("file", fname);		
		writer->SetBool("autoload", box->isChecked());
		writer->EndNode();
	}
	writer->EndNode();

	writer->Close();
	this->CloseDialog();	

	/// export ui tables right away
	ToolkitUtil::Logger logger;
	Ptr<ToolkitUtil::TemplateExporter> exporter = ToolkitUtil::TemplateExporter::Create();	
	exporter->SetDbFactory(Db::Sqlite3Factory::Instance());
	exporter->SetLogger(&logger);
	exporter->Open();
	exporter->ExportUiProperties();
	exporter->Close();
}

}