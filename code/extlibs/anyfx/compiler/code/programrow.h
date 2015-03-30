#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::ProgramRow
    
    A program row denotes a single semi-colon terminated row in a program definition.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <string>
#include <map>
namespace AnyFX
{
class ProgramRow
{
public:

	enum StringType
	{
		VertexShader,
		PixelShader,
		GeometryShader,
		HullShader,
		DomainShader,
		ComputeShader,
		RenderState,        // ALWAYS KEEP AT THE END!  (necessary in program.cc where we check for subroutine bindings)

		NumProgramRows
	};

	/// constructor
	ProgramRow();
	/// destructor
	virtual ~ProgramRow();

	/// get flag type
	const std::string& GetFlag() const;

	/// set string flag
	void SetString(const std::string& flag, const std::string& value);
	/// get string flag
	const std::string& GetString() const;

    /// set subroutine binding
    void SetSubroutineMapping(const std::string& var, const std::string& implementation);
    /// get subroutine bindings
    const std::map<std::string, std::string>& GetSubroutineMappings() const;

private:
	std::string flag;
	std::string stringValue;
    std::map<std::string, std::string> subroutineMappings;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
ProgramRow::GetFlag() const
{
	return this->flag;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
ProgramRow::GetString() const
{
	return this->stringValue;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::map<std::string, std::string>&
ProgramRow::GetSubroutineMappings() const
{
    return this->subroutineMappings;
}

} // namespace AnyFX
//------------------------------------------------------------------------------