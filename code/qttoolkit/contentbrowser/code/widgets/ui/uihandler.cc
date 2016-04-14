//------------------------------------------------------------------------------
//  uihandler.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "uihandler.h"
#include "ui/uifeatureunit.h"

namespace Widgets
{
__ImplementClass(Widgets::UIHandler, 'UIIH', Widgets::BaseHandler);

//------------------------------------------------------------------------------
/**
*/
UIHandler::UIHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
UIHandler::~UIHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
UIHandler::Setup()
{
	BaseHandler::Setup();

	// connect reload buttons to actions
	connect(this->ui->reloadButton, SIGNAL(clicked()), this, SLOT(Reload()));	
	connect(this->ui->visibleCheckBox, SIGNAL(stateChanged(int)), this, SLOT(Visible(int)));
}

//------------------------------------------------------------------------------
/**
*/
void
UIHandler::Cleanup()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
UIHandler::Discard()
{		
	this->Unload();
	UI::UiFeatureUnit::Instance()->StopRenderDebug();
	return BaseHandler::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void
UIHandler::SetUI(Ui::UIInfoWidget* ui)
{
	n_assert(ui);
	this->ui = ui;
	
}
//------------------------------------------------------------------------------
/**
*/
void
UIHandler::LoadUIInfo()
{
	if (!UI::UiFeatureUnit::Instance()->HasLayout(this->GetName()))
	{
		Ptr<UI::UiLayout> layout = UI::UiFeatureUnit::Instance()->CreateLayout(this->GetName(), "gui:" + this->GetName());
		layout->Show();
	}
	else
	{
		Ptr<UI::UiLayout> layout = UI::UiFeatureUnit::Instance()->GetLayout(this->GetName());
		if (!layout->IsShown())
		{
			layout->Show();
		}
	}
	this->ui->visibleCheckBox->blockSignals(true);
	this->ui->visibleCheckBox->setChecked(true);
	this->ui->visibleCheckBox->blockSignals(false);
	UI::UiFeatureUnit::Instance()->StartRenderDebug();
}

//------------------------------------------------------------------------------
/**
*/
void
UIHandler::LoadFontInfo()
{
	
}

//------------------------------------------------------------------------------
/**
*/
void
UIHandler::Reload()
{
	if (UI::UiFeatureUnit::Instance()->HasLayout(this->GetName()))
	{
		Ptr<UI::UiLayout> layout = UI::UiFeatureUnit::Instance()->GetLayout(this->GetName());
		layout->Reload();
	}
	else
	{
		this->LoadUIInfo();
	}
	
}


//------------------------------------------------------------------------------
/**
*/
void
UIHandler::Unload()
{
	UI::UiFeatureUnit::Instance()->FreeLayout(this->GetName());	
}

//------------------------------------------------------------------------------
/**
*/
void
UIHandler::Visible(int state)
{
	if (state == 0)
	{
		Ptr<UI::UiLayout> layout = UI::UiFeatureUnit::Instance()->GetLayout(this->GetName());
		if (layout->IsShown())
		{
			layout->Hide();
		}		
	}
	else
	{
		Ptr<UI::UiLayout> layout = UI::UiFeatureUnit::Instance()->GetLayout(this->GetName());		
		layout->Show();
	}
	
}

} // namespace Widgets