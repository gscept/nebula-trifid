#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::TextureItem
    
    QTreeWidgetItem responsible for clicking textures
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/baseitem.h"
#include "util/string.h"
#include "texturehandler.h"
#include "ui_textureinfowidget.h"
namespace Widgets
{
class TextureItem :	public BaseItem
{
public:
	/// constructor
	TextureItem();
	/// destructor
	~TextureItem();

	/// clones model item
	BaseItem* Clone();

	/// sets the ui
	void SetUi(Ui::TextureInfoWidget* ui);
	/// returns pointer to ui
	Ui::TextureInfoWidget* GetUi() const;

	/// sets up dock widget with texture display information
	void OnActivated();
	/// handles events whenever the item is clicked
	void OnClicked();
	/// handles events whenever the item is right clicked
	void OnRightClicked(const QPoint& pos);

	/// loads a texture from file
	void LoadTexture(const QString& path);

private:
	
	Ui::TextureInfoWidget* ui;
};


//------------------------------------------------------------------------------
/**
*/
inline void 
TextureItem::SetUi( Ui::TextureInfoWidget* ui )
{
	this->ui = ui;
}

//------------------------------------------------------------------------------
/**
*/
inline Ui::TextureInfoWidget* 
TextureItem::GetUi() const
{
	return this->ui;
}

} // namespace Widgets
//------------------------------------------------------------------------------