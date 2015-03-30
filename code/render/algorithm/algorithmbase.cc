//------------------------------------------------------------------------------
//  algorithmbase.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "algorithmbase.h"
#include "graphics/graphicsinterface.h"

using namespace CoreGraphics;
using namespace Graphics;
namespace Algorithm
{
__ImplementClass(Algorithm::AlgorithmBase, 'ALBS', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
AlgorithmBase::AlgorithmBase() :
	enabled(true),
	deferredSetup(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AlgorithmBase::~AlgorithmBase()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
AlgorithmBase::Setup()
{
	// just setup handler, since we don't know what size our output should have
	this->handler = AlgorithmHandler::Create();
	this->handler->SetAlgorithm(this);
	GraphicsInterface::Instance()->AttachHandler(this->handler.upcast<Messaging::Handler>());
}

//------------------------------------------------------------------------------
/**
*/
void 
AlgorithmBase::Discard()
{
	// remove from graphics interface and clear pointer
    this->handler->Close();
	this->handler = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool 
AlgorithmBase::HandleMessage( const Ptr<Messaging::Message>& msg )
{
	// return in subclass
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
AlgorithmBase::Prepare()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
AlgorithmBase::Execute()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
AlgorithmBase::Finish()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
AlgorithmBase::OnDisplayResized(SizeT width, SizeT height)
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
AlgorithmBase::Enable()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
AlgorithmBase::Disable()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
AlgorithmBase::SetEnabled( bool b )
{
	this->enabled = b;
	if (b)
	{
		this->Enable();
	}
	else
	{
		this->Disable();
	}
}

} // namespace Algorithm