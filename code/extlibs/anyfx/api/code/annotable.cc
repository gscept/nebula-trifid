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
    eastl::map<eastl::string, AnnotationVariant>::iterator it = this->annotationMap.begin();
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
bool
Annotable::HasAnnotation(const eastl::string& name)
{
    return this->annotationMap.find(name) != this->annotationMap.end();
}

//------------------------------------------------------------------------------
/**
*/
int 
Annotable::GetAnnotationInt(const eastl::string& name)
{
	assert(this->annotationMap.find(name) != this->annotationMap.end());
	return this->annotationMap[name].data.intValue;
}

//------------------------------------------------------------------------------
/**
*/
bool 
Annotable::GetAnnotationBool(const eastl::string& name)
{
	assert(this->annotationMap.find(name) != this->annotationMap.end());
	return this->annotationMap[name].data.boolValue;
}

//------------------------------------------------------------------------------
/**
*/
double 
Annotable::GetAnnotationDouble(const eastl::string& name)
{
	assert(this->annotationMap.find(name) != this->annotationMap.end());
	return this->annotationMap[name].data.doubleValue;
}

//------------------------------------------------------------------------------
/**
*/
float 
Annotable::GetAnnotationFloat(const eastl::string& name)
{
	assert(this->annotationMap.find(name) != this->annotationMap.end());
	return this->annotationMap[name].data.floatValue;
}

//------------------------------------------------------------------------------
/**
*/
const eastl::string&
Annotable::GetAnnotationString(const eastl::string& name)
{
	assert(this->annotationMap.find(name) != this->annotationMap.end());
	return *this->annotationMap[name].data.stringValue;
}
} // namespace AnyFX