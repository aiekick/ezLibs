#include <TestEzTime.h>
#include <ezlibs/ezTime.hpp>
#include <ezlibs/ezCTest.hpp>
#include <string>
#include <ctime>
#include <thread>
#include <chrono>

// Disable noisy conversion warnings
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)  // conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305)  // truncation from 'double' to 'float'
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

using namespace ez::time;

bool TestEzTime_Iso8601ToEpoch() {
    std::time_t outTime = 0;

    // Test valid ISO8601 date
    CTEST_ASSERT(iso8601ToEpoch("2025-01-15", "%Y-%m-%d", outTime) == true);
    CTEST_ASSERT(outTime > 0);

    // Test another valid date
    std::time_t outTime2 = 0;
    CTEST_ASSERT(iso8601ToEpoch("2020-06-30", "%Y-%m-%d", outTime2) == true);
    CTEST_ASSERT(outTime2 > 0);
    CTEST_ASSERT(outTime2 < outTime); // 2020 should be before 2025

    // Test invalid format
    std::time_t outTime3 = 0;
    CTEST_ASSERT(iso8601ToEpoch("invalid-date", "%Y-%m-%d", outTime3) == false);

    // Test empty string
    std::time_t outTime4 = 0;
    CTEST_ASSERT(iso8601ToEpoch("", "%Y-%m-%d", outTime4) == false);

    // Test with empty format
    std::time_t outTime5 = 0;
    CTEST_ASSERT(iso8601ToEpoch("2025-01-15", "", outTime5) == false);

    return true;
}

bool TestEzTime_EpochToISO8601() {
    std::string outTime;

    // Test with a known epoch time (2025-01-01 00:00:00 UTC is approximately 1735689600)
    // Using a relative time to be safe across systems
    std::time_t testTime = 1609459200; // 2021-01-01 00:00:00 UTC
    CTEST_ASSERT(epochToISO8601(testTime, outTime) == true);
    CTEST_ASSERT(!outTime.empty());
    CTEST_ASSERT(outTime.size() == 10); // YYYY-MM-DD format

    // Test with epoch 0 (1970-01-01)
    std::string outTime2;
    CTEST_ASSERT(epochToISO8601(0, outTime2) == true);
    CTEST_ASSERT(!outTime2.empty());

    return true;
}

bool TestEzTime_DecomposeEpoch() {
    // Test with a known time
    std::time_t testTime = 1609459200; // 2021-01-01 00:00:00 UTC
    tm result = decomposeEpoch(testTime);

    // Just check that we got a valid tm structure (year should be reasonable)
    CTEST_ASSERT(result.tm_year >= 70); // Years since 1900, so >= 1970
    CTEST_ASSERT(result.tm_mon >= 0 && result.tm_mon <= 11);
    CTEST_ASSERT(result.tm_mday >= 1 && result.tm_mday <= 31);

    return true;
}

bool TestEzTime_GetCurrentDate() {
    std::string currentDate = getCurrentDate();

    // Check that we got a non-empty result
    CTEST_ASSERT(!currentDate.empty());

    // Check format is YYYY-MM-DD (10 characters)
    CTEST_ASSERT(currentDate.size() == 10);

    // Check that dashes are in the right places
    CTEST_ASSERT(currentDate[4] == '-');
    CTEST_ASSERT(currentDate[7] == '-');

    return true;
}

bool TestEzTime_GetCurrentDateWithOffset() {
    std::string currentDate = getCurrentDate();
    std::string futureDate = getCurrentDate(24); // 24 hours in the future

    // Check both are valid dates
    CTEST_ASSERT(!currentDate.empty());
    CTEST_ASSERT(!futureDate.empty());
    CTEST_ASSERT(currentDate.size() == 10);
    CTEST_ASSERT(futureDate.size() == 10);

    // Future date should be different (though not necessarily greater due to local time)
    // At minimum, it should be a valid date format
    CTEST_ASSERT(futureDate[4] == '-');
    CTEST_ASSERT(futureDate[7] == '-');

    return true;
}

bool TestEzTime_GetTicks() {
    uint64_t ticks1 = getTicks();

    // Wait a small amount
    std::this_thread::sleep_for(std::chrono::milliseconds(2));

    uint64_t ticks2 = getTicks();

    // Second call should return a larger value
    CTEST_ASSERT(ticks2 > ticks1);

    // Difference should be at least 1ms (accounting for some variation)
    CTEST_ASSERT((ticks2 - ticks1) >= 1);

    return true;
}

bool TestEzTime_ScopedTimer() {
    double measured_time = 0.0;

    {
        ScopedTimer timer(measured_time);
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Should have measured some time (at least 5ms to account for variation)
    CTEST_ASSERT(measured_time >= 5.0);
    CTEST_ASSERT(measured_time < 100.0); // Should not be too large

    return true;
}

bool TestEzTime_GetTimeInterval() {
    // First call initializes
    float interval1 = getTimeInterval();

    // Wait a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    // Second call should return the interval
    float interval2 = getTimeInterval();

    // Should have measured some time (at least 0.003 seconds)
    CTEST_ASSERT(interval2 >= 0.003f);
    CTEST_ASSERT(interval2 < 0.5f); // Should not be too large

    return true;
}

bool TestEzTime_ActionTime_Basic() {
    ActionTime timer;

    // Should start with a valid state
    uint64_t initial = timer.get();
    CTEST_ASSERT(initial >= 0);

    // Wait a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(2));

    // Should have elapsed some time
    uint64_t elapsed = timer.get();
    CTEST_ASSERT(elapsed >= 1);

    return true;
}

bool TestEzTime_ActionTime_GetTime() {
    ActionTime timer;

    // Wait a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Get time in seconds
    double timeInSeconds = timer.getTime();

    // Should be at least 0.005 seconds
    CTEST_ASSERT(timeInSeconds >= 0.005);
    CTEST_ASSERT(timeInSeconds < 0.5);

    return true;
}

bool TestEzTime_ActionTime_Fix() {
    ActionTime timer;

    // Wait a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    // Fix the timer
    timer.fix();

    // Should be close to 0 now
    uint64_t elapsed = timer.get();
    CTEST_ASSERT(elapsed < 10); // Should be very small

    return true;
}

bool TestEzTime_ActionTime_SetTime() {
    ActionTime timer;

    // Set time to 5 seconds
    timer.setTime(5.0);

    // Get time should return approximately 5 seconds
    double timeInSeconds = timer.getTime();
    CTEST_ASSERT(timeInSeconds >= 4.9);
    CTEST_ASSERT(timeInSeconds <= 5.1);

    return true;
}

bool TestEzTime_ActionTime_FixTime() {
    ActionTime timer;

    // Fix time to 3 seconds
    timer.fixTime(3.0);

    // Get time should return approximately 3 seconds
    double timeInSeconds = timer.getTime();
    CTEST_ASSERT(timeInSeconds >= 2.9);
    CTEST_ASSERT(timeInSeconds <= 3.1);

    return true;
}

bool TestEzTime_ActionTime_PauseResume() {
    ActionTime timer;

    // Wait a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    // Pause
    timer.pause();
    uint64_t pausedTime = timer.get();

    // Wait more while paused
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Time should not have changed much
    uint64_t stillPausedTime = timer.get();
    CTEST_ASSERT(stillPausedTime >= pausedTime - 5);
    CTEST_ASSERT(stillPausedTime <= pausedTime + 5);

    // Resume
    timer.resume();

    // Wait a bit more
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    // Time should have increased again
    uint64_t resumedTime = timer.get();
    CTEST_ASSERT(resumedTime > pausedTime + 2);

    return true;
}

bool TestEzTime_ActionTime_IsTimeToAct() {
    ActionTime timer;

    // Immediately, should not be time to act for 20ms
    CTEST_ASSERT(timer.isTimeToAct(20, false) == false);

    // Wait 30ms
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    // Now it should be time to act
    CTEST_ASSERT(timer.isTimeToAct(20, true) == true);

    // After fix, should not be time to act again immediately
    CTEST_ASSERT(timer.isTimeToAct(20, false) == false);

    return true;
}

bool TestEzTime_MeasureOperationUs() {
    // Measure a simple operation
    int counter = 0;
    double time_us = measureOperationUs([&counter]() {
        counter++;
    }, 1);

    // Should have taken some time (though very little)
    CTEST_ASSERT(time_us >= 0.0);
    CTEST_ASSERT(time_us < 10000.0); // Should be less than 10ms
    CTEST_ASSERT(counter == 1);

    // Test with multiple operations
    counter = 0;
    double avg_time_us = measureOperationUs([&counter]() {
        counter++;
    }, 10);

    CTEST_ASSERT(avg_time_us >= 0.0);
    CTEST_ASSERT(counter == 10);

    return true;
}

bool TestEzTime_MeasureOperationUs_WithWork() {
    // Measure an operation that does some work
    double time_us = measureOperationUs([]() {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }, 1);

    // Should have taken at least 80 microseconds (with some margin)
    CTEST_ASSERT(time_us >= 80.0);
    CTEST_ASSERT(time_us < 20000.0); // Should be less than 20ms

    return true;
}

bool TestEzTime_WaitUntil_Success() {
    int counter = 0;

    // Start a thread that will set counter to 5 after 10ms
    std::thread t([&counter]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        counter = 5;
    });

    // Wait for counter to be 5 (max 100ms, check every 5ms)
    bool result = waitUntil([&counter]() { return counter == 5; }, 100, 5);

    CTEST_ASSERT(result == true);
    CTEST_ASSERT(counter == 5);

    t.join();

    return true;
}

bool TestEzTime_WaitUntil_Timeout() {
    int counter = 0;

    // Wait for something that will never happen (reduced timeout)
    bool result = waitUntil([&counter]() { return counter == 100; }, 20, 5);

    // Should timeout and return false
    CTEST_ASSERT(result == false);

    return true;
}

bool TestEzTime_WaitUntil_ImmediateTrue() {
    // Condition is already true
    bool result = waitUntil([]() { return true; }, 50, 5);

    // Should return immediately
    CTEST_ASSERT(result == true);

    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzTime(const std::string& vTest) {
    IfTestExist(TestEzTime_Iso8601ToEpoch);
    else IfTestExist(TestEzTime_EpochToISO8601);
    else IfTestExist(TestEzTime_DecomposeEpoch);
    else IfTestExist(TestEzTime_GetCurrentDate);
    else IfTestExist(TestEzTime_GetCurrentDateWithOffset);
    else IfTestExist(TestEzTime_GetTicks);
    else IfTestExist(TestEzTime_ScopedTimer);
    else IfTestExist(TestEzTime_GetTimeInterval);
    else IfTestExist(TestEzTime_ActionTime_Basic);
    else IfTestExist(TestEzTime_ActionTime_GetTime);
    else IfTestExist(TestEzTime_ActionTime_Fix);
    else IfTestExist(TestEzTime_ActionTime_SetTime);
    else IfTestExist(TestEzTime_ActionTime_FixTime);
    else IfTestExist(TestEzTime_ActionTime_PauseResume);
    else IfTestExist(TestEzTime_ActionTime_IsTimeToAct);
    else IfTestExist(TestEzTime_MeasureOperationUs);
    else IfTestExist(TestEzTime_MeasureOperationUs_WithWork);
    else IfTestExist(TestEzTime_WaitUntil_Success);
    else IfTestExist(TestEzTime_WaitUntil_Timeout);
    else IfTestExist(TestEzTime_WaitUntil_ImmediateTrue);

    return false;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
