#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::UIHandler
    
    Handles UI items
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "widgets/basehandler.h"
#include <QObject>
#include "ui_uiinfowidget.h"

namespace Widgets
{
class UIHandler : public BaseHandler
{
	Q_OBJECT
		__DeclareClass(UIHandler);
public:
	/// constructor
	UIHandler();
	/// destructor
	virtual ~UIHandler();

	/// sets up handler
	virtual void Setup();

	/// discards handler
	virtual bool Discard();

	/// cleans up the handler
	virtual void Cleanup();

	/// sets the node name of the handler
	void SetName(const Util::String& name);
	/// gets the node name of the handler
	const Util::String& GetName() const;

	/// sets the ui on which this handler shall perform its actions
	void SetUI(Ui::UIInfoWidget* ui);
	/// gets pointer to ui
	Ui::UIInfoWidget* GetUI() const;

	/// loads info and puts it in the ui infowidget
	void LoadUIInfo();

	/// loads font information into infowidget
	void LoadFontInfo();

	/// Unload the layout
	void Unload();

	private slots:
	/// Reload
	void Reload();
	/// visibility changed
	void Visible(int);	

private:
	Util::String path;	
	Ui::UIInfoWidget* ui;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
UIHandler::SetName(const Util::String& resource)
{
	n_assert(resource.IsValid());
	this->path = resource;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
UIHandler::GetName() const
{
	return this->path;
}


} // namespace ContentBrowser
//------------------------------------------------------------------------------