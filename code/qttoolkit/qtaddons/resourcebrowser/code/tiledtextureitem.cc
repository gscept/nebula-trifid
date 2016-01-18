//------------------------------------------------------------------------------
//  tiledtextureitem.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tiledtextureitem.h"
#include "io/uri.h"
#include "io/ioserver.h"
#include "imageloaderthread.h"
#include "assetbrowser.h"
#include <QAction>
#include <QMenu>
#include <QComboBox>
#include <QMessageBox>
#include <QGraphicsSceneEvent>
#include <QLayout>
#include <QGraphicsScene>
#include "tiledgraphicsview.h"

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
    this->filename.StripFileExtension();

	// create a new texture unit
	this->loader = new ImageLoaderUnit;
	this->loader->path = String::Sprintf("tex:%s/%s.dds", this->category.AsCharPtr(), this->filename.AsCharPtr());
	connect(this->loader, SIGNAL(OnLoaded()), this, SLOT(OnPreviewLoaded()));
	AssetBrowser::loaderThread->Enqueue(this->loader);

	// format string with the 'clean' name
	QString format;
	format.sprintf("<p align=\"center\"><b>Texture</b><br>%s</p>", this->filename.AsCharPtr());
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
    this->loader->mutex.lock();
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