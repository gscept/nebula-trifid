#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::Saveable
    
    Handles per-object save, load and reset functions. 
	Override this if you want your class to be load/save/reset-able!
	For convenience, Saveable inherits RefCounted so subclasses doesn't need to
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "io/stream.h"
namespace Nody
{
class Saveable : public Core::RefCounted
{
	__DeclareClass(Saveable);
public:
	/// constructor
	Saveable();
	/// destructor
	virtual ~Saveable();

	/// loads object
	virtual void Load(const Ptr<IO::Stream>& stream);
	/// saves object
	virtual void Save(const Ptr<IO::Stream>& stream);
	/// resets object to its original state
	virtual void Reset();
}; 
} // namespace Nody
//------------------------------------------------------------------------------