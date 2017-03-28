//------------------------------------------------------------------------------
//  tiledtextureitem.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tiledtextureitem.h"
#include "assetbrowser.h"
#include "io/uri.h"

#include <QGraphicsSceneMouseEvent>
#include <QFileInfo>



using namespace Util;
namespace ResourceBrowser
{

//------------------------------------------------------------------------------
/**
*/
TiledTextureItem::TiledTextureItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
TiledTextureItem::~TiledTextureItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
TiledTextureItem::Setup()
{
	// setup base class
	TiledGraphicsItem::Setup();

    // remove extension
	String extension = this->filename.GetFileExtension();
    this->filename.StripFileExtension();

	// set color
	this->background->setBrush(QBrush(qRgb(30, 30, 30)));

	// create a new texture unit
	this->loader = new ImageLoaderUnit;
	this->loader->path = String::Sprintf("tex:%s/%s.dds", this->category.AsCharPtr(), this->filename.AsCharPtr());
	connect(this->loader, SIGNAL(OnLoaded()), this, SLOT(OnPreviewLoaded()));
	AssetBrowser::loaderThread->Enqueue(this->loader);

	// get changed date
	IO::URI res = String::Sprintf("%s/%s/%s.%s", this->path.AsCharPtr(), this->category.AsCharPtr(), this->filename.AsCharPtr(), extension.AsCharPtr());
	QFileInfo info(res.LocalPath().AsCharPtr());
	this->lastChanged = info.lastModified();

	// format string with the 'clean' name
	QString format;
	format.sprintf("<p align=\"center\">%s</p>", this->filename.AsCharPtr());
	this->label->setTextWidth(this->background->boundingRect().width());
	this->label->setHtml(format);
}

//------------------------------------------------------------------------------
/**
*/
void
TiledTextureItem::Discard()
{
	TiledGraphicsItem::Discard();

	// make sure our thread is using the mutex, then delete the loader unit
	this->loader->Release();
}

//------------------------------------------------------------------------------
/**
*/
void
TiledTextureItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
        String res = String::Sprintf("tex:%s/%s.dds", this->category.AsCharPtr(), this->filename.AsCharPtr());
        emit this->OnSelected(QString(res.AsCharPtr()));
	}
}

//------------------------------------------------------------------------------
/**
	Aww, we want to be able to reconfigure stuff, but this code is in the content browser...
*/
void
TiledTextureItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	emit this->ItemRightClicked(event);
}

} // namespace ResourceBrowser