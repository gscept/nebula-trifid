#pragma once
//------------------------------------------------------------------------------
/**
@class Test::MultiplayerConnectionTest


(C) 2009 Radon Labs GmbH
*/
#include "testbase/testcase.h"
#include "timing/timer.h"
#include "core/singleton.h"

namespace Test
{
class MultiplayerConnectionTest : public TestCase        
{
    __DeclareClass(MultiplayerConnectionTest);
    __DeclareSingleton(MultiplayerConnectionTest);
public:

    enum TestState
    {
        Start,
        LoggingIn,
        LoggedIn,
        GameSearchRunning,
        GameSearchFinished,
        Joining,
        Joined,
        CreatingSession,
        HostSession,
        SessionStarting,
        SessionStarted,
        RunningSessionLeft, 
        SessionEnding,      
        SessionLeaving,
        
        Exit
    };
    /// constructor
    MultiplayerConnectionTest();
    /// destructor
    ~MultiplayerConnectionTest();

    /// run the test
    virtual void Run();

    /// set test state
    void SetTestState(TestState state);

private:
    /// login
    void Login();
    /// start gamesearch
    void StartGameSearch();
    /// join 
    void Join();
    /// create session
    void CreateSession();
    /// start session
    void StartSession();
    /// end session
    void EndSession();
    /// delete session
    void DeleteSession();
                          
    Timing::Time waitTime;
    TestState curState;
    Timing::Timer timer;
    Timing::Time time;
};
}