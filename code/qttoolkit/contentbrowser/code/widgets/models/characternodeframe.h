#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::CharacterNodeFrame
    
    Implements a frame specific for character nodes.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QFrame>
#include "characternodehandler.h"
#include "ui_characternodeinfowidget.h"
namespace Widgets
{
class CharacterNodeFrame : public QFrame
{
public:
	/// constructor
	CharacterNodeFrame();
	/// destructor
	virtual ~CharacterNodeFrame();

	/// returns pointer to handler
	const Ptr<CharacterNodeHandler>& GetHandler() const;
	/// discards a model node frame
	void Discard();
private:
	Ptr<CharacterNodeHandler> itemHandler;
	Ui::CharacterNodeInfoWidget ui;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CharacterNodeHandler>& 
CharacterNodeFrame::GetHandler() const
{
	return this->itemHandler;
}
} // namespace Widgets
//------------------------------------------------------------------------------