//------------------------------------------------------------------------------
//  matrix44test.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------

#include "matrix44test.h"
#include "math/matrix44.h"
#include "math/float4.h"
#include "math/plane.h"
#include "math/quaternion.h"
#include "math/point.h"
#include "math/vector.h"
#include "mathtestutil.h"
#include "stackalignment.h"
#include "stackdebug.h"

using namespace Math;
using namespace Test;

//------------------------------------------------------------------------------
/**
*/Matrix44Test::Matrix44Test(const char *name) : 
    TestCase(name),
    pOneTwoThree(1.0, 2.0, 3.0, 1.0),
    vOneTwoThree(1.0, 2.0, 3.0, 0.0),
    trans123(float4(1.0f, 0.0f, 0.0f, 0.0f),
			 float4(0.0f, 1.0f, 0.0f, 0.0f),
			 float4(0.0f, 0.0f, 1.0f, 0.0f),
			 pOneTwoThree),
    midentity(matrix44::identity()),
    rotOneX(matrix44::rotationx(1.0)),
    pZero(0.0, 0.0, 0.0, 1.0),
    pOneX(1.0, 0.0, 0.0, 1.0),
    pOneY(0.0, 1.0, 0.0, 1.0)
{
}

//------------------------------------------------------------------------------
/**
*/
void
Matrix44Test::RunTest0()
{
    float4 result;

	// identity and construction
    this->Verify(midentity.isidentity());
    matrix44 m0(midentity);
    this->Verify(m0.isidentity());
    this->Verify(m0 == midentity);

	// multiply by identity
	m0 = matrix44::multiply(trans123, midentity);
	this->Verify(m0 == trans123);

	// point transform by matrix
	result = matrix44::transform(pZero, trans123);
	this->Verify(float4equal(result, pOneTwoThree));

    // multiplication and multiplication order, transform point by matrix
	const matrix44 mRotOneX_Trans123 = matrix44::multiply(rotOneX, trans123);
	this->Verify(matrix44equal(mRotOneX_Trans123,
							   matrix44(float4(1.0f,       0.0f,      0.0f, 0.0f),
										float4(0.0f,  0.540302f, 0.841471f, 0.0f),
										float4(0.0f, -0.841471f, 0.540302f, 0.0f),
										float4(1.0f,       2.0f,      3.0f, 1.0f))));
	result = matrix44::transform(pZero, mRotOneX_Trans123);
	this->Verify(float4equal(result, pOneTwoThree));
	result = matrix44::transform(pOneX, mRotOneX_Trans123);
	this->Verify(float4equal(result, float4(2.0f, 2.0f, 3.0f, 1.0f)));
	result = matrix44::transform(pOneY, mRotOneX_Trans123);
	this->Verify(float4equal(result, float4(1.0f, 2.540302f, 3.841471f, 1.0f)));

	// translate
	m0 = matrix44::identity();
	m0.translate(vOneTwoThree);
	this->Verify(matrix44equal(m0, trans123));
	m0 = mRotOneX_Trans123;
	m0 = matrix44::translation(-0.1f, 4.5f, 2.1f);
	this->Verify(matrix44equal(m0,
							   matrix44(float4( 1.0f, 0.0f, 0.0f, 0.0f),
										float4( 0.0f, 1.0f, 0.0f, 0.0f),
										float4( 0.0f, 0.0f, 1.0f, 0.0f),
										float4(-0.1f, 4.5f, 2.1f, 1.0f))));
	m0 = mRotOneX_Trans123;
	m0 = matrix44::translation(float4(-0.1f, 4.5f, 2.1f, 0.0f));
	this->Verify(matrix44equal(m0,
							   matrix44(float4( 1.0f, 0.0f, 0.0f, 0.0f),
										float4( 0.0f, 1.0f, 0.0f, 0.0f),
										float4( 0.0f, 0.0f, 1.0f, 0.0f),
										float4(-0.1f, 4.5f, 2.1f, 1.0f))));

	// scale
	m0 = mRotOneX_Trans123;
	m0.scale(float4(0.5f, 1.5f, -3.0f, 1.0f));
	this->Verify(matrix44equal(m0,
							   matrix44(float4(0.5f,       0.0f,       0.0f, 0.0f),
										float4(0.0f,  0.810453f, -2.524413f, 0.0f),
										float4(0.0f, -1.262206f, -1.620907f, 0.0f),
										float4(0.5f,  3.000000f, -9.000000f, 1.0f))));
	m0 = matrix44::scaling(0.1f, -2.0f, 13.0f);
	this->Verify(matrix44equal(m0,
							   matrix44(float4(0.1f,  0.0f,  0.0f, 0.0f),
										float4(0.0f, -2.0f,  0.0f, 0.0f),
										float4(0.0f,  0.0f, 13.0f, 0.0f),
										float4(0.0f,  0.0f,  0.0f, 1.0f))));
	m0 = matrix44::scaling(float4(0.1f, -2.0f, 13.0f, 1.0f));
	this->Verify(matrix44equal(m0,
							   matrix44(float4(0.1f,  0.0f,  0.0f, 0.0f),
										float4(0.0f, -2.0f,  0.0f, 0.0f),
										float4(0.0f,  0.0f, 13.0f, 0.0f),
										float4(0.0f,  0.0f,  0.0f, 1.0f))));

	// transpose
	m0 = matrix44::transpose(mRotOneX_Trans123);
	this->Verify(matrix44equal(m0,
							   matrix44(float4( 1.0f,       0.0f,       0.0f, 1.0f),
										float4( 0.0f,  0.540302f, -0.841471f, 2.0f),
										float4( 0.0f,  0.841471f,  0.540302f, 3.0f),
										float4( 0.0f,       0.0f,       0.0f, 1.0f))));
}

//------------------------------------------------------------------------------
/**
*/
void
Matrix44Test::RunTest1()
{
	// rotations
	const matrix44 rotX = matrix44::rotationx(2.0);
	this->Verify(matrix44equal(rotX,
							   matrix44(float4(1.000000f,  0.000000f,  0.000000f, 0.000000f),
										float4(0.000000f, -0.416147f,  0.909297f, 0.000000f),
										float4(0.000000f, -0.909297f, -0.416147f, 0.000000f),
										float4(0.000000f,  0.000000f,  0.000000f, 1.000000f))));
	const matrix44 rotY = matrix44::rotationy(-1.7);
	this->Verify(matrix44equal(rotY,
							   matrix44(float4(-0.128845f, 0.000000f,  0.991665f, 0.000000f),
										float4( 0.000000f, 1.000000f,  0.000000f, 0.000000f),
										float4(-0.991665f, 0.000000f, -0.128845f, 0.000000f),
										float4( 0.000000f, 0.000000f,  0.000000f, 1.000000f))));
	const matrix44 rotZ = matrix44::rotationz(3.1);
	this->Verify(matrix44equal(rotZ,
							   matrix44(float4(-0.999135f,  0.041581f, 0.000000f, 0.000000f),
										float4(-0.041581f, -0.999135f, 0.000000f, 0.000000f),
										float4( 0.000000f,  0.000000f, 1.000000f, 0.000000f),
										float4( 0.000000f,  0.000000f, 0.000000f, 1.000000f))));
	const float4 rotaxis = float4::normalize(float4(1.0f, 0.2f, 2.0f, 0.0f));
	const matrix44 rot = matrix44::rotationaxis(rotaxis, -2.53652f);
	this->Verify(matrix44equal(rot,
							   matrix44(float4(-0.460861f, -0.434427f,  0.773873f, 0.000000f),
										float4( 0.579067f, -0.807997f, -0.108734f, 0.000000f),
										float4( 0.672524f,  0.398013f,  0.623936f, 0.000000f),
										float4( 0.000000f,  0.000000f,  0.000000f, 1.000000f))));

	// reflect
	const plane planeXZ(0.0, 1.0, 0.0, 0.0);
	const matrix44 mReflectXZ = matrix44::reflect(planeXZ);
	this->Verify(matrix44equal(mReflectXZ,
							   matrix44(float4(1.0f,  0.0f, 0.0f, 0.0f ),
										float4(0.0f, -1.0f, 0.0f, 0.0f ),
										float4(0.0f,  0.0f, 1.0f, 0.0f ),
										float4(0.0f,  0.0f, 0.0f, 1.0f ))));
	const float4 pReflected = matrix44::transform(pOneTwoThree, mReflectXZ);
	this->Verify(float4equal(pReflected, float4(1.0f, -2.0f, 3.0f, 1.0f)));

    // test 16-byte alignment of embedded members on the stack, if we use SSE/SSE2 on windows or
    // xbox or ps3
#if (__WIN32__ && !defined(_XM_NO_INTRINSICS_)) || __XBOX360__ || __PS3__    
    {
        testStackAlignment16<matrix44>(this);
    }
#endif

    // load and store aligned
    ALIGN16 const scalar fAlignedLoad[16] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
    ALIGN16 scalar fAlignedStore[16];
	// check alignment
	n_assert(!((unsigned int)fAlignedLoad & 0xF));
	n_assert(!((unsigned int)fAlignedStore & 0xF));
	matrix44 m;
    m.load(fAlignedLoad);
    this->Verify(m == matrix44(float4( 1.0f,  2.0f,  3.0f,  4.0f),
							   float4( 5.0f,  6.0f,  7.0f,  8.0f),
							   float4( 9.0f, 10.0f, 11.0f, 12.0f),
							   float4(13.0f, 14.0f, 15.0f, 16.0f)));
	// load unaligned must work with aligned data too
    m.loadu(fAlignedLoad);
    this->Verify(m == matrix44(float4( 1.0f,  2.0f,  3.0f,  4.0f),
						       float4( 5.0f,  6.0f,  7.0f,  8.0f),
						  	   float4( 9.0f, 10.0f, 11.0f, 12.0f),
							   float4(13.0f, 14.0f, 15.0f, 16.0f)));
	m.store(fAlignedStore);
    this->Verify((fAlignedStore[0]  ==  1.0f) && (fAlignedStore[1]  ==  2.0f) && (fAlignedStore[2]  ==  3.0f) && (fAlignedStore[3]  ==  4.0f) &&
				 (fAlignedStore[4]  ==  5.0f) && (fAlignedStore[5]  ==  6.0f) && (fAlignedStore[6]  ==  7.0f) && (fAlignedStore[7]  ==  8.0f) &&
				 (fAlignedStore[8]  ==  9.0f) && (fAlignedStore[9]  == 10.0f) && (fAlignedStore[10] == 11.0f) && (fAlignedStore[11] == 12.0f) &&
				 (fAlignedStore[12] == 13.0f) && (fAlignedStore[13] == 14.0f) && (fAlignedStore[14] == 15.0f) && (fAlignedStore[15] == 16.0f));
	// store unaligned must work with aligned data too
    m.storeu(fAlignedStore);
    this->Verify((fAlignedStore[0]  ==  1.0f) && (fAlignedStore[1]  ==  2.0f) && (fAlignedStore[2]  ==  3.0f) && (fAlignedStore[3]  ==  4.0f) &&
				 (fAlignedStore[4]  ==  5.0f) && (fAlignedStore[5]  ==  6.0f) && (fAlignedStore[6]  ==  7.0f) && (fAlignedStore[7]  ==  8.0f) &&
				 (fAlignedStore[8]  ==  9.0f) && (fAlignedStore[9]  == 10.0f) && (fAlignedStore[10] == 11.0f) && (fAlignedStore[11] == 12.0f) &&
				 (fAlignedStore[12] == 13.0f) && (fAlignedStore[13] == 14.0f) && (fAlignedStore[14] == 15.0f) && (fAlignedStore[15] == 16.0f));
    m.stream(fAlignedStore);
    this->Verify((fAlignedStore[0]  ==  1.0f) && (fAlignedStore[1]  ==  2.0f) && (fAlignedStore[2]  ==  3.0f) && (fAlignedStore[3]  ==  4.0f) &&
				 (fAlignedStore[4]  ==  5.0f) && (fAlignedStore[5]  ==  6.0f) && (fAlignedStore[6]  ==  7.0f) && (fAlignedStore[7]  ==  8.0f) &&
				 (fAlignedStore[8]  ==  9.0f) && (fAlignedStore[9]  == 10.0f) && (fAlignedStore[10] == 11.0f) && (fAlignedStore[11] == 12.0f) &&
				 (fAlignedStore[12] == 13.0f) && (fAlignedStore[13] == 14.0f) && (fAlignedStore[14] == 15.0f) && (fAlignedStore[15] == 16.0f));

	// load and store unaligned
	ALIGN16 const scalar fAlignedLoadBase[17] = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
	ALIGN16 scalar fAlignedStoreBase[17];
	const scalar *fUnalignedLoad = fAlignedLoadBase + 1;
	scalar *fUnalignedStore = fAlignedStoreBase + 1;
	// check un-alignment
	n_assert(((unsigned int)fUnalignedLoad & 0xF));
	n_assert(((unsigned int)fUnalignedStore & 0xF));
    m.loadu(fUnalignedLoad);
    this->Verify(m == matrix44(float4( 1.0f,  2.0f,  3.0f,  4.0f),
							   float4( 5.0f,  6.0f,  7.0f,  8.0f),
							   float4( 9.0f, 10.0f, 11.0f, 12.0f),
							   float4(13.0f, 14.0f, 15.0f, 16.0f)));
    m.storeu(fUnalignedStore);
    this->Verify((fUnalignedStore[0]  ==  1.0f) && (fUnalignedStore[1]  ==  2.0f) && (fUnalignedStore[2]  ==  3.0f) && (fUnalignedStore[3]  ==  4.0f) &&
				 (fUnalignedStore[4]  ==  5.0f) && (fUnalignedStore[5]  ==  6.0f) && (fUnalignedStore[6]  ==  7.0f) && (fUnalignedStore[7]  ==  8.0f) &&
				 (fUnalignedStore[8]  ==  9.0f) && (fUnalignedStore[9]  == 10.0f) && (fUnalignedStore[10] == 11.0f) && (fUnalignedStore[11] == 12.0f) &&
				 (fUnalignedStore[12] == 13.0f) && (fUnalignedStore[13] == 14.0f) && (fUnalignedStore[14] == 15.0f) && (fUnalignedStore[15] == 16.0f));

}

//------------------------------------------------------------------------------
/**
*/
void
Matrix44Test::RunTest2()
{
    // component-wise access
	matrix44 m1, m0;
    m0.set(float4(2.0f, 0.0f, 0.0f, 0.0f),
           float4(0.0f, 2.0f, 0.0f, 0.0f),
           float4(0.0f, 0.0f, 2.0f, 0.0f),
           float4(0.0f, 0.0f, 0.0f, 1.0f));
    float4 value(2.0f, 0.0f, 0.0f, 0.0f);
    m1.setrow0(value);
    value = float4(0.0f, 2.0f, 0.0f, 0.0f);
    m1.setrow1(value);
    value = float4(0.0f, 0.0f, 2.0f, 0.0f);
    m1.setrow2(value);
    value = float4(0.0f, 0.0f, 0.0f, 1.0f);
    m1.setrow3(value);
    this->Verify(m0 == m1);
    this->Verify(m0.getrow0() == float4(2.0f, 0.0f, 0.0f, 0.0f));
    this->Verify(m0.getrow1() == float4(0.0f, 2.0f, 0.0f, 0.0f));
    this->Verify(m0.getrow2() == float4(0.0f, 0.0f, 2.0f, 0.0f));
    this->Verify(m0.getrow3() == float4(0.0f, 0.0f, 0.0f, 1.0f));


	// transformation
	const quaternion qRotOneX = matrix44::rotationmatrix(rotOneX);

	const point eye(3.0f, 10.0f, -2.0f);
	const point at(3.0f, 2.0f, -2.0f);
	const vector up(0.0f, 1.0f, 0.0f);
	// lookatlh
	matrix44 tmp = matrix44::lookatlh(eye, at, up);
	this->Verify(matrix44equal(tmp, matrix44(float4(  0.0f,  0.0f,  0.0f, 0.0f),
										     float4(  0.0f,  0.0f,  0.0f, 0.0f),
										     float4(  0.0f, -1.0f,  0.0f, 0.0f),
										     float4(  3.0f, 10.0f, -2.0f, 1.0f))));
	// lookatrh
	tmp = matrix44::lookatrh(eye, at, up);
	this->Verify(matrix44equal(tmp, matrix44(float4(  0.0f,  0.0f,  0.0f, 0.0f),
										     float4(  0.0f,  0.0f,  0.0f, 0.0f),
										     float4(  0.0f,  1.0f,  0.0f, 0.0f),
										     float4(  3.0f, 10.0f, -2.0f, 1.0f))));
	// ortholh
	tmp = matrix44::ortholh(1280.0f, 1024.0f, 0.1f, 100.0f);
	this->Verify(matrix44equal(tmp, matrix44(float4( 0.001563f,      0.0f,       0.0f, 0.0f),
										     float4(      0.0f, 0.001953f,       0.0f, 0.0f),
										     float4(      0.0f,      0.0f,   0.01001f, 0.0f),
										     float4(      0.0f,      0.0f, -0.001001f, 1.0f))));
	// orthorh
	tmp = matrix44::orthorh(1280.0f, 1024.0f, 0.1f, 100.0f);
	this->Verify(matrix44equal(tmp, matrix44(float4( 0.001563f,      0.0f,       0.0f, 0.0f),
										     float4(      0.0f, 0.001953f,       0.0f, 0.0f),
										     float4(      0.0f,      0.0f,  -0.01001f, 0.0f),
										     float4(      0.0f,      0.0f, -0.001001f, 1.0f))));
	// orthooffcenterlh
	tmp = matrix44::orthooffcenterlh(100.0f, 1380.0f, 200.0f, 1224.0f, 0.1f, 1000.0f);
	this->Verify(matrix44equal(tmp, matrix44(float4( 0.001563f,       0.0f,     0.0f, 0.0f),
										     float4(      0.0f,  0.001953f,     0.0f, 0.0f),
										     float4(      0.0f,       0.0f,   0.001f, 0.0f),
										     float4( -1.15625f, -1.390625f, -0.0001f, 1.0f))));
	// orthooffcenterrh
	tmp = matrix44::orthooffcenterrh(100.0f, 1380.0f, 200.0f, 1224.0f, 0.1f, 1000.0f);
	this->Verify(matrix44equal(tmp, matrix44(float4( 0.001563f,       0.0f,     0.0f, 0.0f),
										     float4(      0.0f,  0.001953f,     0.0f, 0.0f),
										     float4(      0.0f,       0.0f,  -0.001f, 0.0f),
										     float4( -1.15625f, -1.390625f, -0.0001f, 1.0f))));
	// persplh
	tmp = matrix44::persplh(1280.0f, 1024.0f, 0.1f, 100.0f);
	this->Verify(matrix44equal(tmp, matrix44(float4( 0.000156f,      0.0f,      0.0f, 0.0f),
										     float4(      0.0f, 0.000195f,      0.0f, 0.0f),
										     float4(      0.0f,      0.0f, 1.001001f, 1.0f),
										     float4(      0.0f,      0.0f,  -0.1001f, 0.0f))));

	// persprh
	tmp = matrix44::persprh(1280.0f, 1024.0f, 0.1f, 100.0f);
	this->Verify(matrix44equal(tmp, matrix44(float4( 0.000156f,      0.0f,       0.0f,  0.0f),
										     float4(      0.0f, 0.000195f,       0.0f,  0.0f),
										     float4(      0.0f,      0.0f, -1.001001f, -1.0f),
										     float4(      0.0f,      0.0f,   -0.1001f,  0.0f))));

	// perspoffcenterlh
	tmp = matrix44::perspoffcenterlh(50.0f, 1330.0f, -150.0f, 874.0f, 1.0f, 1000.0f);
	this->Verify(matrix44equal(tmp, matrix44(float4( 0.001563f,       0.0f,       0.0f, 0.0f),
										     float4(      0.0f,  0.001953f,       0.0f, 0.0f),
										     float4(-1.078125f, -0.707031f,  1.001001f, 1.0f),
										     float4(      0.0f,       0.0f, -1.001001f, 0.0f))));

	// perspoffcenterrh
	tmp = matrix44::perspoffcenterrh(50.0f, 1330.0f, -150.0f, 874.0f, 1.0f, 1000.0f);
	this->Verify(matrix44equal(tmp, matrix44(float4(0.001563f,      0.0f,       0.0f,  0.0f),
										     float4(     0.0f, 0.001953f,       0.0f,  0.0f),
										     float4(1.078125f, 0.707031f, -1.001001f, -1.0f),
										     float4(     0.0f,      0.0f, -1.001001f,  0.0f))));
	// rotationyawpitchroll
	tmp = matrix44::rotationyawpitchroll(1.0f, -0.462f, 3.036f);
	this->Verify(matrix44equal(tmp, matrix44(float4(-0.576825f,  0.094347f, 0.811401f, 0.0f),
										     float4( 0.316042f, -0.890177f, 0.328181f, 0.0f),
										     float4( 0.753254f,  0.445739f, 0.483659f, 0.0f),
										     float4(      0.0f,       0.0f,      0.0f, 1.0f))));
	tmp = matrix44::rotationyawpitchroll(-7.0f, 3.0f, -2.0f);
	this->Verify(matrix44equal(tmp, matrix44(float4(-0.229430f,  0.900198f, -0.370144f, 0.000000f),
										     float4( 0.724104f,  0.411982f,  0.553122f, 0.000000f),
										     float4( 0.650412f, -0.141120f, -0.746358f, 0.000000f),
										     float4( 0.000000f,  0.000000f,  0.000000f, 1.000000f))));
}

//------------------------------------------------------------------------------
/**
*/
void
Matrix44Test::Run()
{
    STACK_CHECKPOINT("Matrix44Test::Run()");
    DumpStackCheckpoints();

    this->RunTest0();
    this->RunTest1();
    this->RunTest2();
}
