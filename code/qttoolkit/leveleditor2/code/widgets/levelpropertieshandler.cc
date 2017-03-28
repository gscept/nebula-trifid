//------------------------------------------------------------------------------
//  levelpropertieshandler.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "leveleditor2app.h"
#include "leveleditor2entitymanager.h"
#include <QListWidget>
#include <QPushButton>
#include <QBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPlastiqueStyle>
#include <QDir>
#include "levelpropertieshandler.h"

using namespace Ui;
using namespace BaseGameFeature;

namespace LevelEditor2
{
//------------------------------------------------------------------------------
/**
*/
LevelPropertiesHandler::LevelPropertiesHandler(QWidget *parent /*= 0*/) :QDialog(parent)
{
	this->ui.setupUi(this);
	// note: palette is inherited

	connect(this->ui.startupLevel,SIGNAL(clicked()),this,SLOT(OnStartupChanged()));
}

//------------------------------------------------------------------------------
/**
*/
LevelPropertiesHandler::~LevelPropertiesHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
LevelPropertiesHandler::UpdateValues()
{
	this->ui.levelname->setText(Level::Instance()->GetName().AsCharPtr());	
	this->ui.startupLevel->setCheckState(Level::Instance()->GetStartLevel()?Qt::Checked:Qt::Unchecked);
}

//------------------------------------------------------------------------------
/**
*/
void
LevelPropertiesHandler::OnStartupChanged()
{
	switch(this->ui.startupLevel->checkState())
	{
		case Qt::Checked:
			Level::Instance()->SetStartLevel(true);
			break;
		case Qt::Unchecked:
			Level::Instance()->SetStartLevel(false);
			break;
		default:
			break;
	}

}

}

