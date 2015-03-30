#ifndef SEQUENCER_CUBICEQUATIONSOLVER_H
#define SEQUENCER_CUBICEQUATIONSOLVER_H
//------------------------------------------------------------------------------
/**
    @class Sequencer::CubicEquationSolver
    
    The Cubic Equation Solver is used to solve a polynome of degree 3.
*/
#include "foundation/core/refcounted.h"

namespace Sequencer
{
/// The solution to a cubic equation has three case. There exists only one real solution,
/// there exist three  real soltuions or all three solutions have the same number
enum CubicSolutionCase
{
    ONE_SOLUTION,
    THREE_SOLUTION,
    THREE_SAME_SOLUTION
};
/// Structure containing the result of the solution to a cubic equation
struct CubicSolution
{
    double t0;
    double t1;
    double t2;
    CubicSolutionCase cubicSolutionType;
};

class CubicEquationSolver : public Core::RefCounted
{
    __DeclareClass(CubicEquationSolver);
public:
    /// constructor
    CubicEquationSolver();
    /// destructor
    virtual ~CubicEquationSolver();
    /// Solve a cubic equation by the given input parameters
    static CubicSolution SolveEquation(double a, double b, double c, double d);
    ///  Get the third (cubic?) root of a number
    static double Root3(double value);

private:
};

} // namespace Sequencer
//------------------------------------------------------------------------------
#endif

