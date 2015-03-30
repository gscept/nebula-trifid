#ifndef SEQUENCER_BEZIERGRAPH_H
#define SEQUENCER_BEZIERGRAPH_H
//------------------------------------------------------------------------------
/**
    @class Sequencer::BezierGraph
    
    The BezierGraph is a piecewhise spline of bezier curves.
*/
#include "foundation/core/refcounted.h"
#include "sequencer/beziercurve.h"
#include "util/array.h"
#include "timing/time.h"

namespace Sequencer
{

//------------------------------------------------------------------------------
/**
    @enum Sequencer::Infinity 
    
    Defines behaviour outside the defined curve values.
*/
enum Infinity
{
    /// Cycle the curve
    Cycle,
    /// Cycle the curve but adapt cycle end/start values to fit to each other
    CycleWithOffset,
    /// Oscillate the curve (alternate forward, backwards)
    Oscillate,
    /// Continue in direction of last tangent
    Linear,
    /// Set to last value
    Constant
};
//------------------------------------------------------------------------------
/**
    @class Sequencer::BezierGraph
    
    Complete graph based on piecewhise bezier curves.
*/
class BezierGraph : public Core::RefCounted
{
    __DeclareClass(BezierGraph);
public:
    /// constructor
    BezierGraph();
    /// destructor
    virtual ~BezierGraph();
    /// add a bezier curve to graph.
    void AddBezierCurve(Ptr<BezierCurve> bezierCurve);
    /// get start point of curve
    /// Get the value of the bezier graph at a certain time
    /// In other words, get the y value from x value
    float EvaluateAtTime(Timing::Time time);

    void SetPreInfinity(const String& infString);
    void SetPostInfinity(const String& infString);

private:
	Util::Array<Ptr<BezierCurve> > curveList;
    Infinity postInfinity;
    Infinity preInfinity;
    String InfinityToString(Infinity infinity);
    Infinity InfinityFromString(String infinity);
};
//------------------------------------------------------------------------------
/**
*/
inline void BezierGraph::AddBezierCurve(Ptr<BezierCurve> bezierCurve)
{
    curveList.Append(bezierCurve);
}
} // namespace Sequencer
//------------------------------------------------------------------------------
#endif

