#pragma once
//------------------------------------------------------------------------------
/**
    @class QTFeature::QTServer
       
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "messaging/message.h"
#include "util/delegate.h"
#include <QApplication>

//------------------------------------------------------------------------------
namespace QtFeature
{
class QtServer : public Core::RefCounted
{
    __DeclareClass(QtServer);
    __DeclareSingleton(QtServer);
public:
    /// constructor
    QtServer();
    /// destructor
    virtual ~QtServer();

	/// creates a qapplication
	void Open();
    /// cleans up qt system
    void Close();
	/// Update qt event loop
	void Trigger();

private:
    QApplication * application;
    int argc;
    const char * name;
    const char** argv;
}; 


} // namespace QTFeature
//------------------------------------------------------------------------------
