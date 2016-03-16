#pragma once
//------------------------------------------------------------------------------
/**
    @class QtAttributeControllerAddon::AnimClipController
    
    A widget which represents a dropbox with with anim clips
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
#include <QWidget>
#include "util/string.h"
#include "core/ptr.h"
#include "util/variant.h"
#include "baseattributecontroller.h"

namespace Ui
{
	class StringListController;
}

namespace Game
{
	class Entity;
}

namespace Tools
{
	class IDLAttribute;
}

//------------------------------------------------------------------------------
namespace QtAttributeControllerAddon
{
	class AnimClipController : public BaseAttributeController
{
	Q_OBJECT
public:
	/// constructor
	AnimClipController(QWidget* parent, const Ptr<Game::Entity>& _entity, const Ptr<Tools::IDLAttribute>& _attr);
	/// destructor
	~AnimClipController();

	/// get the value this controller currently has
	Util::Variant GetValue() const;
	/// make the controller view-only
	void Lock();

protected slots:
	/// when the value of the derived controller is changed
	void currentIndexChanged(int);

private:
	/// called when the clip list is fetched
	void OnFetchedClipList(const Ptr<Messaging::Message>& msg);
	/// add clips to ui
	void SetClips(const Util::Array<Util::StringAtom> & clips, const Util::String & current);

	Ui::StringListController* ui;
	Util::String currentValue;
	bool permitEmpty;
};
}
