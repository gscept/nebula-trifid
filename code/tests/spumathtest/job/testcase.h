#pragma once
//------------------------------------------------------------------------------
/**
    @class Test::TestCase
    
    Base class for a test case.
    
    (C) 2006 Radon Labs GmbH
*/

//------------------------------------------------------------------------------
namespace Test
{
class TestCase
{
public:
    /// constructor
    TestCase(const char *name);
    /// destructor
    virtual ~TestCase();
    /// run the test
    virtual void Run();
    /// verify a statement
    void Verify(bool b);
    /// return number of succeeded verifies
    int GetNumSucceeded() const;
    /// return number of failed verifies
    int GetNumFailed() const;
    /// return overall number of verifies
    int GetNumVerified() const;
    /// get the name of this testcase
    const char *GetName() const;
private:
    const char *name;
    int numVerified;
    int numSucceeded;
    int numFailed;    
};

//------------------------------------------------------------------------------
/*
*/
inline
int
TestCase::GetNumSucceeded() const
{
    return this->numSucceeded;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
TestCase::GetNumFailed() const
{
    return this->numFailed;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
TestCase::GetNumVerified() const
{
    return this->numVerified;
}

inline
const char *
TestCase::GetName() const
{
    return this->name;
}

};
//------------------------------------------------------------------------------