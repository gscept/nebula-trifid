//------------------------------------------------------------------------------
//  resourcestringcontroller.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resourcestringcontroller.h"
#include "game/entity.h"
#include "db/valuetable.h"
#include "ui_resourcestringcontroller.h"
#include "basegamefeature/basegameprotocol.h"
#include "idldocument/idlattribute.h"
#include "io/assignregistry.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include "io/ioserver.h"
#include "qtextdocument.h"
#include "io/textreader.h"
#include "scripteditor.h"

using namespace Util;
using namespace Attr;
using namespace IO;

namespace QtAttributeControllerAddon
{

//------------------------------------------------------------------------------
/**
*/
ResourceStringController::ResourceStringController(QWidget* _parent, const Ptr<Game::Entity>& _entity, const Ptr<Tools::IDLAttribute>& _attr):
	BaseAttributeController(_parent),
	attr(_attr),
	editable(false),
	fullName(false)
{
	this->entity = _entity;	
	this->attributeId = AttrId(_attr->GetName());
	this->type = Util::Variant::String;

	// setup ui
	this->ui = new Ui::ResourceStringController();
	this->ui->setupUi(this);

	QStyle* style = QApplication::style();

	this->ui->browseButton->setIcon(style->standardIcon(QStyle::SP_DialogOpenButton));
	this->ui->editButton->setIcon(style->standardIcon(QStyle::SP_FileDialogDetailedView));

	Ptr<BaseGameFeature::GetAttribute> msg = BaseGameFeature::GetAttribute::Create();
	msg->SetAttributeId(this->attributeId);
	_entity->SendSync(msg.cast<Messaging::Message>());

	this->resourceType = this->attr->GetAttributes()["resourceBase"];
	this->fileSuffix = this->attr->GetAttributes()["fileSuffix"];
	if(this->attr->GetAttributes().Contains("edit"))
	{
		this->editable = this->attr->GetAttributes()["edit"].AsBool();
	}
	if (this->attr->HasAttribute("fullName"))
	{
		this->fullName = this->attr->GetAttribute("fullName").AsBool();
	}
	this->ui->editButton->setVisible(this->editable);
	this->ui->editButton->setEnabled(this->editable);
	this->ui->lineEdit->setReadOnly(!this->editable);
	this->ui->lineEdit->setEnabled(this->editable);

	if (this->fullName)
	{
		Util::String full = msg->GetAttr().GetString();
		full.StripAssignPrefix();
		full.StripFileExtension();
		this->ui->lineEdit->setText(full.AsCharPtr());
	}
	else
	{
		this->ui->lineEdit->setText(msg->GetAttr().GetString().AsCharPtr());
	}	

	bool connected = false;
	connected = connect(this->ui->browseButton, SIGNAL(clicked()), this, SLOT(OnBrowse()));
	n_assert(connected);
	connected = connect(this->ui->editButton, SIGNAL(clicked()), this, SLOT(OnEdit()));
	n_assert(connected);
	connected = connect(this->ui->lineEdit, SIGNAL(editingFinished()), this, SLOT(OnLineEdit()));
	n_assert(connected);
}

//------------------------------------------------------------------------------
/**
*/
ResourceStringController::~ResourceStringController()
{
	this->entity = 0;
}

//------------------------------------------------------------------------------
/**
*/
Util::Variant 
ResourceStringController::GetValue() const
{
	Util::String value = this->ui->lineEdit->text().toUtf8().constData();
	if (this->fullName)
	{
		return Util::Variant(this->resourceType + ":" + value + "." + this->fileSuffix);
	}
	else
	{
		return Util::Variant(value);
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
ResourceStringController::Lock()
{
	this->ui->editButton->setEnabled(false);
	this->ui->browseButton->setEnabled(false);
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceStringController::OnBrowse()
{
	n_assert(IO::AssignRegistry::Instance()->HasAssign(this->resourceType));

	Util::String current = this->ui->lineEdit->text().toUtf8().constData();
	IO::URI uri(this->resourceType + ":");
	Util::String path = uri.LocalPath() ;
	Util::String newpath = QFileDialog::getOpenFileName(this,"Select Resource", (path+ "/" + current).AsCharPtr(),("*." + this->fileSuffix).AsCharPtr()).toUtf8().constData();

	if(newpath.IsEmpty())
	{
		return;
	}
	if(newpath.FindStringIndex(path) != 0)
	{
		// not inside assign folder anymore
		QMessageBox::warning(this,"Wrong folder","The file you selected is outside the resource folder tree");
		return;
	}

	Util::String subpath = newpath.ExtractToEnd(path.Length()+1);
	subpath.StripFileExtension();
	this->ui->lineEdit->setText(subpath.AsCharPtr());
	BaseAttributeController::OnValueChanged();	
}

//------------------------------------------------------------------------------
/** 
*/
void
ResourceStringController::OnEdit()
{
	if(this->resourceType == "scr")
	{
		Util::String current = this->ui->lineEdit->text().toUtf8().constData();
		Util::String newpath;
		Util::String txt;

		if(current.IsEmpty())
		{
			IO::URI uri(this->resourceType + ":");
			Util::String path = uri.LocalPath() ;
			newpath = QFileDialog::getSaveFileName(this, "Select Resource", path.AsCharPtr(),("*." + this->fileSuffix).AsCharPtr()).toUtf8().constData();
			if (newpath.IsEmpty())
			{
				return;
			}
			if (newpath.FindStringIndex(path) != 0)
			{
				// not inside assign folder anymore
				QMessageBox::warning(this, "Wrong folder", "The file you selected is outside the resource folder tree");
				return;
			}			
			Util::String subpath = newpath.ExtractToEnd(path.Length()+1);
			if (subpath.FindStringIndex("." + this->fileSuffix) != InvalidIndex)
			{
				subpath.StripFileExtension();
			}			
			this->ui->lineEdit->setText(subpath.AsCharPtr());
			current = subpath;
			BaseAttributeController::OnValueChanged();
		}

		IO::URI uri(this->resourceType + ":" + current + "." + this->fileSuffix);
        ScriptEditor::Instance()->EditFile(uri, current);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceStringController::OnLineEdit()
{
	BaseAttributeController::OnValueChanged();
}

} // namespace QtAttributeControllerAddon