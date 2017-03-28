//------------------------------------------------------------------------------
//  rocketlayout.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "rocketlayout.h"
#include "rocketserver.h"
#include "io/uri.h"
#include "messaging/staticmessagehandler.h"
#include "resources/resource.h"
#include "io/ioserver.h"
#include "Rocket/Core/StyleSheetFactory.h"

using namespace Resources;
using namespace UI;
namespace LibRocket
{
__ImplementClass(LibRocket::RocketLayout, 'ROLY', Base::UiLayoutBase);

//------------------------------------------------------------------------------
/**
*/
RocketLayout::RocketLayout() :
	layout(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
RocketLayout::~RocketLayout()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketLayout::Load()
{
	n_assert(!this->layout);

	// get rocket context
	Rocket::Core::Context* context = RocketServer::Instance()->GetContext();

	// convert resource to file-system path
	IO::URI uri(this->resourceId.AsString());

	// load layout
	this->layout = context->LoadDocument(uri.GetHostAndLocalPath().AsCharPtr());
	n_assert(this->layout);	
	this->layout->SetAttribute<void*>("_ParentClass", (void*)this);

	this->resState = Resource::Loaded;
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketLayout::Reload()
{
	n_assert(this->layout);
	UiLayoutBase::Reload();

	bool isVisible = this->layout->IsVisible();
	this->layout->Hide();

	// cleanup layout, similar to Discard
	this->layout->GetContext()->UnloadDocument(this->layout);
	this->layout->RemoveReference();
	this->layout = 0;
	this->resState = Resource::Initial;

	// clear stylesheet cache
	Rocket::Core::StyleSheetFactory::ClearStyleSheetCache();

	// trigger a load
	this->Load();

	if (isVisible)
	{
		this->layout->Show();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketLayout::Discard()
{
	n_assert(this->layout);
	this->layout->GetContext()->UnloadDocument(this->layout);
	this->layout->RemoveReference();
	this->layout = 0;
	
	UiLayoutBase::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketLayout::Show()
{
	n_assert(this->layout);
	this->layout->Show();    
	UiLayoutBase::Show();
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketLayout::Hide()
{
	n_assert(this->layout);
	this->layout->Hide();
	UiLayoutBase::Hide();
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketLayout::Update()
{
	n_assert(this->layout);
	this->layout->Update();
	UiLayoutBase::Update();
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketLayout::Render(const Util::StringAtom& filter)
{
	n_assert(this->layout);
	this->layout->Render();
	UiLayoutBase::Render(filter);
}

} // namespace Rocket
