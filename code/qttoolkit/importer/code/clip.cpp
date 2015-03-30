#include "clip.h"
namespace Importer
{

//------------------------------------------------------------------------------
/**
*/
Clip::Clip()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Clip::~Clip()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
Clip::SetName( const QString& name )
{
	this->name = name;
}


//------------------------------------------------------------------------------
/**
*/
const QString& 
Clip::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
void 
Clip::SetStart( uint start )
{
	this->start = start;
}

//------------------------------------------------------------------------------
/**
*/
const uint 
Clip::GetStart() const
{
	return this->start;
}

//------------------------------------------------------------------------------
/**
*/
void 
Clip::SetEnd( uint end )
{
	this->end = end;
}

//------------------------------------------------------------------------------
/**
*/
const uint 
Clip::GetEnd() const
{
	return this->end;
}

//------------------------------------------------------------------------------
/**
*/
void 
Clip::SetPreInfinity( Clip::InfinityType infinityType )
{
	this->preInfinity = infinityType;
}

//------------------------------------------------------------------------------
/**
*/
const Clip::InfinityType 
Clip::GetPreInfinity() const
{
	return this->preInfinity;
}

//------------------------------------------------------------------------------
/**
*/
void 
Clip::SetPostInfinity(  Clip::InfinityType infinityType )
{
	this->postInfinity = infinityType;
}

//------------------------------------------------------------------------------
/**
*/
const Clip::InfinityType 
Clip::GetPostInfinity() const
{
	return this->postInfinity;
}
}