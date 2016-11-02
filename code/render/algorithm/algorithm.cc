//------------------------------------------------------------------------------
// algorithm.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "algorithm.h"

namespace Algorithms
{

__ImplementClass(Algorithms::Algorithm, 'ALBA', Core::RefCounted);
//------------------------------------------------------------------------------
/**
*/
Algorithm::Algorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Algorithm::~Algorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
Algorithm::Execute(const Util::StringAtom& str, const IndexT& frameIndex)
{
	// get function and run
	std::function<void(Algorithm* alg)> func = this->functions[str];
	func(this);
}

//------------------------------------------------------------------------------
/**
*/
void
Algorithm::AddFunction(const Util::StringAtom& name, const FunctionType type, const std::function<void(Algorithm*)>& func)
{
	this->nameToType.Add(name, type);
	this->functions.Add(name, func);
}

//------------------------------------------------------------------------------
/**
*/
void
Algorithm::Discard()
{
	this->nameToType.Clear();
	this->functions.Clear();
	// override in subclass for any special discard behavior
}

} // namespace Base