//------------------------------------------------------------------------------
//  tiledmodelitem.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tiledmodelitem.h"
#include "assetbrowser.h"
#include "io/uri.h"

#include <QGraphicsSceneEvent>
#include <QFileInfo>

using namespace Util;
namespace ResourceBrowser
{

//------------------------------------------------------------------------------
/**
*/
TiledModelItem::TiledModelItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
TiledModelItem::~TiledModelItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
TiledModelItem::Setup()
{
    // setup base class
    TiledGraphicsItem::Setup();

    // remove extension
    this->filename.StripFileExtension();

	// set color
	this->background->setBrush(QBrush(qRgb(240, 160, 0)));

    // create a new texture unit
    this->loader = new ImageLoaderUnit;
    this->loader->path = String::Sprintf("%s/%s/%s_n3.thumb", this->path.AsCharPtr(), this->category.AsCharPtr(), this->filename.AsCharPtr());;
    connect(this->loader, SIGNAL(OnLoaded()), this, SLOT(OnPreviewLoaded()));
    AssetBrowser::loaderThread->Enqueue(this->loader);

	// get changed date
	IO::URI res = String::Sprintf("%s/%s/%s.attributes", this->path.AsCharPtr(), this->category.AsCharPtr(), this->filename.AsCharPtr());
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
TiledModelItem::Discard()
{
    TiledGraphicsItem::Discard();

    // make sure our thread is using the mutex, then delete the loader unit
	this->loader->Release();
}

//------------------------------------------------------------------------------
/**
*/
void
TiledModelItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        String res = String::Sprintf("mdl:%s/%s.n3", this->category.AsCharPtr(), this->filename.AsCharPtr());
        emit this->OnSelected(QString(res.AsCharPtr()));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
TiledModelItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	emit this->ItemRightClicked(event);
}

} // namespace ResourceBrowser