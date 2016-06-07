#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FrameAlgorithm
    
    Encapsulates a render algorithm execution.
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "frame/framepassbase.h"
namespace Algorithm
{
	class AlgorithmBase;
}

namespace Frame
{
class FrameAlgorithm : public FramePassBase
{
	__DeclareClass(FrameAlgorithm);
public:
	/// constructor
	FrameAlgorithm();
	/// destructor
	virtual ~FrameAlgorithm();

	/// discard the post effect
	virtual void Discard();

	/// perform pre-frame stuff
	void Begin();
	/// render the post effect
    void Render(IndexT frameIndex);
	/// perform post-frame stuff
	void End();

	/// sets algorithm
	void SetAlgorithm(const Ptr<Algorithm::AlgorithmBase>& alg);
	/// gets algorithm
	const Ptr<Algorithm::AlgorithmBase>& GetAlgorithm() const;

	/// implement display resizing
	void OnWindowResize(SizeT width, SizeT height);

private:
	Util::String className;
	Ptr<Algorithm::AlgorithmBase> algorithm;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
FrameAlgorithm::SetAlgorithm( const Ptr<Algorithm::AlgorithmBase>& alg )
{
	this->algorithm = alg;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Algorithm::AlgorithmBase>& 
FrameAlgorithm::GetAlgorithm() const
{
	return this->algorithm;
}

} // namespace Frame
//------------------------------------------------------------------------------