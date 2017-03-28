//------------------------------------------------------------------------------
// variablerowdelegate.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "variablerowdelegate.h"
#include "QLineEdit"
#include "QComboBox"
#include "materialwindow.h"

namespace Shady
{

//------------------------------------------------------------------------------
/**
*/
VariableRowDelegate::VariableRowDelegate()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VariableRowDelegate::~VariableRowDelegate()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VariableRowDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	MaterialWindow::MaterialVariableState& state = this->window->GetCurrentState().variables[index.row()];

	switch (index.column())
	{
	case 0:
	{
		QLineEdit* edit = (QLineEdit*)editor;
		state.name = edit->text().toUtf8().constData();
		model->setData(index, state.name.AsCharPtr());
		break;
	}
	case 1:
	{
		QComboBox* edit = (QComboBox*)editor;
		Util::Variant::Type type = Util::Variant::StringToType(edit->currentText().toUtf8().constData());
		if (type != state.type)
		{
			state.type = type;

			QModelIndex defValIndex = model->index(index.row(), 2);
			QModelIndex minValIndex = model->index(index.row(), 3);
			QModelIndex maxValIndex = model->index(index.row(), 4);
			QVariant def = model->data(defValIndex);
			QVariant min = model->data(minValIndex);
			QVariant max = model->data(maxValIndex);
			Util::String defaultVal = def.toString().toUtf8().constData();
			Util::String minVal = min.toString().toUtf8().constData();
			Util::String maxVal = max.toString().toUtf8().constData();
			this->window->SetVariableItemEnabled(minValIndex, true);
			this->window->SetVariableItemEnabled(maxValIndex, true);

			switch (type)
			{
			case Util::Variant::Float:
				state.defaultVal = 0.0f;
				state.minVal = 0.0f;
				state.maxVal = 1.0f;
				break;
			case Util::Variant::Float2:
				state.defaultVal = Math::float2(0.0f);
				state.minVal = Math::float2(0.0f);
				state.maxVal = Math::float2(1.0f);
				break;
			case Util::Variant::Float4:
				state.defaultVal = Math::float4(0.0f);
				state.minVal = Math::float4(0.0f);
				state.maxVal = Math::float4(1.0f);
				break;
			case Util::Variant::Int:
				state.defaultVal = 0;
				state.minVal = 0;
				state.maxVal = 1;
				break;
			case Util::Variant::Bool:
				state.defaultVal = false;
				state.minVal = false;
				state.maxVal = false;
				this->window->SetVariableItemEnabled(minValIndex, false);
				this->window->SetVariableItemEnabled(maxValIndex, false);
				break;
			case Util::Variant::String:
				state.defaultVal = "tex:system/white.dds";
				state.minVal = "";
				state.maxVal = "";
				this->window->SetVariableItemEnabled(minValIndex, false);
				this->window->SetVariableItemEnabled(maxValIndex, false);
				break;
			}
			
			// setup model
			model->setData(index, Util::Variant::TypeToString(state.type).AsCharPtr());
			model->setData(defValIndex, state.defaultVal.ToString().AsCharPtr());
			model->setData(minValIndex, state.minVal.ToString().AsCharPtr());
			model->setData(maxValIndex, state.maxVal.ToString().AsCharPtr());
		}
		
		break;
	}
	case 2:
	{
		QLineEdit* edit = (QLineEdit*)editor;
		state.defaultVal = Util::Variant::FromString(edit->text().toUtf8().constData());
		model->setData(index, state.defaultVal.ToString().AsCharPtr());
		break;
	}
	case 3:
	{
		QLineEdit* edit = (QLineEdit*)editor;
		state.minVal = Util::Variant::FromString(edit->text().toUtf8().constData());
		model->setData(index, state.minVal.ToString().AsCharPtr());
		break;
	}
	case 4:
	{
		QLineEdit* edit = (QLineEdit*)editor;
		state.maxVal = Util::Variant::FromString(edit->text().toUtf8().constData());
		model->setData(index, state.maxVal.ToString().AsCharPtr());
		break;
	}
	case 5:
	{
		QLineEdit* edit = (QLineEdit*)editor;
		state.desc = edit->text().toUtf8().constData();
		model->setData(index, state.desc.AsCharPtr());
		break;
	}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VariableRowDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	MaterialWindow::MaterialVariableState& state = this->window->GetCurrentState().variables[index.row()];
	QRegExpValidator* validator = new QRegExpValidator;
	const QString floatNumber = "([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?[fF]?)";
	switch (state.type)
	{
	case Util::Variant::Float:
		validator->setRegExp(QRegExp("^" + floatNumber + "$"));
		break;
	case Util::Variant::Float2:
		validator->setRegExp(QRegExp("^" + floatNumber + "[,]" + floatNumber + "$"));
		break;
	case Util::Variant::Float4:
		validator->setRegExp(QRegExp("^" + floatNumber + "[,]" + floatNumber + "[,]" + floatNumber + "[,]" + floatNumber + "$"));
		break;
	case Util::Variant::Int:
		validator->setRegExp(QRegExp("^([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?){1}$"));
		break;
	case Util::Variant::Bool:
		validator->setRegExp(QRegExp("^(true|false)$"));
		break;
	case Util::Variant::String:
		validator->setRegExp(QRegExp("^([A-Za-z]:/)$"));
		break;
	}
	switch (index.column())
	{
	case 0:
	{
		QLineEdit* edit = (QLineEdit*)editor;
		edit->setText(state.name.AsCharPtr());
		break;
	}
	case 1:
	{
		QComboBox* edit = (QComboBox*)editor;
		edit->addItem("float");
		edit->addItem("float2");
		edit->addItem("float4");
		edit->addItem("int");
		edit->addItem("bool");
		edit->addItem("string");
		edit->setCurrentIndex(edit->findText(Util::Variant::TypeToString(state.type).AsCharPtr()));
		break;
	}
	case 2:
	{
		QLineEdit* edit = (QLineEdit*)editor;
		edit->setValidator(validator);
		edit->setText(state.defaultVal.ToString().AsCharPtr());
		break;
	}
	case 3:
	{
		QLineEdit* edit = (QLineEdit*)editor;
		edit->setValidator(validator);
		edit->setText(state.minVal.ToString().AsCharPtr());
		break;
	}
	case 4:
	{
		QLineEdit* edit = (QLineEdit*)editor;
		edit->setValidator(validator);
		edit->setText(state.maxVal.ToString().AsCharPtr());
		break;
	}
	case 5:
	{
		QLineEdit* edit = (QLineEdit*)editor;
		edit->setText(state.desc.AsCharPtr());
		break;
	}
	}
}

//------------------------------------------------------------------------------
/**
*/
QWidget*
VariableRowDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	switch (index.column())
	{
	case 0:	return new QLineEdit(parent);
	case 1: return new QComboBox(parent);
	case 2: return new QLineEdit(parent);
	case 3: return new QLineEdit(parent);
	case 4: return new QLineEdit(parent);
	case 5: return new QLineEdit(parent);
	}
	return NULL;
}

} // namespace Shady