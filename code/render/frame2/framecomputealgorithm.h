#pragma once
//------------------------------------------------------------------------------
/**
	Performs an algorithm used for computations.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "frameop.h"
#include "algorithm/algorithm.h"
namespace Frame2
{
class FrameComputeAlgorithm : public FrameOp
{
	__DeclareClass(FrameComputeAlgorithm);
public:
	/// constructor
	FrameComputeAlgorithm();
	/// destructor
	virtual ~FrameComputeAlgorithm();

	/// set algorithm
	void SetAlgorithm(const Ptr<Algorithms::Algorithm>& alg);
	/// add function to run
	void SetFunction(const Util::StringAtom& func);

	/// run operation
	void Run(const IndexT frameIndex);
private:
	Util::StringAtom func;
	Ptr<Algorithms::Algorithm> alg;
};

//------------------------------------------------------------------------------
/**
*/
inline void
FrameComputeAlgorithm::SetAlgorithm(const Ptr<Algorithms::Algorithm>& alg)
{
	this->alg = alg;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FrameComputeAlgorithm::SetFunction(const Util::StringAtom& func)
{
	n_assert(this->alg.isvalid());
	n_assert(this->alg->GetFunctionType(func) != Algorithms::Algorithm::Render);
	this->func = func;
}

} // namespace Frame2