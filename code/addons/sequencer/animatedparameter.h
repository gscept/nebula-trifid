#ifndef SEQUENCER_ANIMATEDPARAMETER_H
#define SEQUENCER_ANIMATEDPARAMETER_H
//------------------------------------------------------------------------------
/**
    @class Sequencer::AnimatedParameter
    
    The animated parameter class is a template class for animating
    parameters inside the sequencer. The parameter either returns a fixed
    value or evaluates an animation graph depending on its state.
*/
#include "foundation/core/refcounted.h"
#include "util/string.h"
#include "util/array.h"
#include "util/fixedarray.h"
#include "sequencer/beziergraph.h"
#include "math/point.h"

using namespace Math;

namespace Sequencer
{

//------------------------------------------------------------------------------
/**
    @class Sequencer::AnimatedParameter<T>
    
    General template class, just set and get a fixed value of any type.
*/
template <class T>
class AnimatedParameter
{
public:
    /// constructor
    AnimatedParameter()
    {
    }
    /// destructor
    virtual ~AnimatedParameter()
    {
    }
    /// set value of type T
    void SetValue(T value) const
    {
        this.value = value;
    }
    /// get value of type T
    T GetValue(float time)
    {
        return value;
    }

protected:
    T value;
    bool isAnimated;
};


//------------------------------------------------------------------------------
/**
    @class Sequencer::AnimatedParameter<float>
    
    Template specification for float
*/
template <>
class AnimatedParameter<float>
{
public:
    /// constructor
    AnimatedParameter() : isAnimated(false), bezierGraph(0)
    {
        isAnimated = false;
        bezierGraph = 0;
    }
    /// destructor
    virtual ~AnimatedParameter()
    {
    }
    /// returns float value at time
    float GetValue(float time)
    {
        if (!isAnimated)
        {
            return value;
        }
        else
        {
            n_assert(bezierGraph.isvalid());
            return bezierGraph->EvaluateAtTime(time);
        }
    }
    /// get the fixed value
    float GetFixedValue()
    {
        return value;
    }
    /// set a fixed float value
    void SetValue(float f)
    {
        value = f;
    }
    /// set the bezier graph for animation of value
    void SetGraph(Ptr<BezierGraph> graph)
    {
        bezierGraph = graph;
    }
    /// set the value state animated
    void SetAnimated(bool animated)
    {
        isAnimated = animated;
    }
    /// get the animation state
    bool GetAnimated()
    {
        return isAnimated;
    }

protected:
    /// fixed float value for not animated state
    float value;
    /// bezier graph for animating value
    Ptr<BezierGraph> bezierGraph;
    /// true, if parameter is animated
    bool isAnimated;
};

//------------------------------------------------------------------------------
/**
    @class Sequencer::AnimatedParameter
    
    Template specification for Point
*/
template <>
class AnimatedParameter<point>
{
public:
    /// constructor
    AnimatedParameter()
    {
        isAnimated = false;
        animFloat.SetSize(3);
    }
    /// destructor
    virtual ~AnimatedParameter()
    {
    }
    /// set a comonent of vector animated
    /// each vector value can be set animated or fixed seperately
    void SetAnimated(int i, bool animated)
    {
        n_assert((i>=0)&&(i<3));
        animFloat[i].SetAnimated(animated);
    }
    /// get animation state of vector. animation is true
    /// if at least on vector component is animated.
    bool GetAnimated()
    {
        return (animFloat[0].GetAnimated() 
            ||  animFloat[1].GetAnimated() 
            ||  animFloat[2].GetAnimated());
    }
    /// get the animation state of each component seperately
    bool GetAnimated(int i)
    {
        n_assert((i>=0)&&(i<3));
        return animFloat[i].GetAnimated();
    }
    /// set the vector fixed value
    void SetValue(point value)
    {
        animFloat[0].SetValue(value.x());
        animFloat[1].SetValue(value.y());
        animFloat[2].SetValue(value.z());
    }
    /// set a bezier animation graph for a vector
    /// component
    void SetGraph(int i, Ptr<BezierGraph> bezierGraph)
    {
        n_assert((i>=0)&&(i<3));
        animFloat[i].SetGraph(bezierGraph);
    }
    /// get value of vector at time
    const point GetValue(float time)
    {
        point value;
        if(animFloat[0].GetAnimated())
        {
            value.x() = animFloat[0].GetValue(time);
        }
        else
        {
            value.x() = animFloat[0].GetFixedValue();
        }
        if(animFloat[1].GetAnimated())
        {
            value.y() = animFloat[1].GetValue(time);
        }
        else
        {
            value.y() = animFloat[1].GetFixedValue();
        }
        if(animFloat[2].GetAnimated())
        {
            value.z() = animFloat[2].GetValue(time);
        }
        else
        {
            value.z() = animFloat[2].GetFixedValue();
        }
        return value;
    }
    
    /// get fixed values
    const point GetFixedValue()
    {
        point value;
        value.set(	animFloat[0].GetFixedValue(),
        			animFloat[1].GetFixedValue(),
        			animFloat[2].GetFixedValue());
        return value;
    }

    /// get access to the inner storage of values. each
    /// vector component is a float animated parameter.
    /// get this parameter directly for more easy
    /// initialization.
    AnimatedParameter<float> &GetFloat(int i)
    {
        n_assert((i>=0)&&(i<3));
        return animFloat[i];
    }

protected:
    /// vector is stored as size 3 array of float animated parameter
    FixedArray<AnimatedParameter<float> > animFloat;
    bool isAnimated;
};

//------------------------------------------------------------------------------
/**
    @class Sequencer::AnimatedParameter
    
    Template specification for Point
*/
template <>
class AnimatedParameter<vector>
{
public:
    /// constructor
    AnimatedParameter()
    {
        isAnimated = false;
        animFloat.SetSize(3);
    }
    /// destructor
    virtual ~AnimatedParameter()
    {
    }
    /// set a comonent of vector animated
    /// each vector value can be set animated or fixed seperately
    void SetAnimated(int i, bool animated)
    {
        n_assert((i>=0)&&(i<3));
        animFloat[i].SetAnimated(animated);
    }
    /// get animation state of vector. animation is true
    /// if at least on vector component is animated.
    bool GetAnimated()
    {
        return (animFloat[0].GetAnimated() 
            ||  animFloat[1].GetAnimated() 
            ||  animFloat[2].GetAnimated());
    }
    /// get the animation state of each component seperately
    bool GetAnimated(int i)
    {
        n_assert((i>=0)&&(i<3));
        return animFloat[i].GetAnimated();
    }
    /// set the vector fixed value
    void SetValue(vector value)
    {
        animFloat[0].SetValue(value.x());
        animFloat[1].SetValue(value.y());
        animFloat[2].SetValue(value.z());
    }
    /// set a bezier animation graph for a vector
    /// component
    void SetGraph(int i, Ptr<BezierGraph> bezierGraph)
    {
        n_assert((i>=0)&&(i<3));
        animFloat[i].SetGraph(bezierGraph);
    }
    /// get value of vector at time
    const vector GetValue(float time)
    {
        point value;
        if(animFloat[0].GetAnimated())
        {
            value.x() = animFloat[0].GetValue(time);
        }
        else
        {
            value.x() = animFloat[0].GetFixedValue();
        }
        if(animFloat[1].GetAnimated())
        {
            value.y() = animFloat[1].GetValue(time);
        }
        else
        {
            value.y() = animFloat[1].GetFixedValue();
        }
        if(animFloat[2].GetAnimated())
        {
            value.z() = animFloat[2].GetValue(time);
        }
        else
        {
            value.z() = animFloat[2].GetFixedValue();
        }
        return value;
    }
    
    /// get fixed values
    const point GetFixedValue()
    {
        point value;
        value.set(	animFloat[0].GetFixedValue(),
        			animFloat[1].GetFixedValue(),
        			animFloat[2].GetFixedValue());
        return value;
    }

    /// get access to the inner storage of values. each
    /// vector component is a float animated parameter.
    /// get this parameter directly for more easy
    /// initialization.
    AnimatedParameter<float> &GetFloat(int i)
    {
        n_assert((i>=0)&&(i<3));
        return animFloat[i];
    }

protected:
    /// vector is stored as size 3 array of float animated parameter
    FixedArray<AnimatedParameter<float> > animFloat;
    bool isAnimated;
};

//------------------------------------------------------------------------------
/**
@class Sequencer::AnimatedParameter

Template specification for vector4
*/
template <>
class AnimatedParameter<float4>
{
public:
    /// constructor
    AnimatedParameter()
    {
        isAnimated = false;
        animFloat.SetSize(4);
    }
    /// destructor
    virtual ~AnimatedParameter()
    {
    }
    /// set a comonent of vector animated
    /// each vector value can be set animated or fixed seperately
    void SetAnimated(int i, bool animated)
    {
        n_assert((i>=0)&&(i<4));
        animFloat[i].SetAnimated(animated);
    }
    /// get animation state of vector. animation is true
    /// if at least on vector component is animated.
    bool GetAnimated()
    {
        return (animFloat[0].GetAnimated() 
            ||  animFloat[1].GetAnimated() 
            ||  animFloat[2].GetAnimated()
            ||  animFloat[3].GetAnimated());
    }
    /// get the animation state of each component seperately
    bool GetAnimated(int i)
    {
        n_assert((i>=0)&&(i<4));
        return animFloat[i].GetAnimated();
    }
    /// set the vector fixed value
    void SetValue(float4 value)
    {
        animFloat[0].SetValue(value.x());
        animFloat[1].SetValue(value.y());
        animFloat[2].SetValue(value.z());
        animFloat[2].SetValue(value.w());
    }
    /// set a bezier animation graph for a vector
    /// component
    void SetGraph(int i, Ptr<BezierGraph> bezierGraph)
    {
        n_assert((i>=0)&&(i<4));
        animFloat[i].SetGraph(bezierGraph);
    }
    /// get value of vector at time
    const float4 GetValue(float time)
    {
        float4 value;
        if(animFloat[0].GetAnimated())
        {
            value.x() = animFloat[0].GetValue(time);
        }
        else
        {
            value.x() = animFloat[0].GetFixedValue();
        }
        if(animFloat[1].GetAnimated())
        {
            value.y() = animFloat[1].GetValue(time);
        }
        else
        {
            value.y() = animFloat[1].GetFixedValue();
        }
        if(animFloat[2].GetAnimated())
        {
            value.z() = animFloat[2].GetValue(time);
        }
        else
        {
            value.z() = animFloat[2].GetFixedValue();
        }
        if(animFloat[3].GetAnimated())
        {
            value.w() = animFloat[3].GetValue(time);
        }
        else
        {
            value.w() = animFloat[3].GetFixedValue();
        }
        return value;
    }

    /// get fixed values
    const float4 GetFixedValue()
    {
        float4 value;
        value.x() = animFloat[0].GetFixedValue();
        value.y() = animFloat[1].GetFixedValue();
        value.z() = animFloat[2].GetFixedValue();
        value.w() = animFloat[3].GetFixedValue();
        return value;
    }

    /// get access to the inner storage of values. each
    /// vector component is a float animated parameter.
    /// get this parameter directly for more easy
    /// initialization.
    AnimatedParameter<float> &GetFloat(int i)
    {
        n_assert((i>=0)&&(i<4));
        return animFloat[i];
    }

protected:
    /// vector is stored as size 3 array of float animated parameter
    FixedArray<AnimatedParameter<float> > animFloat;
    bool isAnimated;
};

} // namespace Sequencer
//------------------------------------------------------------------------------
#endif