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

	enum AssetFilter
	{
		Textures = 1 << 0,
		Models = 1 << 1,
		Surfaces = 1 << 2,
		All = Textures + Models + Surfaces
	};

	/// constructor
	AssetBrowser();
	/// destructor
	virtual ~AssetBrowser();

	/// open texture browser, and close it whenever a texture is selected or the window closed
	int Execute(const QString& title, const AssetFilter& filter);

	/// open texture browser
	void Open();
	/// close texture browser
	void Close();

	/// handle opening
	void showEvent(QShowEvent* event);
	/// handle closing
	void closeEvent(QCloseEvent* event);
	/// handle right clicking
	void contextMenuEvent(QContextMenuEvent* event);

	/// get the texture selected
	const QString& GetSelectedTexture() const;

	/// set the filter of the asset browser
	void SetFilter(const AssetFilter& filter);

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

signals:
    /// signal emitted when a texture is selected, only happens if browser is not opened through Execute
    void TextureSelected(const QString& tex);
    /// signal emitted when a model is selected, only happens if browser is not opened through Execute
    void ModelSelected(const QString& mdl);
    /// signal emitted when a surface is selected, only happens if browser is not opened through Execute
    void SurfaceSelected(const QString& sur);
	/// signal emitted when browser gets right clicked
	void ContextMenuOpened(QContextMenuEvent* event);

private:

	friend class TiledTextureItem;
    friend class TiledSurfaceItem;
    friend class TiledModelItem;

	static ImageLoaderThread* loaderThread;

	/// helper function to setup directories
	void SetupRoot();

	bool isExecuting;
	AssetFilter filter;
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