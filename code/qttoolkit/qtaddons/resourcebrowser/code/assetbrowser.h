#pragma once
//------------------------------------------------------------------------------
/**
	@class ResourceBrowser::TextureBrowser
	
	The texture browser is used to display textures in 'work', their name, size and format, and then preview their DDS counterpart in a tiled window.
	
	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "core/singleton.h"
#include "imageloaderthread.h"

#include <QDialog>
#include <QGraphicsGridLayout>
#include <QGraphicsPixmapItem>
#include <QList>
#include <QShortcut>
#include <QMenuBar>
#include <QDockWidget>
#include <QListWidgetItem>
#include <QPropertyAnimation>

namespace Ui
{
	class AssetBrowser;
}
namespace ResourceBrowser
{

class TiledGraphicsItem;
class AssetBrowser : 
	public QDockWidget,
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
	/// remove item from outside the browser
	void RemoveItem(TiledGraphicsItem* item);

	/// execute the browser, making it impossible to unless selected
	int Execute();
	/// accept the browser, closing the modality by force
	void Accept();
	/// accept the browser, closing the modality by force
	void Reject();

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
	/// handle pressing escape
	void OnEscape();
	/// handle whenever an item gets right clicked
	void OnItemRightClicked(QGraphicsSceneContextMenuEvent* event);

    /// handle the textures filter checkbox
    void OnTexturesFilterChecked(bool b);
    /// handle the models filter checkbox
    void OnModelsFilterChecked(bool b);
    /// handle the surfaces filter checkbox
    void OnSurfacesFilterChecked(bool b);

	/// handle sorting
	void OnSortingChanged();

	/// update the folder list widget
	void UpdateAssetFolders();
	/// handle an item getting double clicked in the asset folder
	void OnAssetFolderClicked(QListWidgetItem* item);

	/// handle the animation getting updated
	void OnAnimationUpdated(const QVariant& value);

signals:
    /// signal emitted when a texture is selected, only happens if browser is not opened through Execute
    void TextureSelected(const QString& tex);
    /// signal emitted when a model is selected, only happens if browser is not opened through Execute
    void ModelSelected(const QString& mdl);
    /// signal emitted when a surface is selected, only happens if browser is not opened through Execute
    void SurfaceSelected(const QString& sur);
	/// signal emitted when browser gets right clicked
	void ContextMenuOpened(QContextMenuEvent* event);
	/// signal emitted when an item was right clicked
	void ItemContextMenuOpened(ResourceBrowser::TiledGraphicsItem* item, QGraphicsSceneContextMenuEvent* event);

private:

	friend class TiledTextureItem;
    friend class TiledSurfaceItem;
    friend class TiledModelItem;

	static ImageLoaderThread* loaderThread;

	/// helper function to setup directories
	void SetupRoot();

	bool isExecuting;
	bool shouldStopExecuting;
	int executeResult;

	AssetFilter filter;
	QString selectedResource;
	Ui::AssetBrowser* ui;
	QGraphicsGridLayout* layout;
	QShortcut* backShortcut;
	QShortcut* escShortcut;
	QPropertyAnimation* executingAnimation;
    QMenuBar menubar;
	QList<QGraphicsPixmapItem*> items;
	FileWatcher assetFolderWatcher;
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