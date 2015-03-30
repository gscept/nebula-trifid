#ifndef TEST_TESTATTRS_H
#define TEST_TESTATTRS_H
//------------------------------------------------------------------------------
/**
    @file test/testattrs.h
    
    Declare common attributes for the tests.
    
    (C) 2006 Radon Labs GmbH
*/
#include "attr/attribute.h"

//------------------------------------------------------------------------------
namespace Attr
{
    DeclareGuid(GuidValue, 'guid', ReadWrite);
    DeclareBool(BoolValue, 'bval', ReadWrite);
    DeclareFloat(FloatValue, 'fval', ReadWrite);
    DeclareInt(IntValue, 'ival',  ReadWrite);
    DeclareFloat4(Float4Value, 'v4vl', ReadWrite);
    DeclareMatrix44(Matrix44Value, 'mxvl', ReadWrite);
    DeclareString(StringValue, 'sval', ReadWrite);
    DeclareBlob(BlobValue, 'blob', ReadWrite);

    DeclareString(Name, 'NAME', ReadWrite);
    DeclareBool(Male, 'MALE', ReadWrite);
    DeclareFloat(IQ, 'IQ__', ReadWrite);
    DeclareInt(Age, 'AGE_', ReadWrite);
    DeclareFloat4(Velocity, 'VELO', ReadWrite);
    DeclareFloat4(Color, 'COLR', ReadWrite);
    DeclareMatrix44(Transform, 'TFRM', ReadWrite);
    DeclareString(CarModel, 'CARM', ReadWrite);
    DeclareString(Street, 'STRT', ReadWrite);
    DeclareString(Product, 'PROD', ReadWrite);
    DeclareString(City, 'CITY', ReadWrite);
    DeclareString(Phone, 'PHON', ReadWrite);
    DeclareGuid(GUID, 'GUID', ReadWrite);
    DeclareInt(Nr, 'NR__', ReadWrite);
    DeclareInt(Price, 'PRCE', ReadWrite);
    DeclareInt(Stock, 'STCK', ReadWrite);
    DeclareString(Country, 'CTRY', ReadWrite);
};
//------------------------------------------------------------------------------
#endif
    