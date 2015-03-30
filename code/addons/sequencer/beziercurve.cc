//------------------------------------------------------------------------------
//  sequencer/beziercurve.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "sequencer/beziercurve.h"
#include "sequencer/cubicequationsolver.h"

namespace Sequencer
{
__ImplementClass(Sequencer::BezierCurve, 'BCUR', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
BezierCurve::BezierCurve()
{
    this->points.SetSize(4);
}

//------------------------------------------------------------------------------
/**
*/
BezierCurve::~BezierCurve()
{
}
    
//------------------------------------------------------------------------------
/**
    calculate the bezier co-efficients from bezier points
*/
void
BezierCurve::CalculatePolynome()
{
    a.x() = -points[0].x() + 3 * points[1].x() - 3 * points[2].x() + points[3].x();
    a.y() = -points[0].y() + 3 * points[1].y() - 3 * points[2].y() + points[3].y();

    b.x() = 3 * points[0].x() - 6 * points[1].x() + 3 * points[2].x();
    b.y() = 3 * points[0].y() - 6 * points[1].y() + 3 * points[2].y();

    c.x() = -3 * points[0].x() + 3 * points[1].x();
    c.y() = -3 * points[0].y() + 3 * points[1].y();

    d.x() = points[0].x();
    d.y() = points[0].y();
}

//------------------------------------------------------------------------------
/**
    Get the value of the bezier curve at a certain time.
    In other words, get the y value from x value.
    Usually bezier splines are evaluted using de Casteljau algortihm. This gives
    you the x and y value of the curve at the parametric position of t which is
    between 0 and 1.
    For keyframe base animation, we are not intereseted about parameter t, but
    we have to get the value according to a time. Because the x axis out timeline,
    we have to find the y value from an x value.
    Accordnding to the maya manual, this is done by:

    When evaluating a time within a segment, the following algortithms are used:

    For weighted tangents:
	    where x is the start of the segment
	    given the bezier x parameters a', b', c', d', find the parameter t 
	    which satisfies the formula:
		    (time - x) = (t^3 * a') + (t^2 + b') + (t * c') + d'
	    with t (and the bezier y parameters a, b, c, d) compute the value as:
		    v = (t^3 * a) + (t^2 + b) + (t * c) + d
*/
float
BezierCurve::EvaluateAtTime(Timing::Time time)
{
    double t0, t1, t2;
    // testdata - compare with webpage
    //this.SolveCubic(2, -4, -22, 24, out solution, out t0, out t1, out t2);
    //this.SolveCubic(3, -10, 14, 27, out solution, out t0, out t1, out t2);
    //this.SolveCubic(1, 6, 12, 8, out solution, out t0, out t1, out t2);

    // get t parameter
    CubicSolution solution =
        CubicEquationSolver::SolveEquation(a.x(), b.x(), c.x(), d.x() - time);
    t0 = solution.t0;
    t1 = solution.t1;
    t2 = solution.t2;

    if (solution.cubicSolutionType == THREE_SOLUTION)
    {
        // find the fitting t and set as t0
        double findValue;
        if ((t0 >= 0) && (t0 <= 1)) findValue = t0;
        else if ((t1 >= 0) && (t1 <= 1)) findValue = t1;
        else if ((t2 >= 0) && (t2 <= 1)) findValue = t2;
        // this case gets nasty
        // we have a round error, so that the value of any t is
        // either a little below zero or above 1 - have to find which one
        // code looks ugly... but didn't find more elegant way
        else
        {
            double delta;
            double minDelta;
            minDelta = abs(0 - t0);
            findValue=0;
            delta = abs(1 - t0);
            if (delta < minDelta)
            {
                minDelta = delta;
                findValue = 1;
            }
            delta = abs(0 - t1);
            if (delta < minDelta)
            {
                minDelta = delta;
                findValue = 0;
            }
            delta = abs(1 - t1);
            if (delta < minDelta)
            {
                minDelta = delta;
                findValue = 1;
            }
            delta = abs(0 - t2);
            if (delta < minDelta)
            {
                minDelta = delta;
                findValue = 0;
            }
            delta = abs(1 - t2);
            if (delta < minDelta)
            {
                minDelta = delta;
                findValue = 1;
            }
        }
        t0 = findValue;
    }

    // solve polynome equation with t for y value
    double yValue = a.y() * t0 * t0 * t0 + b.y() * t0 * t0 + c.y() * t0 + d.y();

    return (float)yValue;
}

}  // namespace Sequencer