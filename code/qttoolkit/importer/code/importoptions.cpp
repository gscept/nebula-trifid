#include "importoptions.h"

namespace Importer
{


//------------------------------------------------------------------------------
/**
*/
ImportOptions::ImportOptions() :
	importFlags(None),
	importMode(Static)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ImportOptions::~ImportOptions()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ImportOptions::AddTake( Take* take )
{
	this->takes.append(take);
}

//------------------------------------------------------------------------------
/**
*/
Take* 
ImportOptions::GetTake( uint index )
{
	return this->takes[index];
}

//------------------------------------------------------------------------------
/**
*/
const QList<Take*>& 
ImportOptions::GetTakes() const
{
	return this->takes;
}


}