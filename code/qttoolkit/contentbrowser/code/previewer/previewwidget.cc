//------------------------------------------------------------------------------
//  previewwidget.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "previewwidget.h"
#include "contentbrowserapp.h"
#include "qmimedata.h"


namespace ContentBrowser
{

//------------------------------------------------------------------------------
/**
*/
PreviewWidget::PreviewWidget(QWidget* parent) :
	QtNebulaWidget(parent)
{
	this->setAcceptDrops(true);	
}

//------------------------------------------------------------------------------
/**
*/
PreviewWidget::~PreviewWidget()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
PreviewWidget::dragEnterEvent( QDragEnterEvent* e )
{
	// get mime data
	const QMimeData* data = e->mimeData();
	
	// only accept if this is an item
	if (data->hasFormat("nebula/resourceid"))
	{
		e->accept();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PreviewWidget::dropEvent( QDropEvent* e )
{
	// get mime data
	const QMimeData* data = e->mimeData();	
	// only accept if this is an item
	if (data->hasFormat("nebula/resourceid"))
	{
		QString text = data->data("nebula/resourceid");

		// now split on category and file
		QStringList res = text.split("/");
		QString cat = res[0];
		QString file = res[1];

		// call texture dropped callback
		this->OnTextureDropped(cat.toUtf8().constData(), file.toUtf8().constData(), Math::float2(e->pos().x(), e->pos().y()));
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PreviewWidget::OnTextureDropped( const Util::String& category, const Util::String& name, const Math::float2 pos )
{
	// get the preview state
	const Ptr<PreviewState>& state = this->app->GetPreviewState();
}

} // namespace ContentBrowser