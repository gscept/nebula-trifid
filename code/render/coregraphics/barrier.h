#pragma once
//------------------------------------------------------------------------------
/**
	A barrier contains information about a GPU execution barrier, which is used to
	sequence two dependent commands.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
namespace CoreGraphics
{
class RenderTexture;
class Barrier
{
public:

	enum BarrierUsage
	{
		Subpass,			// use in subpass to ensure a previous call within a subpass is finished
		TransformFeedback,	// wait for vertex shader to finish, and vertex input to begin
		ComputeImage		// wait for compute to finish, and fragment shader to begin
	};

	/// constructor
	Barrier();
	/// destructor
	virtual ~Barrier();

	/// set barrier usage
	void SetUsage(const BarrierUsage usage);
	/// get barrier usage
	const BarrierUsage GetUsage() const;
	/// add image to use in barrier
	void AddTexture(const Ptr<CoreGraphics::RenderTexture>& tex);
private:

	BarrierUsage usage;
	Util::Array<Ptr<CoreGraphics::RenderTexture>> images;
};


//------------------------------------------------------------------------------
/**
*/
inline
Barrier::Barrier()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
inline 
Barrier::~Barrier()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
inline void
Barrier::SetUsage(const BarrierUsage usage)
{
	this->usage = usage;
}

//------------------------------------------------------------------------------
/**
*/
inline const CoreGraphics::Barrier::BarrierUsage
Barrier::GetUsage() const
{
	return this->usage;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Barrier::AddTexture(const Ptr<CoreGraphics::RenderTexture>& tex)
{
	this->images.Append(tex);
}

} // namespace CoreGraphics