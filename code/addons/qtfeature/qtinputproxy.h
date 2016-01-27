#pragma once
//------------------------------------------------------------------------------
/**
    @class QtFeature::QtInputProxy
    
    Input handler that will forward all nebula keybord events to a QApplication
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "input/inputhandler.h"
#include "input/key.h"
#include <QObject>

//------------------------------------------------------------------------------
namespace QtFeature
{
class QtInputProxy: public Input::InputHandler
{
    __DeclareClass(QtInputProxy);
public:
        /// constructor
    QtInputProxy();
    /// destructor
    virtual ~QtInputProxy();
    
    /// set target widget for inputs
    void SetReceiver(QObject * obj);
    
	/// converts Qt key to Nebula
	static int Qt2NebInputKey(const Qt::Key& inCode);
	/// converts Nebula key to Qt
	static int Neb2QtInputKey(const Input::Key::Code& inCode);

	/// sets if the input proxy should ignore events
	void SetIgnoreEvents(bool b);

protected:
    friend class Base::InputServerBase;
        
    /// called when an input event should be processed
    virtual bool OnEvent(const Input::InputEvent& inputEvent);    

private:       
    /// key down event
    bool OnKeyDown(const Input::InputEvent& inputEvent);
    /// key up event
    bool OnKeyUp(const Input::InputEvent& inputEvent);     
    /// focus gained
    bool OnGainFocus();
    /// Send key event
    bool SendKeyEvent(const Input::InputEvent& inputEvent);

	/// receiver target
    QObject* receiver;

	bool ignore;
};

//------------------------------------------------------------------------------
/**
*/
inline void
QtInputProxy::SetIgnoreEvents(bool b)
{
	ignore = b;
}

} // namespace QtFeature
//------------------------------------------------------------------------------