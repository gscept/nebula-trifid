//------------------------------------------------------------------------------
//  tiledsurfaceitem.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tiledsurfaceitem.h"
#include "assetbrowser.h"
#include <QGraphicsSceneMouseEvent>

using namespace Util;
namespace ResourceBrowser
{

//------------------------------------------------------------------------------
/**
*/
TiledSurfaceItem::TiledSurfaceItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
TiledSurfaceItem::~TiledSurfaceItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
TiledSurfaceItem::Setup()
{
    // setup base class
    TiledGraphicsItem::Setup();

    // remove extension
    this->filename.StripFileExtension();

    // create a new texture unit
    this->loader = new ImageLoaderUnit;
    this->loader->path = String::Sprintf("%s/%s/%s_sur.thumb", this->path.AsCharPtr(), this->category.AsCharPtr(), this->filename.AsCharPtr());
    connect(this->loader, SIGNAL(OnLoaded()), this, SLOT(OnPreviewLoaded()));
    AssetBrowser::loaderThread->Enqueue(this->loader);

    // format string with the 'clean' name
    QString format;
    format.sprintf("<p align=\"center\"><b>Surface material</b><br>%s</p>", this->filename.AsCharPtr());
    this->label->setTextWidth(this->background->boundingRect().width());
    this->label->setHtml(format);
}

//------------------------------------------------------------------------------
/**
*/
void
TiledSurfaceItem::Discard()
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
TiledSurfaceItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        String res = String::Sprintf("sur:%s/%s.sur", this->category.AsCharPtr(), this->filename.AsCharPtr());
        emit this->OnSelected(QString(res.AsCharPtr()));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
TiledSurfaceItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	emit this->ItemRightClicked(event);
}

//------------------------------------------------------------------------------
/**
*/
void
TiledSurfaceItem::OnPreviewLoaded()
{
    // set position of graphics
    this->graphics->setPixmap(QPixmap::fromImage(*this->loader->texture));
    this->graphics->setPos(
        this->background->boundingRect().width() / 2 - this->graphics->boundingRect().width() / 2,
        this->background->boundingRect().width() / 2 - this->graphics->boundingRect().height() / 2);

    // move label too
    this->label->setPos(this->label->pos().x(), this->graphics->boundingRect().height() + 20);
}

} // namespace ResourceBrowser