//------------------------------------------------------------------------------
//  editorcontroller.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QPainter>

#include "ui_scripteditor.h"
#include "scripteditor.h"
#include "io/ioserver.h"
#include "io/stream.h"
#include "io/textreader.h"
#include "io/textwriter.h"

using namespace Ui;
using namespace Util;
using namespace IO;

namespace QtAttributeControllerAddon
{

ScriptEditor* ScriptEditor::singleton = 0;
//------------------------------------------------------------------------------
/**
*/
ScriptEditor::ScriptEditor(QWidget* parent) : 
    QMainWindow(parent)
{
    n_assert(singleton == 0);
    this->ui = new Ui::ScriptEditorWindow;
	this->ui->setupUi(this);			
    connect(this->ui->actionNew, SIGNAL(triggered()), this, SLOT(New()));
	connect(this->ui->actionSave, SIGNAL(triggered()), this, SLOT(Save()));
    connect(this->ui->actionOpen, SIGNAL(triggered()), this, SLOT(Open()));

    // generate text which gets shown if no script is open
    QPixmap pix(800, 600);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    QPen pen;
    pen.setColor(QColor(64, 64, 64, 0));
    painter.setPen(QPen(Qt::darkGray));
    QFont font;
    font.setFamily("Segoe UI");
    font.setPointSize(28);
    painter.setFont(font);
    painter.drawText(QRect(0, 0, 800, 600), Qt::AlignCenter, "No files. Many alone. Much empty. Such sad.");
    
    // create label which contains the message
    this->backgroundLabel = new QLabel;
    this->backgroundLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->backgroundLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    this->ui->centralwidget->layout()->addWidget(this->backgroundLabel);
    this->backgroundLabel->setPixmap(pix);

    this->ui->fileTabWidget->setVisible(false);   
    
    connect(this->ui->fileTabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(Close(int)));
    singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
ScriptEditor::~ScriptEditor()
{
    n_assert(singleton != 0);
    delete this->ui;
	singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptEditor::EditFile(const IO::URI& file, const Util::String& title)
{
    n_assert(file.IsValid());
    IndexT index = this->files.FindIndex(file);
    if (index == InvalidIndex)
    {
        Util::String path = file.LocalPath();

        // open file with read/write access, meaning the file will be created if it doesn't yet exist
        Ptr<Stream> stream = IoServer::Instance()->CreateStream(path);
        stream->SetAccessMode(Stream::ReadWriteAccess);
        stream->Open();

        Ptr<TextReader> txstream = TextReader::Create();
        txstream->SetStream(stream);
        txstream->Open();		

        // create UI
        QGridLayout* layout = new QGridLayout;
		layout->setContentsMargins(QMargins(1, 1, 1, 1));
        QLuaTextEdit* textEdit = new QLuaTextEdit;
        QFrame* tabFrame = new QFrame;
        layout->addWidget(textEdit);
		Util::String scripttxt = txstream->ReadAll();
		bool forseSave = false;
		if (scripttxt.Length() == 0)
		{
			forseSave = true;
			// empty script file, add some defaults to it
			scripttxt = "-- this is the default script with some predefined callbacks\n\
-- uncomment the callbacks you need\n\n\
-- run when the entity is created\n\
--function oninit(eid)\n\
--end\n\n\
-- run every frame\n\
--function onframe(eid)\n\
--end\n\n\
-- run upon a collision event\n\
--function oncollision(eid, othereid, point, normal)\n\
--end\n\n\
-- input event has happened\n\
-- code is a string describing the key or mouse button, up is true for KeyUp and false for KeyDown\n\
--function oninput(eid, code, up)\n\n\
--end\n\n\
-- if your model has animevents (and you activated feedback for them)\n\
--function onanimevent(eid,event)\n\
--end\n\n\
--end\n\n\n\n-- For ScriptingTriggerProperties some additional callbacks are available:\n\n\
-- runs when entering trigger\n\
--function onenter(eid,othereid)\n\
--end\n\n\
-- runs every frame inside object\n\
--function oninside(eid, othereid) \n\
--end\n\n\
-- runs when exiting trigger\n\
--function onexit(eid, othereid)\n\
--end\n\n\
-- if the entity has the interactproperty these additional callbacks are available:\n\n\
-- the mouse went over the gameentity\n\
--function onmouseenter(eid)\n\
--end\n\n\
-- the mouse left the gameentity\n\
--function onmouseleave(eid)\n\
--end\n\n\
-- the mouse was clicked/released while over the game entity\n\
--function onmousebutton(eid,button,down)\n\
--end";

		}
		textEdit->setPlainText(scripttxt.AsCharPtr());		
        tabFrame->setLayout(layout);

        // close stream
        txstream->Close();
        stream->Close();

        // connect modification signal
        connect(textEdit, SIGNAL(modificationChanged(bool)), this, SLOT(Modified(bool)));

        // add text fields to array
        this->textFields.Append(textEdit);
        this->titles.Append(title);
        this->files.Append(file);

        // add tab with given title
        this->ui->fileTabWidget->addTab(tabFrame, title.AsCharPtr());      
        this->ui->fileTabWidget->setVisible(true);
        this->backgroundLabel->setVisible(false);
		if (forseSave)
		{
			this->Save();
		}
    }
    else
    {
        this->ui->fileTabWidget->setCurrentIndex(index);
        this->ui->fileTabWidget->setVisible(true);
        this->backgroundLabel->setVisible(false);
    }

    // show window and make sure it is infront
    this->show();
    this->raise();
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptEditor::Modified(bool mod)
{
	if(mod)
	{
        const String& title = this->titles[this->ui->fileTabWidget->currentIndex()];
        this->ui->fileTabWidget->setTabText(this->ui->fileTabWidget->currentIndex(), (title + " *").AsCharPtr());
	}
	else
	{
        const String& title = this->titles[this->ui->fileTabWidget->currentIndex()];
		this->ui->fileTabWidget->setTabText(this->ui->fileTabWidget->currentIndex(), title.AsCharPtr());
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ScriptEditor::Open()
{
    IO::URI scriptDir("scr:");
    QString fullPath = QFileDialog::getOpenFileName(NULL, "Open script file", scriptDir.LocalPath().AsCharPtr(), tr("Script Files (*.lua)"));
    if (!fullPath.isEmpty())
    {
        Util::String path(fullPath.toUtf8().constData());
        this->EditFile(IO::URI(path), path.ExtractLastDirName() + "/" + path.ExtractFileName());
    }
}


//------------------------------------------------------------------------------
/**
*/
void 
ScriptEditor::New()
{
    IO::URI scriptDir("scr:");
    QString fullPath = QFileDialog::getSaveFileName(NULL, "Create new script", scriptDir.LocalPath().AsCharPtr(), tr("Script Files (*.lua)"));
    if (!fullPath.isEmpty())
    {
        Util::String path(fullPath.toUtf8().constData());
        this->EditFile(IO::URI(path), path.ExtractLastDirName() + "/" + path.ExtractFileName());        
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptEditor::Save()
{
    if (this->ui->fileTabWidget->currentIndex() != -1)
    {
        QLuaTextEdit* edit = this->textFields[this->ui->fileTabWidget->currentIndex()];
        const IO::URI& path = this->files[this->ui->fileTabWidget->currentIndex()];
        Util::String txt = edit->toPlainText().toAscii().constData();

        // unflag modified
        this->Modified(false);

        Ptr<Stream> stream = IoServer::Instance()->CreateStream(path);	
        stream->SetAccessMode(Stream::WriteAccess);

        Ptr<TextWriter> writer = TextWriter::Create();
        writer->SetStream(stream);
        writer->Open();
        writer->WriteString(txt);
        writer->Close();	
    }    
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptEditor::Close(int index)
{
    // remove tab
    this->ui->fileTabWidget->removeTab(index);

    // cleanup data
    this->files.EraseIndex(index);
    this->titles.EraseIndex(index);
    this->textFields.EraseIndex(index);

    if (files.Size() == 0)
    {
        this->ui->fileTabWidget->setVisible(false);
        this->backgroundLabel->setVisible(true);
    }
}
}