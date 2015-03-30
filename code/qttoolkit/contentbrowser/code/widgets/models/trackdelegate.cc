//------------------------------------------------------------------------------
//  trackdelegate.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "trackdelegate.h"
#include "trackcontroller.h"
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QMouseEvent>

namespace Widgets
{

//------------------------------------------------------------------------------
/**
*/
TrackDelegate::TrackDelegate(TrackController* controller)
{
	n_assert(controller);
	this->controller = controller;
}

//------------------------------------------------------------------------------
/**
*/
TrackDelegate::~TrackDelegate()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
QWidget* 
TrackDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	switch (index.column())
	{
	case 1:
		{
			QDoubleSpinBox* box = new QDoubleSpinBox(parent);
			box->setMinimum(0);
			box->setMaximum(100);
			box->setSingleStep(0.01f);
			box->setButtonSymbols(QAbstractSpinBox::NoButtons);
			return box;
		}
	}
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool 
TrackDelegate::editorEvent( QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index )
{
	switch (index.column())
	{
	case 0:
		{
			if (event->type() == QEvent::MouseButtonDblClick)
			{
				QMouseEvent* ev = (QMouseEvent*)event;
				if (ev->button() == Qt::LeftButton)
				{
                    emit this->controller->ClipRemoved(index.row());
                    return true;
				}
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
TrackDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const
{
	switch (index.column())
	{
	case 1:
		{
			QDoubleSpinBox* box = qobject_cast<QDoubleSpinBox*>(editor);
			box->setValue(this->controller->weights[index.row()]);
			break;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
TrackDelegate::setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
	switch (index.column())
	{
	case 1:
		{
			QDoubleSpinBox* box = qobject_cast<QDoubleSpinBox*>(editor);
			this->controller->weights[index.row()] = box->value();
			model->setData(index, box->value());
			emit this->controller->WeightChanged(index.row());
			break;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
QSize 
TrackDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	return QSize(30, 15);
}

} // namespace Widgets