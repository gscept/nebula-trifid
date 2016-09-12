#pragma once
//------------------------------------------------------------------------------
/**
	An algorithm is a frame script triggered piece of code which can run
	in sequences.

	An algorithm can be purely compute, in which case they may not execute within 
	a pass. They can be purely render, which means they must execute within
	a pass. They can also be mixed, in which case the right function has to 
	be executed within the correct scope. 

	An actual algorithm inherits this class and statically binds StringAtom objects
	with functions, and registers them with a certain type, which can be retrieved
	when performing frame script validation. 
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "util/stringatom.h"
#include <functional>
namespace Algorithms
{
class Algorithm : public Core::RefCounted
{
	__DeclareClass(Algorithm);
public:
	enum FunctionType
	{
		None,			// does neither rendering nor compute
		Render,			// algorithm does rendering only
		Compute			// algorithm does only computations
	};

	/// constructor
	Algorithm();
	/// destructor
	virtual ~Algorithm();
	
	/// setup algorithm
	virtual void Setup();
	/// discard operation
	virtual void Discard();
	/// run algorithm using string atom and type, type cannot be mixed when executing
	virtual void Execute(const Util::StringAtom& str, const IndexT& frameIndex);
	/// get type of function name
	const FunctionType& GetFunctionType(const Util::StringAtom& str);

protected:
	/// add algorithm
	void AddFunction(const Util::StringAtom& name, const FunctionType type, const std::function<void(Algorithm*)>& func);

	Util::Dictionary<Util::StringAtom, std::function<void(Algorithm*)>> functions;
	Util::Dictionary<Util::StringAtom, FunctionType> nameToType;
};

//------------------------------------------------------------------------------
/**
*/
inline const Algorithms::Algorithm::FunctionType&
Algorithm::GetFunctionType(const Util::StringAtom& str)
{
	return this->nameToType[str];
}

} // namespace Base