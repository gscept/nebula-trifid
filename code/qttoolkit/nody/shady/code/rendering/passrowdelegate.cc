//------------------------------------------------------------------------------
// passrowdelegate.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "passrowdelegate.h"
#include "QLineEdit"
#include "materialwindow.h"
#include "coregraphics/shaderserver.h"
#include "ui_frameshaderbrowser.h"
#include "frameshaderviz.h"

namespace Shady
{

//------------------------------------------------------------------------------
/**
*/
PassRowDelegate::PassRowDelegate()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
PassRowDelegate::~PassRowDelegate()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
PassRowDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	MaterialWindow::MaterialPassState& state = this->window->GetCurrentState().passes[index.row()];
	switch (index.column())
	{
	case 0:
	{
		QLineEdit* edit = (QLineEdit*)editor;
		state.batch = edit->text().toUtf8().constData();
		model->setData(index, state.batch.AsCharPtr());
		break;
	}
	case 1:
	{
		QComboBox* edit = (QComboBox*)editor;
		state.shader = edit->currentText().toUtf8().constBegin();
		model->setData(index, state.shader.AsCharPtr());

		Util::String shaderResource = Util::String::Sprintf("shd:%s", state.shader.AsCharPtr());
		Ptr<CoreGraphics::Shader> shader = CoreGraphics::ShaderServer::Instance()->GetShader(shaderResource);
		state.variation = CoreGraphics::ShaderServer::Instance()->FeatureMaskToString(shader->GetVariationByIndex(0)->GetFeatureMask());
		model->setData(model->index(index.row(), 2), state.variation.AsCharPtr());
		break;
	}
	case 2:
	{
		QComboBox* edit = (QComboBox*)editor;
		state.variation = edit->currentText().toUtf8().constBegin();
		model->setData(index, state.variation.AsCharPtr());
		break;
	}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
PassRowDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	MaterialWindow::MaterialPassState& state = this->window->GetCurrentState().passes[index.row()];
	switch (index.column())
	{
	case 0:
	{
		QPushButton* edit = (QPushButton*)editor;
		edit->setText(state.batch.AsCharPtr());
		break;
	}
	case 1:
	{
		QComboBox* edit = (QComboBox*)editor;
		Util::Array<Resources::ResourceId> shaders = CoreGraphics::ShaderServer::Instance()->GetAllShaders().KeysAsArray();
		IndexT i;
		for (i = 0; i < shaders.Size(); i++)
		{
			Util::String resource = shaders[i].AsString();
			resource.StripAssignPrefix();
			edit->addItem(resource.AsCharPtr());
		}
		edit->setCurrentIndex(edit->findText(state.shader.AsCharPtr()));
		break;
	}
	case 2:
	{
		QComboBox* edit = (QComboBox*)editor;
		Util::String shaderResource = Util::String::Sprintf("shd:%s", state.shader.AsCharPtr());
		Ptr<CoreGraphics::Shader> shader = CoreGraphics::ShaderServer::Instance()->GetShader(shaderResource);
		IndexT i;
		for (i = 0; i < shader->GetNumVariations(); i++)
		{
			Util::String str = CoreGraphics::ShaderServer::Instance()->FeatureMaskToString(shader->GetVariationByIndex(i)->GetFeatureMask());
			edit->addItem(str.AsCharPtr());
		}
		edit->setCurrentIndex(edit->findText(state.variation.AsCharPtr()));
		break;
	}
	}
}

//------------------------------------------------------------------------------
/**
*/
QWidget*
PassRowDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	switch (index.column())
	{
	case 0:	{ this->OnBrowseFrameShaders(index); return NULL; }
	case 1: return new QComboBox(parent);
	case 2: return new QComboBox(parent);
	}
	return NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
PassRowDelegate::OnBrowseFrameShaders(const QModelIndex& index) const
{
	FrameshaderViz viz;
	int code = viz.exec();
	if (code == QDialog::Accepted)
	{

	}
}

} // namespace Shady