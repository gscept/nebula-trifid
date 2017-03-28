#pragma once
//------------------------------------------------------------------------------
/**
	Implements the Preetham analytical sky model for atmospheric scattering effects.

	Source: http://www.cs.utah.edu/~shirley/papers/sunsky/sunsky.pdf
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "math/vector.h"
namespace PostEffect
{
class PreethamUtil
{
public:

	/// compute preetham coefficients from global light direction and turbidity
	static void Calculate(float turbidity, const Math::vector& globalLightDir, const Math::vector& viewVec, Math::vector& A, Math::vector& B, Math::vector& C, Math::vector& D, Math::vector& E, Math::vector& Z);

private:

	/// calculate distribution coefficients from turbidity
	static void CalculateCoefficients(float turbidity, Math::vector& paramA, Math::vector& paramB, Math::vector& paramC, Math::vector& paramD, Math::vector& paramE);
	/// calculate using Perez using contribution coefficients
	static float Perez(float globalLightTheta, float gamma, float paramA, float paramB, float paramC, float paramD, float paramE);
	/// calculate zenith luminance (luminance Y)
	static float ZenithLuminance(float turbidity, float globalLightTheta);
	/// calculate zenith chromacities (chromacities x and y)
	static float ZenithChromacity(float turbidity, float globalLightTheta, const Math::float4& row1, const Math::float4& row2, const Math::float4& row3);
};
} // namespace PostEffect