#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::Annotable
    
    Classes who inherit from annotable can have annotations.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "vartypes.h"
#include "EASTL/string.h"
#include "EASTL/map.h"
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

    /// returns true if annotation exists
    bool HasAnnotation(const eastl::string& name);

	/// get int value
    int GetAnnotationInt(const eastl::string& name);
	/// get bool value
    bool GetAnnotationBool(const eastl::string& name);
	/// get double value
    double GetAnnotationDouble(const eastl::string& name);
	/// get float value
    float GetAnnotationFloat(const eastl::string& name);
	/// get string value
    const eastl::string& GetAnnotationString(const eastl::string& name);

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
            eastl::string* stringValue;
		} data;

		VariableType type;
	};

	eastl::vector<VariableType> annotationTypes;
    eastl::map<eastl::string, AnnotationVariant> annotationMap;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------