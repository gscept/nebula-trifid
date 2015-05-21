#pragma once
//------------------------------------------------------------------------------
/**
	@class GraphicsFeature::AnimPath
	
	Allows a user to build a path using a template function 
	as the type of data which the path should modify.
	Each curve is added with a duration which describes the time each curve should take to play.

	The type being used with the PathAnim class only needs to define the plus, minus and multiplication operators.

	This class is used by the PathAnimation resource, but can be used by itself too. 
	AnimPaths can be relative to the original 'state' of a variable by using the Begin/End APIs,
	or simply as an animated offset.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

// optimization macros which allows us to bypass pow since we don't actually need to use a dynamic exponent
#define POW3(x) (x)*(x)*(x)
#define POW2(x) (x)*(x)

#include "core/refcounted.h"
#include "coreanimation/infinitytype.h"
namespace GraphicsFeature
{
template <class TYPE>
class AnimPath
{
public:
	/// constructor
	AnimPath();
	/// destructor
	virtual ~AnimPath();

	/// cleanup anim path
	void Cleanup();

	/// set infinity type
	void SetInfinity(const CoreAnimation::InfinityType::Code& infinity);

	/// begin a path, this is required if the path needs a starting point; it's optional to use this, but without it, the start value will be 0
	void Begin(const TYPE& start);
	/// create cubic curve to point (uses two reference points)
	void CubicTo(const TYPE& to, const TYPE& c0, const TYPE& c1, const float start, const float end);
	/// create bezier curve to point (uses one reference point)
	void QuadraticTo(const TYPE& to, const TYPE& c0, const float start, const float end);
	/// create linear curve to point
	void LinearTo(const TYPE& to, const float start, const float end);
	/// create a linear curve back to the start, this is basically a shortcut to doing LinearTo back to the start value; it's optional to use this
	void End(const float duration = 0.0f);

	/// get running time in seconds
	const float GetLength() const;
	/// get sample from path, using a sample point which is somewhere between [0..totalLength] where length is the total length of the path
	TYPE Sample(float sample);
private:

	struct Curve
	{
		TYPE target;
		TYPE start;
		float from;
		float to;
		float duration;

		/// sampling function for getting the LOCAL sample, meaning the sample must be between [0..duration]
		virtual TYPE Sample(float time) const = 0;
	};

	struct CubicCurve : public Curve
	{
		TYPE c0;
		TYPE c1;
		TYPE Sample(float time) const
		{
			time /= this->duration;
			float v0 = POW3(1 - time);
			float v1 = 3 * POW2(1 - time) * time;
			float v2 = 3 * (1 - time) * POW2(time);
			float v3 = POW3(time);
			return this->start * v0 + this->c0 * v1 + this->c1 * v2 + this->target * v3;
		}
	};

	struct QuadraticCurve : public Curve
	{
		TYPE c0;
		TYPE Sample(float time) const
		{
			time /= this->duration;
			float v0 = POW2(1 - time);
			float v1 = 2 * (1 - time) * time;
			float v2 = POW2(time);
			return this->start * v0 + this->c0 * v1 + this->target * v2;
		}
	};
	
	struct LinearCurve : public Curve
	{
		TYPE Sample(float time) const
		{
			time /= this->duration;
			return this->start + (this->target - this->start) * time;
		}
	};

	/// helper function which generalizes the setup of curves with the parameters they all have in common
	void SetupCurve(const TYPE& to, const float start, const float end, Curve* curve);

	float totalLength;
	TYPE start;
	CoreAnimation::InfinityType::Code infinity;
	Util::Array<Curve*> curves;
};

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
GraphicsFeature::AnimPath<TYPE>::AnimPath() :
	totalLength(0.0f),	
	start(TYPE(0)),
	infinity(CoreAnimation::InfinityType::Constant)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
GraphicsFeature::AnimPath<TYPE>::~AnimPath()
{
	this->Cleanup();
}

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
void
GraphicsFeature::AnimPath<TYPE>::Cleanup()
{
	IndexT i;
	for (i = 0; i < this->curves.Size(); i++)
	{
		n_delete(this->curves[i]);
	}
	this->curves.Clear();
}

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
TYPE
GraphicsFeature::AnimPath<TYPE>::Sample(float sample)
{

	// if we have a cycling animation, we should just go back to the start
	if (this->infinity == CoreAnimation::InfinityType::Cycle) sample = Math::n_fmod(sample, this->totalLength);
	else													  sample = Math::n_min(sample, this->totalLength);
	
	bool inbetween = false;
	IndexT i;
	
	// this will be the value we return, initiate it to the start value (which will be the value 
	TYPE retval = this->start;
	
	// shortcut if we are at the last frame
	
	if (this->curves.Size() == 0)	goto quickexit;
	else if (sample == this->totalLength)
	{
		retval = this->curves.Back()->target;
		goto quickexit;
	}

	// find which curve our sample lies in	
	for (i = 0; i < this->curves.Size(); i++)
	{
		Curve* curve = this->curves[i];

		// in first curve
		if (sample >= curve->from && sample < curve->to)
		{
			retval = curve->Sample(sample - curve->from);
			break;
		}
		else if (i > 0)
		{
			// check if we are between samples
			Curve* prev = this->curves[i - 1];
			if (sample >= prev->to && sample < curve->from)
			{
				retval = prev->target;
				break;
			}
		}
	}

quickexit:
	// time is the total time offset from the start to the beginning of the current sample
	return retval;
}

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
inline void
GraphicsFeature::AnimPath<TYPE>::SetupCurve(const TYPE& to, const float start, const float end, Curve* curve)
{
	n_assert(start <= end);
	float duration = end - start;
	this->totalLength = Math::n_max(end, this->totalLength);

	// if this is the first curve, then it should start at 'start', otherwise it should continue from the next curve
	if (this->curves.IsEmpty()) curve->start = this->start;
	else						curve->start = this->curves.Back()->target;
	curve->from = start;
	curve->to = end;
	curve->duration = duration;
	curve->target = to;
	this->curves.Append(curve);
}

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
inline void
GraphicsFeature::AnimPath<TYPE>::Begin(const TYPE& start)
{
	// change the starting point of the first curve
	if (!this->curves.IsEmpty()) this->curves.Front()->start = start;
	this->start = start;
}

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
void
GraphicsFeature::AnimPath<TYPE>::SetInfinity(const CoreAnimation::InfinityType::Code& infinity)
{
	this->infinity = infinity;
}

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
inline void
GraphicsFeature::AnimPath<TYPE>::CubicTo(const TYPE& to, const TYPE& c0, const TYPE& c1, const float start, const float end)
{
	CubicCurve* curve = n_new(CubicCurve);
	curve->c0 = c0;
	curve->c1 = c1;
	this->SetupCurve(to, start, end, curve);
}

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
inline void
GraphicsFeature::AnimPath<TYPE>::QuadraticTo(const TYPE& to, const TYPE& c0, const float start, const float end)
{
	QuadraticCurve* curve = n_new(QuadraticCurve);
	curve->c0 = c0;
	this->SetupCurve(to, start, end, curve);
}

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
inline void
GraphicsFeature::AnimPath<TYPE>::LinearTo(const TYPE& to, const float start, const float end)
{
	LinearCurve* curve = n_new(LinearCurve);
	this->SetupCurve(to, start, end, curve);
}

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
void
GraphicsFeature::AnimPath<TYPE>::End(const float duration)
{
	LinearCurve* curve = n_new(LinearCurve);
	this->SetupCurve(this->start, this->totalLength, this->totalLength + duration, curve);
}

//------------------------------------------------------------------------------
/**
*/
template <class TYPE>
inline const float
GraphicsFeature::AnimPath<TYPE>::GetLength() const
{
	return this->totalLength;
}

} // namespace GraphicsFeature