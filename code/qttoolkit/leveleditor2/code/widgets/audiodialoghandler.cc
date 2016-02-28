//------------------------------------------------------------------------------
//  audiodialoghandler.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "leveleditor2app.h"
#include "audiodialoghandler.h"
#include "faudio/audiodevice.h"
#include "game/gameexporter.h"

using namespace Ui;

namespace LevelEditor2
{

//------------------------------------------------------------------------------
/**
*/
AudioDialogHandler::AudioDialogHandler(QWidget* parent) :
    QDialog(parent)	
{
	this->ui.setupUi(this);
			
	connect(this->ui.buttonBox, SIGNAL(accepted()),this, SLOT(SaveAudioProperties()));  
	connect(this->ui.buttonBox, SIGNAL(rejected()), this, SLOT(CloseDialog()));
}


//------------------------------------------------------------------------------
/**
*/
AudioDialogHandler::~AudioDialogHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDialogHandler::SetupDialog()
{

	Util::Array<Util::String> banks = FAudio::AudioDevice::Instance()->FindBankFiles();

	Util::Dictionary<Util::String, bool> autoloader = FAudio::AudioDevice::ParseAutoload("root:data/tables/db/_audiobanks.xml");

	for (IndexT i = 0; i < banks.Size(); i++)
	{
		this->ui.tableWidget->insertRow(i);
		Util::String basename = banks[i];
		QTableWidgetItem * name = new QTableWidgetItem(basename.AsCharPtr());
		this->ui.tableWidget->setItem(i, 0, name);


		QCheckBox * loaded = new QCheckBox;
		loaded->setProperty("fname", basename.AsCharPtr());
		bool loadStatus = FAudio::AudioDevice::Instance()->BankLoaded(banks[i]);
		loaded->setChecked(loadStatus);
		this->ui.tableWidget->setCellWidget(i, 1, loaded);
		connect(loaded, SIGNAL(stateChanged(int)), this, SLOT(LoadBank(int)));


		QCheckBox * autoloaded = new QCheckBox;
		loadStatus = false;
		if (autoloader.Contains(banks[i]))
		{
			loadStatus = autoloader[banks[i]];
		}
		autoloaded->setChecked(loadStatus);
		this->ui.tableWidget->setCellWidget(i, 2, autoloaded);

		if (basename == "Master Bank" || basename == "Master Bank.strings")
		{
			loaded->setEnabled(false);
			autoloaded->setChecked(true);
			autoloaded->setEnabled(false);
		}
	}
	this->ui.tableWidget->resizeColumnsToContents();
}	



//------------------------------------------------------------------------------
/**
*/
void
AudioDialogHandler::CloseDialog()
{
	ui.tableWidget->clearContents();
	int rows = ui.tableWidget->rowCount();
	for (int i = 0; i < rows; i++)
	{
		ui.tableWidget->removeRow(0);
	}
	
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDialogHandler::LoadBank(int state)
{
	QCheckBox * box = (QCheckBox*)QObject::sender();
	QString name = box->property("fname").toString();
	Util::String bankname = name.toLatin1().constData();
	if (box->isChecked())
	{
		FAudio::AudioDevice::Instance()->LoadBank(bankname);
	}
	else
	{
		FAudio::AudioDevice::Instance()->UnloadBank(bankname);
	}
}





//------------------------------------------------------------------------------
/**
*/
void
AudioDialogHandler::SaveAudioProperties()
{
	Util::Dictionary<Util::String, bool> autoloads = FAudio::AudioDevice::ParseAutoload("root:data/tables/db/_audiobanks.xml");
		
	int rows = ui.tableWidget->rowCount();
	for (int i = 0; i < rows; i++)
	{
		QCheckBox * box = (QCheckBox*)ui.tableWidget->cellWidget(i, 2);
		Util::String filename(ui.tableWidget->item(i, 0)->text().toLatin1().constData());
		if (filename == "Master Bank" || filename == "Master Bank.strings")
		{
			continue;
		}
		if (autoloads.Contains(filename))
		{
			autoloads[filename] = box->isChecked();
		}
		else
		{
			autoloads.Add(filename, box->isChecked());
		}
	}

	Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream("root:data/tables/db/_audiobanks.xml");
	stream->SetAccessMode(IO::Stream::WriteAccess);
	Ptr<IO::XmlWriter> writer = IO::XmlWriter::Create();
	writer->SetStream(stream);
	
	writer->Open();

	writer->BeginNode("_AudioBanks");
    writer->SetBool("IsVirtualCategory", true);
	for (int b = 0; b < autoloads.Size(); b++)
	{
		writer->BeginNode("Item");
		writer->SetString("Id", autoloads.KeyAtIndex(b));
		writer->SetBool("AutoLoad", autoloads.ValueAtIndex(b));
		writer->EndNode();
	}
	writer->EndNode();
	writer->Close();
	this->CloseDialog();
    ToolkitUtil::Logger logger;
    Ptr<ToolkitUtil::GameExporter> exporter = ToolkitUtil::GameExporter::Create();	
    exporter->SetLogger(&logger);
    exporter->Open();
    exporter->ExportTables();
    exporter->Close();	
}

}