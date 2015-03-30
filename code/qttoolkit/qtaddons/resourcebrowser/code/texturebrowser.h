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
#include "textureloaderthread.h"

namespace Ui
{
	class TextureBrowser;
}
namespace ResourceBrowser
{

class TextureBrowser : 
	public QDialog,
	public Core::RefCounted
{
	Q_OBJECT
	__DeclareClass(TextureBrowser);	
	__DeclareSingleton(TextureBrowser);
public:
	/// constructor
	TextureBrowser();
	/// destructor
	virtual ~TextureBrowser();

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
	void OnDirectoryClicked(const QString& dir);
	/// handle a texture being clicked
	void OnTextureClicked(const QString& tex);
	/// handle gfx
	void OnGfxClicked(const QString& gfx);
	/// handle backing in the hierarchy
	void OnBack();

private:

	friend class TiledTextureItem;
	static TextureLoaderThread* loaderThread;

	/// helper function to setup directories
	void SetupRoot();

	bool isExecuting;
	QString selectedResource;
	Ui::TextureBrowser* ui;
	QGraphicsGridLayout* layout;
	QShortcut* shortcut;
	QList<QGraphicsPixmapItem*> items;
};


//------------------------------------------------------------------------------
/**
*/
inline const QString&
TextureBrowser::GetSelectedTexture() const
{
	return this->selectedResource;
}

} // namespace ResourceBrowser