//------------------------------------------------------------------------------
//  tiledmodelitem.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tiledmodelitem.h"
#include "assetbrowser.h"
#include <QGraphicsSceneEvent>

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

    // create a new texture unit
    this->loader = new ImageLoaderUnit;
    this->loader->path = String::Sprintf("%s/%s/%s_n3.thumb", this->path.AsCharPtr(), this->category.AsCharPtr(), this->filename.AsCharPtr());;
    connect(this->loader, SIGNAL(OnLoaded()), this, SLOT(OnPreviewLoaded()));
    AssetBrowser::loaderThread->Enqueue(this->loader);

    // format string with the 'clean' name
    QString format;
    format.sprintf("<p align=\"center\"><b>Model</b><br>%s</p>", this->filename.AsCharPtr());
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
    this->loader->mutex.lock();
	this->loader->Release();
	this->loader->mutex.unlock();
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