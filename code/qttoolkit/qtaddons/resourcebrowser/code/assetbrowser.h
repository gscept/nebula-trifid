#pragma once
//------------------------------------------------------------------------------
/**
	@class ResourceBrowser::TextureBrowser
	
	The texture browser is used to display textures in 'work', their name, size and format, and then preview their DDS counterpart in a tiled window.
	
	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QDialog>
#include <QGraphicsGridLayout>
#include <QGraphicsPixmapItem>
#include <QList>
#include <QShortcut>
#include "core/refcounted.h"
#include "core/singleton.h"
#include "imageloaderthread.h"

namespace Ui
{
	class AssetBrowser;
}
namespace ResourceBrowser
{

class AssetBrowser : 
	public QDialog,
	public Core::RefCounted
{
	Q_OBJECT
	__DeclareClass(AssetBrowser);	
	__DeclareSingleton(AssetBrowser);
public:
	/// constructor
	AssetBrowser();
	/// destructor
	virtual ~AssetBrowser();

	/// open texture browser, and close it whenever a texture is selected or the window closed
	int Execute(const QString& title);

	/// open texture browser
	void Open();
	/// close texture browser
	void Close();

	/// handle opening
	void showEvent(QShowEvent* event);
	/// handle closing
	void closeEvent(QCloseEvent* event);

	/// get the texture selected
	const QString& GetSelectedTexture() const;

private slots:
	/// handle a directory being clicked
	void OnDirectoryClicked(const QString& dir, const QString& path);
	/// handle a texture being clicked
	void OnTextureClicked(const QString& tex);
	/// handle model getting clicked
	void OnModelClicked(const QString& mdl);
	/// handle material getting clicked
	void OnSurfaceClicked(const QString& sur);
	/// handle backing in the hierarchy
	void OnBack();

private:

	friend class TiledTextureItem;
    friend class TiledSurfaceItem;
    friend class TiledModelItem;

	static ImageLoaderThread* loaderThread;

	/// helper function to setup directories
	void SetupRoot();

	bool isExecuting;
	QString selectedResource;
	Ui::AssetBrowser* ui;
	QGraphicsGridLayout* layout;
	QShortcut* shortcut;
	QList<QGraphicsPixmapItem*> items;
};


//------------------------------------------------------------------------------
/**
*/
inline const QString&
AssetBrowser::GetSelectedTexture() const
{
	return this->selectedResource;
}

} // namespace ResourceBrowser