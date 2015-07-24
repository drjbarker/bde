// bdlmtt_timedsemaphoreimpl_pthread.cpp                               -*-C++-*-
#include <bdlmtt_timedsemaphoreimpl_pthread.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmtt_timedsemaphoreimpl_pthread_cpp,"$Id$ $CSID$")

#include <bdlmtt_muteximpl_pthread.h>   // for testing only
#include <bdlmtt_saturatedtimeconversionimputil.h>
#include <bdlmtt_threadutil.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

#include <bsls_assert.h>

#ifdef BDLMTT_PLATFORM_POSIX_THREADS

// Platform-specific implementation starts here.

#include <bsl_ctime.h>
#include <bsl_c_errno.h>

namespace BloombergLP {

#if !defined(BSLS_PLATFORM_OS_DARWIN)
// Set the condition clock type, except on Darwin which doesn't support it.

class CondAttr {
    // This class is a thin wrapper over 'pthread_condattr_t' structure which
    // gets configured with the proper clock type for the purpose of
    // initializing the 'pthread_cond_t' object.

    // DATA
    pthread_condattr_t d_attr;

    // NOT IMPLEMENTED
    CondAttr();
    CondAttr(const CondAttr&);
    CondAttr& operator=(const CondAttr&);

public:
    CondAttr(bsls::SystemClockType::Enum clockType)
        // Create the 'pthread_condattr_t' structure and initialize it with the
        // specified 'clockType'.
    {
        int rc = pthread_condattr_init(&d_attr);
        (void) rc; BSLS_ASSERT(0 == rc);  // can only fail on ENOMEM

        clockid_t clockId;
        switch (clockType) {
          case bsls::SystemClockType::e_REALTIME: {
            clockId = CLOCK_REALTIME;
          } break;
          case bsls::SystemClockType::e_MONOTONIC: {
            clockId = CLOCK_MONOTONIC;
          } break;
          default:
            BSLS_ASSERT_OPT("Invalid ClockType parameter value" && 0);
        }

        pthread_condattr_setclock(&d_attr, clockId);
    }

    ~CondAttr()
        // Destroy the 'pthread_condattr_t' structure.
    {
        int rc = pthread_condattr_destroy(&d_attr);
        (void) rc; BSLS_ASSERT(0 == rc);  // can only fail on invalid 'd_attr'
    }

    const pthread_condattr_t& conditonAttributes() const
    {
        return d_attr;
    }
};

#endif

// STATIC HELPER FUNCTIONS

static
void initializeCondition(pthread_cond_t              *cond,
                         bsls::SystemClockType::Enum  clockType)
    // Initialize the specified 'cond' variable with the specified 'clockType'.
{
#ifdef BSLS_PLATFORM_OS_DARWIN
    (void) clockType;
    int rc = pthread_cond_init(cond, 0);
    (void) rc; BSLS_ASSERT(0 == rc); // 'pthread_cond_int' can only fail for
                                     // two possible reasons in this usage and
                                     // neither should ever occur:
                                     //: 1 lack of system resources
                                     //: 2 attempt to re-initialise 'cond'
#else
    CondAttr attr(clockType);
    int rc = pthread_cond_init(cond, &attr.conditonAttributes());
    (void) rc; BSLS_ASSERT(0 == rc); // 'pthread_cond_int' can only fail for
                                     // three possible reasons in this usage
                                     // and none should ever occur:
                                     //: 1 lack of system resources
                                     //: 2 attempt to re-initialise 'cond'
                                     //: 3 the attribute is invalid
#endif
}

static
int decrementIfPositive(bsls::AtomicInt *a)
    // Try to decrement the specified atomic integer 'a' if positive.  Return
    // 0 on success and a non-zero value otherwise.
{
    int i = *a;

    while (i > 0) {
        if (i == a->testAndSwap(i, i - 1)) {
            return 0;                                                 // RETURN
        }
        i = *a;
    }
    return -1;
}

namespace bdlmtt {
           // -----------------------------------------------------
           // class TimedSemaphoreImpl<PthreadTimedSemaphore>
           // -----------------------------------------------------

// CREATORS
TimedSemaphoreImpl<bdlmtt::Platform::PthreadTimedSemaphore>::
                TimedSemaphoreImpl(bsls::SystemClockType::Enum clockType)
: d_resources(0)
, d_waiters(0)
#ifdef BSLS_PLATFORM_OS_DARWIN
, d_clockType(clockType)
#endif
{
    pthread_mutex_init(&d_lock, 0);
    initializeCondition(&d_condition, clockType);
}

TimedSemaphoreImpl<bdlmtt::Platform::PthreadTimedSemaphore>::
     TimedSemaphoreImpl(int count, bsls::SystemClockType::Enum clockType)
: d_resources(count)
, d_waiters(0)
#ifdef BSLS_PLATFORM_OS_DARWIN
, d_clockType(clockType)
#endif
{
    pthread_mutex_init(&d_lock, 0);
    initializeCondition(&d_condition, clockType);
}

TimedSemaphoreImpl<bdlmtt::Platform::PthreadTimedSemaphore>::
                                                    ~TimedSemaphoreImpl()
{
    pthread_mutex_lock(&d_lock);
    pthread_mutex_destroy(&d_lock);

    pthread_cond_destroy(&d_condition);

    BSLS_ASSERT(d_resources >= 0);
    BSLS_ASSERT(d_waiters >= 0);
}

// MANIPULATORS
void TimedSemaphoreImpl<bdlmtt::Platform::PthreadTimedSemaphore>::post()
{
    ++d_resources;
    // barrier
    if (d_waiters > 0) {
        pthread_mutex_lock(&d_lock);
        pthread_cond_signal(&d_condition);
        pthread_mutex_unlock(&d_lock);
    }
}

void
TimedSemaphoreImpl<bdlmtt::Platform::PthreadTimedSemaphore>::post(int n)
{
    BSLS_ASSERT(n > 0);

    d_resources += n;
    // barrier
    if (d_waiters > 0) {
        pthread_mutex_lock(&d_lock);
        pthread_cond_broadcast(&d_condition);
        pthread_mutex_unlock(&d_lock);
    }
}

int TimedSemaphoreImpl<bdlmtt::Platform::PthreadTimedSemaphore>::timedWait(
                                              const bsls::TimeInterval& timeout)
{
    if (0 == decrementIfPositive(&d_resources)) {
        return 0;                                                     // RETURN
    }

    int ret = 0;
    pthread_mutex_lock(&d_lock);
    ++d_waiters;

    while (0 != decrementIfPositive(&d_resources)) {
        int status = timedWaitImp(timeout);
        if (0 != status) {
            BSLS_ASSERT(-1 == status);    // timeout and not an error
            ret = 1;
            break;
        }
    }

    --d_waiters;
    pthread_mutex_unlock(&d_lock);
    return ret;
}

int TimedSemaphoreImpl<bdlmtt::Platform::PthreadTimedSemaphore>
                               ::timedWaitImp(const bsls::TimeInterval& timeout)
{
#ifdef BSLS_PLATFORM_OS_DARWIN
    // Darwin supports only realtime clock for the condition variable.

    bsls::TimeInterval realTimeout(timeout);

    if (d_clockType != bsls::SystemClockType::e_REALTIME) {
        // since cond_timedwait operates only with the realtime clock, adjust
        // the timeout value to make it consistent with the realtime clock
        realTimeout += bsls::SystemTime::nowRealtimeClock()
                       - bdlt::CurrentTime::now(d_clockType);
    }

    timespec ts;
    SaturatedTimeConversionImpUtil::toTimeSpec(&ts, realTimeout);

#else  // !DARWIN
    timespec ts;
    SaturatedTimeConversionImpUtil::toTimeSpec(&ts, timeout);
#endif

    int status = pthread_cond_timedwait(&d_condition, &d_lock, &ts);
    return status == 0 ? 0 : (status == ETIMEDOUT ? -1 : -2);
}

int TimedSemaphoreImpl<bdlmtt::Platform::PthreadTimedSemaphore>::tryWait()
{
    const int newValue = decrementIfPositive(&d_resources);
    if (newValue >= 0) {
        if (newValue != 0) {
            pthread_cond_signal(&d_condition);
        }
        return 0;                                                     // RETURN
    }
    return 1;
}

void TimedSemaphoreImpl<bdlmtt::Platform::PthreadTimedSemaphore>::wait()
{
    if (0 == decrementIfPositive(&d_resources)) {
        return;                                                       // RETURN
    }

    pthread_mutex_lock(&d_lock);
    ++d_waiters;
    while (0 != decrementIfPositive(&d_resources)) {
        pthread_cond_wait(&d_condition, &d_lock);
    }
    --d_waiters;
    pthread_mutex_unlock(&d_lock);
}
}  // close package namespace

}  // close namespace BloombergLP

#endif  // BDLMTT_PLATFORM_POSIX_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
