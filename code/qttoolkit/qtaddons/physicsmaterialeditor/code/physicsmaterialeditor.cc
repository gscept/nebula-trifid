//------------------------------------------------------------------------------
//  physicsmaterialeditor.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physicsmaterialeditor.h"
#include "ui_physicsmaterialeditorwindow.h"
#include "io/ioserver.h"
#include "QMessageBox"
#include "physics/materialtable.h"
#include "QSpinBox"
#include "io/stream.h"
#include "io/xmlwriter.h"

using namespace IO;
using namespace Util;
using namespace Physics;


namespace QtPhysics
{

//------------------------------------------------------------------------------
/**
*/
MaterialsEditor::MaterialsEditor() :	
	modified(false)
{
	// create ui
    this->ui = new Ui::PhysicsMaterialEditor;

	// setup ui
	this->ui->setupUi(this);
	this->setWindowFlags(Qt::Tool);

    connect(this->ui->saveButton, SIGNAL(pressed()), this, SLOT(OnSave()));
    connect(this->ui->cancelButton, SIGNAL(pressed()), this, SLOT(OnCancel()));
    connect(this->ui->removeButton, SIGNAL(pressed()), this, SLOT(OnRemove()));
    connect(this->ui->addButton, SIGNAL(pressed()), this, SLOT(OnAdd()));    
    connect(this->ui->interactionWidget, SIGNAL(cellChanged(int, int)), this, SLOT(OnInteractionChanged(int, int)));
    connect(this->ui->materialsWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnMaterialChanged(QTableWidgetItem*)));
    this->SetupUiFromTable();
}

//------------------------------------------------------------------------------
/**
*/
MaterialsEditor::~MaterialsEditor()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
MaterialsEditor::OnCancel()
{
    this->hide();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialsEditor::OnAdd()
{
    this->ui->materialsWidget->insertRow(this->materials.Size());
    MaterialTable::Material mat;
    mat.name = "dummy";
    mat.friction = 0.5f;
    mat.restitution = 0.5f;
    this->materials.Append(mat);
    Guid guid;
    guid.Generate();
    String intName = guid.AsString();
    this->realNames.Append(intName);
	String newName = "New Material";
	bool done = false;
	Array<String> othernames = this->names.ValuesAsArray();
	while (!done)
	{
		if (othernames.FindIndex(newName) != InvalidIndex)
		{
			newName.Append("X");
		}
		else
		{
			done = true;
		}
	}
    this->names.Add(intName, newName);
    Dictionary<String, String> newDic;
    this->interactions.Add(intName, newDic);
    this->SetupUiFromTable();
	this->SetModified();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialsEditor::OnRemove()
{
    QList<QTableWidgetItem *> items = this->ui->materialsWidget->selectedItems();
    if (items.size())
    {
        QTableWidgetItem * item = items[0];
        int row = item->row();
        this->materials.EraseIndex(row);
        Util::String name = this->realNames[row];
        this->names.Erase(name);        
        this->realNames.EraseIndex(row);
        this->interactions.Erase(name);
        for (int i = 0; i < this->interactions.Size(); i++)
        {
			if (this->interactions.ValueAtIndex(i).Contains(name))
			{
				this->interactions.ValueAtIndex(i).Erase(name);
			}            
        }
    }
    this->SetupUiFromTable();
	this->SetModified();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialsEditor::OnSave()
{
	Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream("root:data/tables/physicsmaterials.xml");
	stream->SetAccessMode(IO::Stream::WriteAccess);
	Ptr<IO::XmlWriter> writer = IO::XmlWriter::Create();
	writer->SetStream(stream);

	writer->Open();
	writer->BeginNode("PhysicsMaterials");
	writer->SetBool("IsVirtualCategory", true);
	for (int i = 0; i < this->materials.Size(); i++)
	{
		writer->BeginNode("Item");
		writer->SetString("Id", this->names[this->realNames[i]]);
		writer->SetFloat("Friction", this->materials[i].friction);
		writer->SetFloat("Restitution", this->materials[i].restitution);
		Array<String> inters;
		const Util::Dictionary<Util::String, Util::String> & inter = this->interactions[this->realNames[i]];
		for (int j = i; j < this->materials.Size(); j++)
		{
			if (inter.Contains(this->realNames[j]) && !inter[this->realNames[j]].IsEmpty())
			{
				String action = this->names[this->realNames[j]];				
				action += "=";
				action += inter[this->realNames[j]];
				inters.Append(action);
			}
		}
		writer->SetString("CollisionEvents", String::Concatenate(inters, " "));
		writer->EndNode();
	}

	writer->EndNode();
	writer->Close();
	Physics::MaterialTable::Setup();
	this->ResetModified();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialsEditor::OnFrictionChanged(double val)
{
    QDoubleSpinBox * box = (QDoubleSpinBox*)QObject::sender();
    int material = box->property("material").toInt();
    this->materials[material].friction = val;
    this->SetModified();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialsEditor::OnRestitutionChanged(double val)
{
    QDoubleSpinBox * box = (QDoubleSpinBox*)QObject::sender();
    int material = box->property("material").toInt();
    this->materials[material].restitution = val;
    this->SetModified();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialsEditor::OnInteractionChanged(int x, int y)
{
    QTableWidgetItem * item = this->ui->interactionWidget->item(x, y);
    Util::String event = item->text().toLatin1().constData();
    Util::String namex = this->realNames[x];
    Util::String namey = this->realNames[y];
    if (this->interactions[namex].Contains(namey))
    {
        this->interactions[namex][namey] = event;
    }
    else
    {
        this->interactions[namex].Add(namey, event);
    }
    this->SetModified();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialsEditor::OnMaterialChanged(QTableWidgetItem* item)
{
    if (item->column() == 0)
    {
		String newName = item->text().toLatin1().constData();
		int row = item->row();

		bool duplicate = false;
		// check for duplicates (exluding my the own name)
		// feels a bit brute force
		for (int i = 0; i < this->realNames.Size(); i++)
		{			
			if (i != row && this->names[this->realNames[i]] == newName)
			{
				duplicate = true;
			}
		}
		if (!duplicate)
		{
			this->names[this->realNames[row]] = item->text().toLatin1().constData();
			this->SetupUiFromTable();
			this->SetModified();
		}
		else
		{
			this->ui->materialsWidget->blockSignals(true);
			item->setText(this->names[this->realNames[row]].AsCharPtr());
			this->ui->materialsWidget->blockSignals(false);
		}
    } 
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialsEditor::SetupUiFromTable()
{
    this->ui->materialsWidget->blockSignals(true);
    this->ui->interactionWidget->blockSignals(true);

    this->ui->materialsWidget->clearContents();
    int curRows = this->ui->materialsWidget->rowCount();
    for (int i = curRows; i < this->realNames.Size(); i++)
    {
        this->ui->materialsWidget->insertRow(i);
    }
    for (int i = this->realNames.Size(); i < curRows; i++)
    {
        this->ui->materialsWidget->removeRow(i);
    }
    
    for (int i = 0; i < this->realNames.Size(); i++)
    {                        
        QTableWidgetItem * name = new QTableWidgetItem(this->names[this->realNames[i]].AsCharPtr());
        
        this->ui->materialsWidget->setItem(i, 0, name);
        
        QDoubleSpinBox * frictionWidget = new QDoubleSpinBox;
        frictionWidget->setValue(this->materials[i].friction);
        frictionWidget->setSingleStep(0.1);
        frictionWidget->setMinimum(0.0);        
                        
        this->ui->materialsWidget->setCellWidget(i, 1, frictionWidget);
        frictionWidget->setProperty("material", i);
        connect(frictionWidget, SIGNAL(valueChanged(double)), this, SLOT(OnFrictionChanged(double)));

        QDoubleSpinBox * restWidget = new QDoubleSpinBox;
        restWidget->setValue(this->materials[i].restitution);
		restWidget->setSingleStep(0.1);

        this->ui->materialsWidget->setCellWidget(i, 2, restWidget);
        restWidget->setProperty("material", i);
        connect(restWidget, SIGNAL(valueChanged(double)), this, SLOT(OnRestitutionChanged(double)));
    }
            
    this->ui->interactionWidget->clear();
	curRows = this->ui->interactionWidget->rowCount();
    for (int i = curRows; i < this->realNames.Size(); i++)
    {
        this->ui->interactionWidget->insertRow(i);
        this->ui->interactionWidget->insertColumn(i);
    }
    for (int i = this->realNames.Size(); i < curRows; i++)
    {
        this->ui->interactionWidget->removeRow(i);
        this->ui->interactionWidget->removeColumn(i);        
    }   
    
    for (int i = 0; i < this->realNames.Size(); i++)
    {        
        QTableWidgetItem * name = new QTableWidgetItem(this->names[this->realNames[i]].AsCharPtr());
        name->setFlags(name->flags() & (255 - Qt::ItemIsEditable));
        this->ui->interactionWidget->setHorizontalHeaderItem(i, name);

        name = new QTableWidgetItem(this->names[this->realNames[i]].AsCharPtr());
        name->setFlags(name->flags() & (255 - Qt::ItemIsEditable));
        this->ui->interactionWidget->setVerticalHeaderItem(i, name);
    }

    for (int i = 0; i < this->realNames.Size(); i++)
    {
        const Util::Dictionary<Util::String, Util::String>& row = this->interactions[this->realNames[i]];
        for (int j = 0; j < this->realNames.Size(); j++)
        {
            QTableWidgetItem * name;
            if (j >= i)
            {             
                QString entry;
                if (row.Contains(this->realNames[j]))
                {
                    entry = row[this->realNames[j]].AsCharPtr();
                }
                name = new QTableWidgetItem(entry);
            }
            else
            {
                name = new QTableWidgetItem();
                name->setFlags(name->flags() & (255 - Qt::ItemIsEnabled));
				name->setBackgroundColor(QColor(80, 80, 80));
            }                                    
            this->ui->interactionWidget->setItem(i, j, name);
        }
    }    
    this->ui->materialsWidget->blockSignals(false);
    this->ui->interactionWidget->blockSignals(false);
    this->ResetModified();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialsEditor::ResetModified()
{
	this->modified = false;	
	this->ui->saveButton->setEnabled(false);	
    this->setWindowTitle("Physics Materials Editor");
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialsEditor::SetModified()
{
	if (!this->modified)
	{
		this->setWindowTitle("Physics Materials Editor *");
		this->ui->saveButton->setEnabled(true);				
	}
	this->modified = true;	
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialsEditor::closeEvent(QCloseEvent * event)
{
	if (this->IsModified())
	{
		QMessageBox box;
		box.setText("The values have been modified");
		box.setInformativeText("Do you want to save your changes");
		box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
		box.setDefaultButton(QMessageBox::Save);
		int ret = box.exec();
        switch (ret)
        {
            case QMessageBox::Save:
            {
                this->OnSave();
            }
            break;
            default:
                break;
        }
		
	}
	QDialog::closeEvent(event);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialsEditor::showEvent(QShowEvent * event)
{
    this->materials = MaterialTable::GetMaterialTable();
    this->interactions = MaterialTable::GetInteractionTable();
    this->realNames.Clear();    
    this->names.Clear();

    for (int i = 0; i < this->materials.Size(); i++)
    {
        this->realNames.Append(this->materials[i].name.AsString());        
        this->names.Add(this->realNames[i], this->materials[i].name.AsString());
    }

    this->SetupUiFromTable();
}

} // namespace QtPhysics