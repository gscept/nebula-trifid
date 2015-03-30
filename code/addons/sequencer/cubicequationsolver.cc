//------------------------------------------------------------------------------
//  sequencer/cubicequationsolver.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "sequencer/cubicequationsolver.h"

namespace Sequencer
{
__ImplementClass(Sequencer::CubicEquationSolver, 'CESV', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
CubicEquationSolver::CubicEquationSolver()
{
}

//------------------------------------------------------------------------------
/**
*/
CubicEquationSolver::~CubicEquationSolver()
{
}

//------------------------------------------------------------------------------
/**
    Get the third (cubic?) root of a number
*/
double CubicEquationSolver::Root3(double value)
{
    if (value < 0)
    {
        return -pow(abs(value), 1.0 / 3.0);
    }
    return pow(value, 1.0 / 3.0);
}

//------------------------------------------------------------------------------
/**
    Solve a cubic equation by the given input parameters
*/
CubicSolution CubicEquationSolver::SolveEquation(
    double a, double b, double c, double d)
{
    CubicSolution solution;
    double f = ((3 * c / a) - (b * b / (a * a))) / 3;
    double g = ((2 * b * b * b / (a * a * a)) - (9 * b * c / (a * a)) + (27 * d / a)) / 27;
    double h = (g * g / 4) + (f * f * f / 27);

    if ((h == 0) && (g == 0) && (h == 0))
    {
        solution.cubicSolutionType = THREE_SAME_SOLUTION;
        solution.t0 = Root3(d / a) * -1;
        solution.t1 = solution.t0;
        solution.t2 = solution.t0;
    }
    if (h > 0)
    {
        solution.cubicSolutionType = ONE_SOLUTION;
        double r = -(g/2.0) + sqrt(h);
        double s = Root3(r);
        double t = -(g/2.0) - sqrt(h);
        double u = Root3(t);
        solution.t0 = s + u - (b / (3.0 * a));
        // t1 and t2 are note defined, set to zero
        solution.t1 = 0;
        solution.t2 = 0;
    }
    else
    {
        solution.cubicSolutionType = THREE_SOLUTION;
        double i = sqrt((g*g/4.0)-h);
        double j = Root3(i);
        double k = acos(-(g/(2*i)));
        double l = j * -1;
        double m = cos(k/3);
        double n = sqrt(3.0) * sin(k/3.0);
        double p = (b/(3*a))*-1;
        solution.t0 = 2*j*cos(k/3.0) - (b/(3*a));
        solution.t1 = l*(m+n)+p;
        solution.t2 = l*(m-n)+p;
    }
    return solution;
}


}  // namespace Sequencer