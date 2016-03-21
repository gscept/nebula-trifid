#pragma once
//------------------------------------------------------------------------------
/**
    @class Widgets::BaseHandler
    
    Base class for resource handlers.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include <QObject>
namespace Widgets
{
class BaseHandler : 
	public QObject,
	public Core::RefCounted
{
	Q_OBJECT
	__DeclareClass(BaseHandler);
public:
	/// constructor
	BaseHandler();
	/// destructor
	virtual ~BaseHandler();

    /// cleanup the handler
    virtual void Cleanup();

	/// sets up handler
	virtual void Setup();
	/// discards handler
	virtual bool Discard();

	/// handle on-frame stuff like overlay UI
	virtual void OnFrame();

	/// returns true if handler is setup
	const bool IsSetup() const;

protected:
	bool isSetup;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const bool 
BaseHandler::IsSetup() const
{
	return this->isSetup;
}

} // namespace Widgets
//------------------------------------------------------------------------------