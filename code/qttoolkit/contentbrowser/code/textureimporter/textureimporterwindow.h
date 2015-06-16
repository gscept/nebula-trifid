#pragma once
//------------------------------------------------------------------------------
/**
    @class TextureImporter::TextureImporterWindow
    
    Implements the GUI for the texture importer.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "ui_textureimporterwindow.h"
#include "io/uri.h"
#include "texutil/textureattrtable.h"
#include "importerwindow.h"

namespace TextureImporter
{
class TextureImporterWindow : public ContentBrowser::ImporterWindow
{
	Q_OBJECT
public:
	/// constructor
	TextureImporterWindow();
	/// destructor
	virtual ~TextureImporterWindow();

	/// sets the file URI
	void SetUri(const IO::URI& path);
	/// opens the app
	void Open();

private slots:
	/// called whenever import is pressed
	void OnImport();

	/// called whenever width is set
	void OnWidthChanged(int value);
	/// called whenever height is set
	void OnHeightChanged(int value);
	/// called whenever filter is set
	void OnFilterChanged(int index);
	/// called whenever quality is changed
	void OnQualityChanged(int index);
	/// called whenever format (RGB) is changed
	void OnFormatRGBChanged(int index);
	/// called whenever format (RGBA) is changed
	void OnFormatRGBAChanged(int index);
	/// called whenever generate mip maps is changed
	void OnToggleGenerateMipMaps(bool state);
	/// called whenever the sRGB checkbox is changed
	void OnToggleSRGB(bool state);

private:

	ToolkitUtil::TextureAttrs currentAttrs;
	ToolkitUtil::TextureAttrTable attrTable;
	Ui::TextureImporterWindow ui;
	IO::URI path;
}; 
} // namespace TextureImporter
//------------------------------------------------------------------------------