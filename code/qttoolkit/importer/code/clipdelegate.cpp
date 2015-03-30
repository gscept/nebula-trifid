#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include "clipdelegate.h"
#include "clipcontroller.h"
namespace Importer
{
//------------------------------------------------------------------------------
/**
*/
ClipDelegate::ClipDelegate( ClipController* controller )
{
	this->controller = controller;
}

//------------------------------------------------------------------------------
/**
*/
ClipDelegate::~ClipDelegate()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
QWidget* 
ClipDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	this->controller->SetCurrentClip(index);
	switch(index.column())
	{
	case 0:
		{
			return new QLineEdit(parent);
		}
	case 1:
		{
			QSpinBox* box = new QSpinBox(parent);
			box->setMinimum(0);
			box->setMaximum(1000000);
			box->setButtonSymbols(QAbstractSpinBox::NoButtons);
			return box;
		}			
	case 2:
		{
			QSpinBox* box = new QSpinBox(parent);
			box->setMinimum(0);
			box->setMaximum(1000000);
			box->setButtonSymbols(QAbstractSpinBox::NoButtons);
			return box;
		}	
	case 3:
		{
			QComboBox* box = new QComboBox(parent);
			box->addItem("Constant");
			box->addItem("Cycle");
			return box;
		}
	case 4:
		{
			QComboBox* box = new QComboBox(parent);
			box->addItem("Constant");
			box->addItem("Cycle");
			return box;
		}
	}
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const
{
	switch(index.column())
	{
	case 0:
		{
			QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
			lineEdit->setText(this->currentClip->GetName());
			break;
		}
	case 1:
		{
			QSpinBox* box = qobject_cast<QSpinBox*>(editor);
			box->setValue(this->currentClip->GetStart());
			break;
		}
	case 2:
		{
			QSpinBox* box = qobject_cast<QSpinBox*>(editor);
			box->setValue(this->currentClip->GetEnd());
			break;
		}
	case 3:
		{
			QComboBox* box = qobject_cast<QComboBox*>(editor);
			box->setCurrentIndex(this->currentClip->GetPreInfinity());
			break;
		}
	case 4:
		{
			QComboBox* box = qobject_cast<QComboBox*>(editor);
			box->setCurrentIndex(this->currentClip->GetPostInfinity());
			break;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipDelegate::setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
	switch(index.column())
	{
	case 0:
		{
			QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
			this->currentClip->SetName(lineEdit->text());
			model->setData(index, this->currentClip->GetName());
			this->controller->EmitChanged();
			break;
		}
	case 1:
		{
			QSpinBox* box = qobject_cast<QSpinBox*>(editor);
			this->currentClip->SetStart(box->value());
			model->setData(index, this->currentClip->GetStart());
			this->controller->EmitChanged();
			break;
		}
	case 2:
		{
			QSpinBox* box = qobject_cast<QSpinBox*>(editor);
			this->currentClip->SetEnd(box->value());
			model->setData(index, this->currentClip->GetEnd());
			this->controller->EmitChanged();
			break;
		}
	case 3:
		{
			QComboBox* box = qobject_cast<QComboBox*>(editor);
			this->currentClip->SetPreInfinity((Clip::InfinityType)box->currentIndex());
			model->setData(index, this->currentClip->GetPreInfinity() == Clip::Constant ? "Constant" : "Cycle");
			break;
		}
	case 4:
		{
			QComboBox* box = qobject_cast<QComboBox*>(editor);
			this->currentClip->SetPostInfinity((Clip::InfinityType)box->currentIndex());
			model->setData(index, this->currentClip->GetPostInfinity() == Clip::Constant ? "Constant" : "Cycle");
			break;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipDelegate::SetCurrentClip( Clip* clip )
{
	this->currentClip = clip;
}

//------------------------------------------------------------------------------
/**
*/
QSize 
ClipDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	return QSize(100, 32);
}

//------------------------------------------------------------------------------
/**
*/
void 
ClipDelegate::updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	editor->setGeometry(option.rect);
}

}