//------------------------------------------------------------------------------
// preethamutil.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "preethamutil.h"
#include "math/scalar.h"
#include "math/point.h"

using namespace Math;
namespace PostEffect
{

//------------------------------------------------------------------------------
/**
*/
void
PreethamUtil::Calculate(float turbidity, const Math::vector& globalLightDir, const Math::vector& viewVec, Math::vector& A, Math::vector& B, Math::vector& C, Math::vector& D, Math::vector& E, Math::vector& Z)
{
	Math::point sunPos = point(0) + globalLightDir;
	//float gamma = vector::dot3(-globalLightDir, viewVec);
	//float theta = n_acos(dir.z() / dir.length());
	//float theta = n_clamp(n_acos(vector::dot3(globalLightDir, vector::upvec())) , 0, N_PI_HALF + N_PI_HALF / 8);
	//float theta = n_acos(vector::dot3(globalLightDir, vector::upvec()));
	float theta = n_acos(sunPos.y() / sunPos.length());

	// calculate coefficents
	PreethamUtil::CalculateCoefficients(turbidity, A, B, C, D, E);

	// calculate zenith parameters
	Z.x() = PreethamUtil::ZenithChromacity(turbidity, theta, float4(0.00166f, -0.00375f, 0.00209f, 0), float4(-0.02903f, 0.06377f, -0.03202f, 0.00394f), float4(0.11693f, -0.21196f, 0.06052f, 0.25886f));
	Z.y() = PreethamUtil::ZenithChromacity(turbidity, theta, float4(0.00275f, -0.00610f, 0.00317f, 0), float4(-0.04214f, 0.08970f, -0.04153f, 0.00516f), float4(0.15346f, -0.26756f, 0.06670f, 0.26688f));
	Z.z() = PreethamUtil::ZenithLuminance(turbidity, theta);// / 1.1f * PreethamUtil::Perez(theta, 0, A.z(), B.z(), C.z(), D.z(), E.z());
	
	Z.x() /= PreethamUtil::Perez(0, theta, A.x(), B.x(), C.x(), D.x(), E.x());
	Z.y() /= PreethamUtil::Perez(0, theta, A.y(), B.y(), C.y(), D.y(), E.y());

	// scale luminance to a sane value, and ensure it's above absolute 0
	Z.z() /= 1.1f *  turbidity / PreethamUtil::Perez(0, theta, A.z(), B.z(), C.z(), D.z(), E.z());
	Z.z() = n_max(0.0001f, Z.z());

	// done!
}

//------------------------------------------------------------------------------
/**
*/
void
PreethamUtil::CalculateCoefficients(float turbidity, Math::vector& paramA, Math::vector& paramB, Math::vector& paramC, Math::vector& paramD, Math::vector& paramE)
{
	paramA = vector(-0.0193, -0.0167, 0.1787) * turbidity +		vector(-0.2592, -0.2608, -1.4630);
	paramB = vector(-0.0665, -0.0950, -0.3554) * turbidity +	vector(0.0008, 0.0092, 0.4275);
	paramC = vector(-0.0004, -0.0079, -0.0227) * turbidity +	vector(0.2125, 0.2102, 5.3251);
	paramD = vector(-0.0641, -0.0441, 0.1206) * turbidity +		vector(-0.8989, -1.6537, -2.5771);
	paramE = vector(-0.0033, -0.0109, -0.0670) * turbidity +	vector(0.0452, 0.0529, 0.3703);
}

//------------------------------------------------------------------------------
/**
*/
float
PreethamUtil::Perez(float globalLightTheta, float gamma, float paramA, float paramB, float paramC, float paramD, float paramE)
{
	float gammaCos = n_cos(gamma);
	return (1.0f + paramA * n_exp(paramB / n_cos(globalLightTheta + 0.0001f))) * (1.0f + paramC * n_exp(paramD * gamma) + paramE * gammaCos * gammaCos);
}

//------------------------------------------------------------------------------
/**
*/
float
PreethamUtil::ZenithLuminance(float turbidity, float globalLightTheta)
{
	const float chi = (4.0f / 9.0f - turbidity / 120.0f) * (N_PI - 2 * globalLightTheta);
	return (4.0453f * turbidity - 4.9710f) * n_tan(chi) - 0.2155f * turbidity + 2.4192f;
}

//------------------------------------------------------------------------------
/**
*/
float
PreethamUtil::ZenithChromacity(float turbidity, float globalLightTheta, const Math::float4& row1, const Math::float4& row2, const Math::float4& row3)
{
	const float thetaSq = globalLightTheta * globalLightTheta;
	float4 thetaVec = float4(thetaSq * globalLightTheta, thetaSq, globalLightTheta, 1);
	float4 turbVec = float4(turbidity * turbidity, turbidity, 1, 0);
	return float4::dot3(turbVec, float4(float4::dot(thetaVec, row1), float4::dot(thetaVec, row2), float4::dot(thetaVec, row3), 0));
}

} // namespace PostEffect