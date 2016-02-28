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
#include "game/gameexporter.h"

using namespace Ui;
using namespace Util;
using namespace IO;
namespace LevelEditor2
{

typedef struct
{
	Util::String resource;
	Util::String name;
	bool cursor;
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
	
	if (IO::IoServer::Instance()->FileExists("root:data/tables/db/_ui_layouts.xml"))
	{        
		Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream("root:data/tables/db/_ui_layouts.xml");
		stream->SetAccessMode(IO::Stream::ReadAccess);
		Ptr<IO::XmlReader> reader = IO::XmlReader::Create();
		{
			reader->SetStream(stream);
			if (reader->Open())
			{
                String category = reader->GetCurrentNodeName();
                if(category != "_Ui_Layouts")
                {
                    n_warning("corrupt ui layouts file: %s\n", reader->GetStream()->GetURI().LocalPath().AsCharPtr());
                    return false;
                }
                
                if (reader->SetToFirstChild("Item")) do
                {                    
                   LayoutEntry entry;
                   entry.resource = reader->GetString("Id");
                   entry.name = reader->GetString("Name");
                   entry.autoload = reader->GetBool("AutoLoad");
				   entry.cursor = reader->GetOptBool("UICursor", false);
                   layouts.Add(entry.resource, entry);
                }
                while (reader->SetToNextChild("Item"));
                reader->Close();
            }
        }
    }

    if (IO::IoServer::Instance()->FileExists("root:data/tables/db/_ui_fonts.xml"))
    {        
        Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream("root:data/tables/db/_ui_fonts.xml");
        stream->SetAccessMode(IO::Stream::ReadAccess);
        Ptr<IO::XmlReader> reader = IO::XmlReader::Create();
        {
            reader->SetStream(stream);
            if (reader->Open())
            {
                String category = reader->GetCurrentNodeName();
                if(category != "_Ui_Fonts")
                {
                    n_warning("corrupt ui fonts file: %s\n", reader->GetStream()->GetURI().LocalPath().AsCharPtr());
                    return false;
                }

                if (reader->SetToFirstChild("Item")) do
                {                    
                    FontEntry entry;
                    entry.resource = reader->GetString("Id");
                    entry.family = reader->GetString("UIFontFamily");
                    entry.style = (UI::FontStyle)reader->GetInt("UIFontStyle");
                    entry.weight = (UI::FontWeight)reader->GetInt("UIFontWeight");
                    entry.autoload = reader->GetBool("AutoLoad");
                    fonts.Add(entry.resource, entry);                    
                }
                while (reader->SetToNextChild("Item"));
                reader->Close();
            }
        }
    }

    if (IO::IoServer::Instance()->FileExists("root:data/tables/db/_ui_scripts.xml"))
    {        
        Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream("root:data/tables/db/_ui_scripts.xml");
        stream->SetAccessMode(IO::Stream::ReadAccess);
        Ptr<IO::XmlReader> reader = IO::XmlReader::Create();
        {
            reader->SetStream(stream);
            if (reader->Open())
            {
                String category = reader->GetCurrentNodeName();
                if(category != "_Ui_Scripts")
                {
                    n_warning("corrupt ui scripts file: %s\n", reader->GetStream()->GetURI().LocalPath().AsCharPtr());
                    return false;
                }

                if (reader->SetToFirstChild("Item")) do
                {                    
                    ScriptEntry entry;
                    entry.resource = reader->GetString("Id");		
                    entry.autoload = reader->GetBool("AutoLoad");
                    scripts.Add(entry.resource, entry);            
                }
                while (reader->SetToNextChild("Item"));
                reader->Close();
            }
        }
    }	
	return true;
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

	// clear tables before adding stuff
	this->ui.layoutWidget->setRowCount(0);
	this->ui.fontWidget->setRowCount(0);
	this->ui.scriptWidget->setRowCount(0);

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
				bool cursor;
				if (layouts.Contains(resource))
				{
					name = layouts[resource].name;
					autoload = layouts[resource].autoload;
					cursor = layouts[resource].cursor;
				}
				else
				{
					name = resource.ExtractFileName();
					name.StripFileExtension();
					autoload = false;
					cursor = false;
				}
				QTableWidgetItem * iname = new QTableWidgetItem(name.AsCharPtr());
				this->ui.layoutWidget->setItem(fileIndex, 1, iname);
				iname->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

				QCheckBox * cursorcheck = new QCheckBox;
				cursorcheck->setChecked(cursor);
				this->ui.layoutWidget->setCellWidget(fileIndex, 2, cursorcheck);

				QCheckBox * autoloadcheck = new QCheckBox;
				autoloadcheck->setChecked(autoload);
				this->ui.layoutWidget->setCellWidget(fileIndex, 3, autoloadcheck);
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
	this->ui.layoutWidget->clearContents();
	this->ui.layoutWidget->setRowCount(0);
	
	this->ui.fontWidget->clearContents();
	this->ui.fontWidget->setRowCount(0);
	
	this->ui.scriptWidget->clearContents();
	this->ui.scriptWidget->setRowCount(0);
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

    {
	    Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream("root:data/tables/db/_ui_layouts.xml");
	    stream->SetAccessMode(IO::Stream::WriteAccess);
	    Ptr<IO::XmlWriter> writer = IO::XmlWriter::Create();
	    writer->SetStream(stream);
	
	    writer->Open();

	    writer->BeginNode("_Ui_Layouts");
        writer->SetBool("IsVirtualCategory", true);
	    rows = ui.layoutWidget->rowCount();
	    for (int i = 0; i < rows; i++)
	    {
		    Util::String fname(ui.layoutWidget->item(i, 0)->text().toLatin1().constData());
		    Util::String name(ui.layoutWidget->item(i, 1)->text().toLatin1().constData());
			QCheckBox * cursorbox = (QCheckBox*)ui.layoutWidget->cellWidget(i, 2);
		    QCheckBox * autobox = (QCheckBox*)ui.layoutWidget->cellWidget(i, 3);
		    writer->BeginNode("Item");
		    writer->SetString("Id", fname);
		    writer->SetString("Name", name);
			writer->SetBool("UICursor", cursorbox->isChecked());
		    writer->SetBool("AutoLoad", autobox->isChecked());
		    writer->EndNode();
	    }
	    writer->EndNode();
        writer->Close();
    }

    {
        Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream("root:data/tables/db/_ui_fonts.xml");
        stream->SetAccessMode(IO::Stream::WriteAccess);
        Ptr<IO::XmlWriter> writer = IO::XmlWriter::Create();
        writer->SetStream(stream);

        writer->Open();

        writer->BeginNode("_Ui_Fonts");
        writer->SetBool("IsVirtualCategory", true);
	    rows = ui.fontWidget->rowCount();
	    for (int i = 0; i < rows; i++)
	    {
		    Util::String fname(ui.fontWidget->item(i, 0)->text().toLatin1().constData());
		    Util::String family(ui.fontWidget->item(i, 1)->text().toLatin1().constData());
		    QComboBox * style = (QComboBox *)ui.fontWidget->cellWidget(i, 2);
		    QComboBox * weight = (QComboBox *)ui.fontWidget->cellWidget(i, 3);
		    QCheckBox * box = (QCheckBox*)ui.fontWidget->cellWidget(i, 4);
		    writer->BeginNode("Item");
		    writer->SetString("Id", fname);
		    writer->SetString("UIFontFamily", family);
		    writer->SetInt("UIFontStyle", style->currentIndex());
		    writer->SetInt("UIFontWeight", weight->currentIndex());
		    writer->SetBool("AutoLoad", box->isChecked());
		    writer->EndNode();
	    }
	    writer->EndNode();
        writer->Close();
    }

    {
        Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream("root:data/tables/db/_ui_scripts.xml");
        stream->SetAccessMode(IO::Stream::WriteAccess);
        Ptr<IO::XmlWriter> writer = IO::XmlWriter::Create();
        writer->SetStream(stream);

        writer->Open();

        writer->BeginNode("_Ui_Scripts");
        writer->SetBool("IsVirtualCategory", true);

	    rows = ui.scriptWidget->rowCount();
	    for (int i = 0; i < rows; i++)
	    {
		    Util::String fname(ui.scriptWidget->item(i, 0)->text().toLatin1().constData());		
		    QCheckBox * box = (QCheckBox*)ui.scriptWidget->cellWidget(i, 1);
		    writer->BeginNode("Item");
		    writer->SetString("Id", fname);		
		    writer->SetBool("AutoLoad", box->isChecked());
		    writer->EndNode();
	    }
	    writer->EndNode();
	    writer->Close();
    }
	this->CloseDialog();	
    ToolkitUtil::Logger logger;
    Ptr<ToolkitUtil::GameExporter> exporter = ToolkitUtil::GameExporter::Create();	
    exporter->SetLogger(&logger);
    exporter->Open();
    exporter->ExportTables();
    exporter->Close();	
}

}