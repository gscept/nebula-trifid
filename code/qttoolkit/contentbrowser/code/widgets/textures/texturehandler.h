#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::TextureItemHandler
    
    Implements a handler class for texture items
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/basehandler.h"
#include <QObject>
#include "ui_textureinfowidget.h"

namespace Widgets
{
class TextureHandler : public BaseHandler
{
	Q_OBJECT
	__DeclareClass(TextureHandler);
public:
	/// constructor
	TextureHandler();
	/// destructor
	virtual ~TextureHandler();

	/// sets the texture resource
	void SetTextureResource(const Util::String& resource);
	/// gets the texture resource
	const Util::String& GetTextureResource() const;
	/// sets the texture category
	void SetTextureCategory(const Util::String& cat);
	/// gets the texture category
	const Util::String& GetTextureCategory() const;
	/// sets the ui on which this handler shall perform its actions
	void SetUI(Ui::TextureInfoWidget* ui);
	/// gets pointer to ui
	Ui::TextureInfoWidget* GetUI() const;

	/// loads texture and puts it in the ui
	bool LoadTexture();

private slots:
	/// called whenever the texture needs a reload
	void OnReload();

private:
	
	Util::String texture;
	Util::String category;
	Ui::TextureInfoWidget* ui;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
TextureHandler::SetTextureResource( const Util::String& resource )
{
	n_assert(resource.IsValid());
	this->texture = resource;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
TextureHandler::GetTextureResource() const
{
	return this->texture;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
TextureHandler::SetTextureCategory( const Util::String& cat )
{
	n_assert(cat.IsValid());
	this->category = cat;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
TextureHandler::GetTextureCategory() const
{
	return this->category;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
TextureHandler::SetUI( Ui::TextureInfoWidget* ui )
{
	n_assert(ui);
	this->ui = ui;
}

//------------------------------------------------------------------------------
/**
*/
inline Ui::TextureInfoWidget* 
TextureHandler::GetUI() const
{
	return this->ui;
}
} // namespace Widgets
//------------------------------------------------------------------------------