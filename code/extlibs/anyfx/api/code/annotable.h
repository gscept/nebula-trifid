#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::Annotable
    
    Classes who inherit from annotable can have annotations.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "vartypes.h"
#include <string>
#include <map>
#include "EASTL/vector.h"
namespace AnyFX
{
class Annotable
{
public:
	/// constructor
	Annotable();
	/// destructor
	virtual ~Annotable();

	/// get int value
	int GetAnnotationInt(const std::string& name);
	/// get bool value
	bool GetAnnotationBool(const std::string& name);
	/// get double value
	double GetAnnotationDouble(const std::string& name);
	/// get float value
	float GetAnnotationFloat(const std::string& name);
	/// get string value
	const std::string& GetAnnotationString(const std::string& name);

private:
	friend class EffectAnnotationStreamLoader;

	struct AnnotationVariant
	{
		union
		{
			int intValue;
			bool boolValue;
			double doubleValue;
			float floatValue;
			std::string* stringValue;
		} data;

		VariableType type;
	};

	eastl::vector<VariableType> annotationTypes;
	std::map<std::string, AnnotationVariant> annotationMap;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------