//------------------------------------------------------------------------------
//  annotable.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "annotable.h"
#include <assert.h>

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
Annotable::Annotable()
{
	this->annotationMap.clear();
}

//------------------------------------------------------------------------------
/**
*/
Annotable::~Annotable()
{
	// clear all strings
	std::map<std::string, AnnotationVariant>::iterator it = this->annotationMap.begin();
	for (it = this->annotationMap.begin(); it != this->annotationMap.end(); it++)
	{
		if (it->second.type == String)
		{
			delete it->second.data.stringValue;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
int 
Annotable::GetAnnotationInt( const std::string& name )
{
	assert(this->annotationMap.find(name) != this->annotationMap.end());
	return this->annotationMap[name].data.intValue;
}

//------------------------------------------------------------------------------
/**
*/
bool 
Annotable::GetAnnotationBool( const std::string& name )
{
	assert(this->annotationMap.find(name) != this->annotationMap.end());
	return this->annotationMap[name].data.boolValue;
}

//------------------------------------------------------------------------------
/**
*/
double 
Annotable::GetAnnotationDouble( const std::string& name )
{
	assert(this->annotationMap.find(name) != this->annotationMap.end());
	return this->annotationMap[name].data.doubleValue;
}

//------------------------------------------------------------------------------
/**
*/
float 
Annotable::GetAnnotationFloat( const std::string& name )
{
	assert(this->annotationMap.find(name) != this->annotationMap.end());
	return this->annotationMap[name].data.floatValue;
}

//------------------------------------------------------------------------------
/**
*/
const std::string& 
Annotable::GetAnnotationString( const std::string& name )
{
	assert(this->annotationMap.find(name) != this->annotationMap.end());
	return *this->annotationMap[name].data.stringValue;
}
} // namespace AnyFX