// btlsos_tcptimedcbacceptor.cpp                                      -*-C++-*-
#include <btlsos_tcptimedcbacceptor.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlsos_tcptimedcbacceptor_cpp,"$Id$ $CSID$")

#include <btlsos_tcptimedcbchannel.h>
#include <btlsos_tcpcbchannel.h>
#include <btlso_timereventmanager.h>
#include <btlso_streamsocketfactory.h>
#include <btlso_streamsocket.h>
#include <btlsc_flag.h>

#include <bdlf_function.h>
#include <bdlf_memfn.h>
#include <bdlf_bind.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_blockgrowth.h>

#ifdef TEST
// These dependencies will cause the test driver to recompile when the concrete
// implementation of the event manager changes.
#include <btlso_tcptimereventmanager.h>
#endif

#include <bsl_algorithm.h>
#include <bsl_vector.h>

// ===========================================================================
// IMPLEMENTATION DETAILS
// ---------------------------------------------------------------------------
// 1.  Internally, this acceptor holds a queue of callbacks for allocation
// requests.  The queue contains both timed and non-timed callbacks along with
// any supporting data for a request, such as the timeout value, if any, and
// flags.
//
// 2.  At most two callbacks are registered at any time with the socket event
// manager: a timer callback and an accept callback.  A timer callback is
// registered if and only if an accept callback is registered and if the
// corresponding request is for a timed operation (i.e., as a result of
// 'timedAllocate' or 'timedAllocateTimed' operations).
//
// 3.  In case a timer callback is registered, the timer registration ID is
// cached.  A NULL value is used for the timer ID to determine if a timer
// callback is registered.
//
// 4.  The request queue keeps the type of the result (i.e., timed or non-timed
// channel) and the request can invoke the callback in a type-safe manner.
//
// 5.  All allocate methods register callbacks with a socket event manager if
// the request queue size is 1 (i.e., when only the current request is cached).
// Subsequent requests are simply queued.  They are loaded into the current
// request when that request has been completed, but no additional callback is
// registered for that (see end of method 'acceptCb').
//
// 6.  The 'deallocate' method does not deallocate a channel directly, it
// rather installs the deallocate callback to be invoked on the next invocation
// of the 'dispatch' method of the timer event manager.  The deallocate
// callback will actually destroy the resources allocates for a channel.
// ===========================================================================

namespace BloombergLP {

// ============================================================================
//                             LOCAL DEFINITIONS
// ============================================================================

                         // ========================
                         // Local typedefs and enums
                         // ========================

enum {
    CALLBACK_SIZE      = sizeof(btlsc::TimedCbChannelAllocator::Callback),
    TIMEDCALLBACK_SIZE = sizeof (btlsc::TimedCbChannelAllocator::TimedCallback),
    ARENA_SIZE         = CALLBACK_SIZE < TIMEDCALLBACK_SIZE
                         ? TIMEDCALLBACK_SIZE
                         : CALLBACK_SIZE
};

enum {
    CHANNEL_SIZE       = sizeof(btlsos::TcpCbChannel) <
                                               sizeof(btlsos::TcpTimedCbChannel)
                         ? sizeof(btlsos::TcpTimedCbChannel)
                         : sizeof(btlsos::TcpCbChannel)
};

enum {
    CLOSED             = -3,
    UNINITIALIZED      = -2,
    INVALID            = -1,
    CANCELLED          = -1,
    SUCCESS            = 0
};

namespace btlsos {

                       // ============================
                       // class TcpTimedCbAcceptor_Reg
                       // ============================

class TcpTimedCbAcceptor_Reg {
    // This class stores either a callback or a timed callback, and allows to
    // invoke it, in a type-safe manner.

    // PRIVATE DATA MEMBERS
    union {
        char                                d_callbackArena[ARENA_SIZE];
        bsls::AlignmentUtil::MaxAlignedType d_align;  // for alignment
    } d_cb;                                      // callback storage arena

    bool              d_isTimedChannel;    // true if the channel is timed
    bool              d_isTimedOperation;  // true if the callback is timed
    bsls::TimeInterval d_timeout;           // timeout, if callback is timed
    int               d_flags;             // associated flags

  private:
    // Not implemented.
    TcpTimedCbAcceptor_Reg(const TcpTimedCbAcceptor_Reg&);
    TcpTimedCbAcceptor_Reg&
                               operator=(const TcpTimedCbAcceptor_Reg&);
  public:
    // CREATORS
    TcpTimedCbAcceptor_Reg(
            const bsls::TimeInterval& timeout
          , const bdlf::Function<void (*)(btlsc::TimedCbChannel*, int)>& functor
          , int flags);
    TcpTimedCbAcceptor_Reg(
            const bdlf::Function<void (*)(btlsc::CbChannel*, int)>& functor
          , int flags);
    TcpTimedCbAcceptor_Reg(
            const bdlf::Function<void (*)(btlsc::TimedCbChannel*, int)>& functor
          , int flags);
    TcpTimedCbAcceptor_Reg(
            const bsls::TimeInterval& timeout
          , const bdlf::Function<void (*)(btlsc::CbChannel*, int)>& functor
          , int flags);
        // Create a callback from a specified functor 'func' with specified
        // 'flags', and an optionally specified 'timeout' for the callback
        // execution.

    ~TcpTimedCbAcceptor_Reg();
        // Destroy this object.

    // MANIPULATORS
    void invoke(int status);
    void invoke(btlsc::CbChannel *channel, int status);
    void invokeTimed(btlsc::TimedCbChannel *channel, int status);
        // Invoke the callback functor contained in this request passing to it
        // an optionally specified 'channel' value for the channel address and
        // the specified 'status'.  If 'channel' is not specified, a NULL value
        // is used.

    // ACCESSORS
    int flags() const;
        // Access the 'flags' with which this object was created.

    bool isTimedResult() const;
        // Return 'true' if the underlying channel is timed, 'false' otherwise.

    bool isTimedOperation() const;
        // Return non-zero if this callback request has a timeout value.

    const bsls::TimeInterval& timeout() const;
        // Access the timeout value, if any, with which this object was
        // created.
};

// CREATORS
TcpTimedCbAcceptor_Reg::TcpTimedCbAcceptor_Reg(
        const bsls::TimeInterval& timeout
      , const bdlf::Function<void (*)(btlsc::TimedCbChannel*, int)>& functor
      , int flags)
: d_isTimedChannel(true)
, d_isTimedOperation(true)
, d_timeout(timeout)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
             bdlf::Function<void (*)(btlsc::TimedCbChannel*, int)>(functor);
}

TcpTimedCbAcceptor_Reg::TcpTimedCbAcceptor_Reg(
        const bdlf::Function<void (*)(btlsc::TimedCbChannel*, int)>& functor
      , int flags)
: d_isTimedChannel(true)
, d_isTimedOperation(false)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
             bdlf::Function<void (*)(btlsc::TimedCbChannel*, int)>(functor);
}

TcpTimedCbAcceptor_Reg::TcpTimedCbAcceptor_Reg(
        const bdlf::Function<void (*)(btlsc::CbChannel*, int)>& functor
      , int flags)
: d_isTimedChannel(false)
, d_isTimedOperation(false)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
             bdlf::Function<void (*)(btlsc::CbChannel*, int)>(functor);
}

TcpTimedCbAcceptor_Reg::TcpTimedCbAcceptor_Reg(
        const bsls::TimeInterval& timeout
      , const bdlf::Function<void (*)(btlsc::CbChannel*, int)>& functor
      , int flags)
: d_isTimedChannel(false)
, d_isTimedOperation(true)
, d_timeout(timeout)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
             bdlf::Function<void (*)(btlsc::CbChannel*, int)>(functor);
}

TcpTimedCbAcceptor_Reg::~TcpTimedCbAcceptor_Reg()
{
    if (d_isTimedChannel) {
        bdlf::Function<void (*)(btlsc::TimedCbChannel*, int)> *cb =
            (bdlf::Function<void (*)(btlsc::TimedCbChannel*, int)> *)
                (void *) d_cb.d_callbackArena;

        bslalg::ScalarDestructionPrimitives::destroy(cb);
    }
    else {
        bdlf::Function<void (*)(btlsc::CbChannel*, int)> *cb =
            (bdlf::Function<void (*)(btlsc::CbChannel*, int)> *)
                (void *) d_cb.d_callbackArena;

        bslalg::ScalarDestructionPrimitives::destroy(cb);
    }
}

// MANIPULATORS
inline
void TcpTimedCbAcceptor_Reg::invoke(int status) {
    if (d_isTimedChannel) {
        this->invokeTimed(NULL, status);
    }
    else {
        this->invoke(NULL, status);
    }
}

inline
void TcpTimedCbAcceptor_Reg::invoke(btlsc::CbChannel *channel, int status) {
    BSLS_ASSERT(!d_isTimedChannel);
    bdlf::Function<void (*)(btlsc::CbChannel*, int)> *cb =
             (bdlf::Function<void (*)(btlsc::CbChannel*, int)> *)
                (void *) d_cb.d_callbackArena;
    (*cb)(channel, status);
}

inline
void TcpTimedCbAcceptor_Reg::invokeTimed(btlsc::TimedCbChannel *channel,
                                         int                    status) {
    BSLS_ASSERT(d_isTimedChannel);
    bdlf::Function<void (*)(btlsc::TimedCbChannel*, int)> *cb =
        (bdlf::Function<void (*)(btlsc::TimedCbChannel*, int)> *)
            (void *) d_cb.d_callbackArena;
    (*cb)(channel, status);
}

// ACCESSORS
inline
int TcpTimedCbAcceptor_Reg::flags() const {
    return d_flags;
}

inline
bool TcpTimedCbAcceptor_Reg::isTimedResult() const {
    return d_isTimedChannel;
}

inline
bool TcpTimedCbAcceptor_Reg::isTimedOperation() const {
    return d_isTimedOperation;
}

inline
const bsls::TimeInterval& TcpTimedCbAcceptor_Reg::timeout() const {
    return d_timeout;
}

// ============================================================================
//                          END OF LOCAL DEFINITIONS
// ============================================================================

                         // ------------------------
                         // class TcpTimedCbAcceptor
                         // ------------------------

// PRIVATE MANIPULATORS
void TcpTimedCbAcceptor::acceptCb()
{
    BSLS_ASSERT(d_callbacks.size()); // At least one must be registered.

    // Accept the socket connection prior to getting the current request.

    btlso::StreamSocket<btlso::IPv4Address> *connection;
    int status = d_serverSocket_p->accept(&connection);

    d_currentRequest_p = d_callbacks.back();
    BSLS_ASSERT(d_currentRequest_p);

    // Deregister associated timer, if any.

    if (d_timerId) {
        d_manager_p->deregisterTimer(d_timerId);
        d_timerId = NULL;
    }
    BSLS_ASSERT(NULL == d_timerId);   // internal (in-method) invariant

    if (!status) {    // A new connection is accepted.
        BSLS_ASSERT(connection);

        if (d_currentRequest_p->isTimedResult()) {
            TcpTimedCbChannel *result =
                   new (d_channelPool) TcpTimedCbChannel(connection,
                                                                d_manager_p,
                                                                d_allocator_p);
            bsl::vector<btlsc::CbChannel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(),
                                static_cast<btlsc::CbChannel*>(result));
            d_channels.insert(idx, result);
            d_currentRequest_p->invokeTimed(result, 0);
        }
        else {
            TcpCbChannel *result =
                        new (d_channelPool) TcpCbChannel(connection,
                                                                d_manager_p,
                                                                d_allocator_p);
            bsl::vector<btlsc::CbChannel*>::iterator idx =
                bsl::lower_bound(d_channels.begin(), d_channels.end(),
                                 static_cast<btlsc::CbChannel*>(result));
            d_channels.insert(idx, result);
            d_currentRequest_p->invoke(result, 0);
        }
    }
    else {  // Existing connection - find out what happened
        if (status == btlso::SocketHandle::BTESO_ERROR_INTERRUPTED &&
            d_currentRequest_p->flags() & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
            d_currentRequest_p->invoke(btesc_Flag::BTESC_ASYNC_INTERRUPT);
        }
        else {
            if (status != btlso::SocketHandle::BTESO_ERROR_WOULDBLOCK) {
                d_currentRequest_p->invoke(status);
            }
            else {
                return; // EWOULDBLOCK -- ignored.
            }
        }
    }

    d_callbacks.pop_back();
    d_callbackPool.deleteObjectRaw(d_currentRequest_p);

    postCbCleanup();
}

void TcpTimedCbAcceptor::deallocateCb(btlsc::CbChannel *channel)
{
    BSLS_ASSERT(channel);

    TcpTimedCbChannel *c =
        dynamic_cast<TcpTimedCbChannel*>(channel);
    btlso::StreamSocket<btlso::IPv4Address> *s = NULL;

    if (c) {
        s = c->socket();
    }
    else {
        TcpCbChannel *c =
            dynamic_cast<TcpCbChannel*>(channel);
        BSLS_ASSERT(c);
        s = c->socket();
    }
    BSLS_ASSERT(s);
    channel->~CbChannel();  // This will cancel all pending requests.
    d_factory_p->deallocate(s);

    bsl::vector<btlsc::CbChannel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(), channel);
    BSLS_ASSERT(idx != d_channels.end() && *idx == channel);
    d_channels.erase(idx);
    d_channelPool.deleteObjectRaw(channel);
}

void TcpTimedCbAcceptor::postCbCleanup()
{
    if (d_callbacks.size()) {
        d_currentRequest_p = d_callbacks.back();
        if (d_currentRequest_p->isTimedOperation()) {
            const bsls::TimeInterval& timeout = d_currentRequest_p->timeout();
            d_timerId = d_manager_p->registerTimer(timeout, d_timeoutFunctor);
        }
        else {
            d_timerId = NULL; // internal (in-method) invariant
        }
    }
    else {
        if (d_serverSocket_p) {
            // If 'close' was called in the invoked callback, the listening
            // socket was deallocated, and 'd_serverSocket_p' is NULL.

            d_manager_p->deregisterSocketEvent(d_serverSocket_p->handle(),
                                               btlso::EventType::BTESO_ACCEPT);
        }
    }

    d_currentRequest_p = NULL; // internal class invariant
}

void TcpTimedCbAcceptor::timerCb()
{
    BSLS_ASSERT(d_callbacks.size()); // At least one must be registered.

    // The socket has already accepted in the prior 'acceptCb' callback.

    d_currentRequest_p = d_callbacks.back();
    BSLS_ASSERT(d_currentRequest_p);

    d_currentRequest_p->invoke(0);

    d_callbacks.pop_back();
    d_callbackPool.deleteObjectRaw(d_currentRequest_p);

    postCbCleanup();
}

// CREATORS
TcpTimedCbAcceptor::TcpTimedCbAcceptor(
        btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
        btlso::TimerEventManager                      *manager,
        bslma::Allocator                             *basicAllocator)
: d_callbackPool(sizeof(TcpTimedCbAcceptor_Reg), basicAllocator)
, d_channelPool(CHANNEL_SIZE, basicAllocator)
, d_callbacks(basicAllocator)
, d_channels(basicAllocator)
, d_manager_p(manager)
, d_factory_p(factory)
, d_serverSocket_p(NULL)
, d_isInvalidFlag(0)
, d_timerId(NULL)
, d_currentRequest_p(NULL)
, d_allocator_p(basicAllocator)
{
    d_acceptFunctor
        = bdlf::Function<void (*)()>(
              bdlf::MemFnUtil::memFn(&TcpTimedCbAcceptor::acceptCb, this)
            , d_allocator_p);

    d_timeoutFunctor
        = bdlf::Function<void (*)()>(
              bdlf::MemFnUtil::memFn(&TcpTimedCbAcceptor::timerCb, this)
            , d_allocator_p);
}

TcpTimedCbAcceptor::TcpTimedCbAcceptor(
        btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
        btlso::TimerEventManager                      *manager,
        int                                           numElements,
        bslma::Allocator                             *basicAllocator)
: d_callbackPool(sizeof(TcpTimedCbAcceptor_Reg), basicAllocator)
, d_channelPool(CHANNEL_SIZE,
                bsls::BlockGrowth::BSLS_CONSTANT,
                numElements,
                basicAllocator)
, d_callbacks(basicAllocator)
, d_channels(numElements, (btlsc::CbChannel *)0, basicAllocator)
, d_manager_p(manager)
, d_factory_p(factory)
, d_serverSocket_p(NULL)
, d_isInvalidFlag(0)
, d_timerId(NULL)
, d_currentRequest_p(NULL)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT(0 < numElements);
    d_acceptFunctor
        = bdlf::Function<void (*)()>(
              bdlf::MemFnUtil::memFn(&TcpTimedCbAcceptor::acceptCb, this)
            , d_allocator_p);

    d_timeoutFunctor
        = bdlf::Function<void (*)()>(
              bdlf::MemFnUtil::memFn(&TcpTimedCbAcceptor::timerCb, this)
            , d_allocator_p);
}

TcpTimedCbAcceptor::~TcpTimedCbAcceptor()
{
    invalidate();
    cancelAll();
    if (d_serverSocket_p) {
        close();
    }
    // Deallocate channels
    while (d_channels.size()) {
        btlsc::CbChannel *ch = d_channels[d_channels.size()-1];
        BSLS_ASSERT(ch);
        ch->invalidate();
        deallocateCb(ch);
    }
}

// MANIPULATORS
int TcpTimedCbAcceptor::allocate(const Callback& callback, int flags)
{
    if (d_isInvalidFlag) {
        return INVALID;
    }
    if (NULL == d_serverSocket_p) {
        callback(NULL, UNINITIALIZED);
        return UNINITIALIZED;
    }

    TcpTimedCbAcceptor_Reg *cb =
        new (d_callbackPool) TcpTimedCbAcceptor_Reg(callback, flags);

    // For safety, we push the callback before registering the socket event,
    // although when this component is used by 'btlmt_channelpool' (as
    // intended), the event ('d_acceptFunctor') should not be called until
    // *after* this function returns since both this function and the event are
    // processed in the I/O thread of the 'btlmt::ChannelPool' object.

    d_callbacks.push_front(cb);
    if (1 == d_callbacks.size()) {
        if (0 != d_manager_p->registerSocketEvent(d_serverSocket_p->handle(),
                                                 btlso::EventType::BTESO_ACCEPT,
                                                 d_acceptFunctor)) {
            cb->invoke(CANCELLED);
            d_callbacks.pop_back();
            d_callbackPool.deleteObjectRaw(cb);
            return INVALID;
        }
    }
    return SUCCESS;
}

int TcpTimedCbAcceptor::allocateTimed(const TimedCallback& callback, int flags)
{
    if (d_isInvalidFlag) {
        return INVALID;
    }
    if (NULL == d_serverSocket_p) {
        callback(NULL, UNINITIALIZED);
        return UNINITIALIZED;
    }

    TcpTimedCbAcceptor_Reg *cb =
        new (d_callbackPool) TcpTimedCbAcceptor_Reg(callback, flags);

    // For safety, we push the callback before registering the socket event,
    // although when this component is used by 'btlmt_channelpool' (as
    // intended), the event ('d_acceptFunctor') should not be called until
    // *after* this function returns since both this function and the event are
    // processed in the I/O thread of the 'btlmt::ChannelPool' object.

    d_callbacks.push_front(cb);
    if (1 == d_callbacks.size()) {
        if (0 != d_manager_p->registerSocketEvent(d_serverSocket_p->handle(),
                                                 btlso::EventType::BTESO_ACCEPT,
                                                 d_acceptFunctor)) {
            cb->invoke(CANCELLED);
            d_callbacks.pop_back();
            d_callbackPool.deleteObjectRaw(cb);
            return INVALID;
        }
    }
    return SUCCESS;
}

void TcpTimedCbAcceptor::cancelAll()
{
    if (d_currentRequest_p) {
        // A callback is active -- can't destroy current request.

        bsl::deque<TcpTimedCbAcceptor_Reg *> toBeCancelled(
                d_callbacks.begin(),
                d_callbacks.begin() + d_callbacks.size() - 1,
                d_allocator_p);

        d_callbacks.erase(d_callbacks.begin(),
                          d_callbacks.begin() + d_callbacks.size() - 1);
        BSLS_ASSERT(d_currentRequest_p == d_callbacks.back());

        int numToCancel = toBeCancelled.size();
        while (--numToCancel >= 0) {
            TcpTimedCbAcceptor_Reg *reg = toBeCancelled[numToCancel];
            BSLS_ASSERT(reg);
            reg->invoke(-1);
            d_callbackPool.deleteObjectRaw(reg);
        }
        BSLS_ASSERT(d_currentRequest_p == d_callbacks.back());
    }
    else {
        bsl::deque<TcpTimedCbAcceptor_Reg *>
                                     toBeCancelled(d_callbacks, d_allocator_p);
        d_callbacks.clear();
        int numToCancel = toBeCancelled.size();
        if (numToCancel) {
            d_manager_p->deregisterSocketEvent(d_serverSocket_p->handle(),
                                               btlso::EventType::BTESO_ACCEPT);

            if (d_timerId) {
                d_manager_p->deregisterTimer(d_timerId);
                d_timerId = NULL;
            }
        }

        while (--numToCancel >= 0) {
            TcpTimedCbAcceptor_Reg *reg = toBeCancelled[numToCancel];
            BSLS_ASSERT(reg);
            reg->invoke(-1);
            d_callbackPool.deleteObjectRaw(reg);
        }
    }
}

int TcpTimedCbAcceptor::close()
{
    enum { SUCCESS = 0 };
    BSLS_ASSERT(NULL != d_serverSocket_p);
    BSLS_ASSERT(d_serverAddress.portNumber());  // Address is valid.
    cancelAll();
    d_factory_p->deallocate(d_serverSocket_p);
    d_serverSocket_p = NULL;
    d_serverAddress = btlso::IPv4Address();
    return SUCCESS;
}

void TcpTimedCbAcceptor::deallocate(btlsc::CbChannel *channel)
{
    BSLS_ASSERT(channel);
    channel->invalidate();
    bdlf::Function<void (*)()> cb(
            bdlf::BindUtil::bindA(
                d_allocator_p
              , &TcpTimedCbAcceptor::deallocateCb
              , this
              , channel));
    d_manager_p->registerTimer(bdlt::CurrentTime::now(), cb);
}

void TcpTimedCbAcceptor::invalidate()
{
    d_isInvalidFlag = 1;
}

int TcpTimedCbAcceptor::open(const btlso::IPv4Address& endpoint,
                             int                       queueSize,
                             int                       reuseAddress)
{
    BSLS_ASSERT(0 < queueSize);
    BSLS_ASSERT(NULL == d_serverSocket_p);

    enum {
        REUSEADDRESS_FAILED  = -6,
        ALLOCATION_FAILED    = -5,
        BIND_FAILED          = -4,
        LISTEN_FAILED        = -3,
        LOCALINFO_FAILED     = -2,
        BLOCKMODE_FAILED     = -1,
        SUCCESS              =  0
    };

    d_serverSocket_p = d_factory_p->allocate();
    if (!d_serverSocket_p) {
        return ALLOCATION_FAILED;
    }

    if (0 !=
           d_serverSocket_p->setOption(btlso::SocketOptUtil::BTESO_SOCKETLEVEL,
                                       btlso::SocketOptUtil::BTESO_REUSEADDRESS,
                                       !!reuseAddress))
    {
        d_factory_p->deallocate(d_serverSocket_p);
        return REUSEADDRESS_FAILED;
    }

    if (0 != d_serverSocket_p->bind(endpoint)) {
        d_factory_p->deallocate(d_serverSocket_p);
        d_serverSocket_p = NULL;
        return BIND_FAILED;
    }

    if (0 != d_serverSocket_p->localAddress(&d_serverAddress)) {
        d_factory_p->deallocate(d_serverSocket_p);
        d_serverSocket_p = NULL;
        return BIND_FAILED;
    }
    BSLS_ASSERT(d_serverAddress.portNumber());

    if (0 != d_serverSocket_p->listen(queueSize)) {
        d_factory_p->deallocate(d_serverSocket_p);
        d_serverSocket_p = NULL;
        return LISTEN_FAILED;
    }
#ifndef BTLSO_PLATFORM_WIN_SOCKETS
    // Windows has a bug -- setting listening socket to non-blocking mode will
    // force subsequent 'accept' calls to return WSAEWOULDBLOCK *even when
    // connection is present*.

    if (0 != d_serverSocket_p->setBlockingMode(
                                           bteso_Flag::BTESO_NONBLOCKING_MODE))
    {
        d_factory_p->deallocate(d_serverSocket_p);
        d_serverSocket_p = NULL;
        return BLOCKMODE_FAILED;
    }
#endif
    return SUCCESS;
}

int TcpTimedCbAcceptor::setOption(int level, int option, int value)
{
    BSLS_ASSERT(d_serverSocket_p);
    return d_serverSocket_p->setOption(level, option, value);
}

int TcpTimedCbAcceptor::timedAllocate(const Callback&           callback,
                                      const bsls::TimeInterval& timeout,
                                      int                       flags)
{
    if (d_isInvalidFlag) {
        return INVALID;
    }
    if (NULL == d_serverSocket_p) {
        callback(NULL, UNINITIALIZED);
        return UNINITIALIZED;
    }
    // void *arena = d_callbackPool.allocate(); BSLS_ASSERT(arena);

    TcpTimedCbAcceptor_Reg *cb =
        new (d_callbackPool)
                     TcpTimedCbAcceptor_Reg(timeout, callback, flags);

    // For safety, we push the callback before registering the socket event,
    // although when this component is used by 'btlmt_channelpool' (as
    // intended), the event ('d_acceptFunctor') should not be called until
    // *after* this function returns since both this function and the event are
    // processed in the I/O thread of the 'btlmt::ChannelPool' object.

    d_callbacks.push_front(cb);
    if (1 == d_callbacks.size()) {
        d_timerId = d_manager_p->registerTimer(timeout, d_timeoutFunctor);
        BSLS_ASSERT(d_timerId);
        if (0 != d_manager_p->registerSocketEvent(d_serverSocket_p->handle(),
                                                 btlso::EventType::BTESO_ACCEPT,
                                                 d_acceptFunctor)) {
            cb->invoke(CANCELLED);
            d_callbacks.pop_back();
            d_callbackPool.deleteObjectRaw(cb);
            return INVALID;
        }
    }
    return SUCCESS;
}

int TcpTimedCbAcceptor::timedAllocateTimed(const TimedCallback&      callback,
                                           const bsls::TimeInterval& timeout,
                                           int                       flags)
{

    if (d_isInvalidFlag) {
        return INVALID;
    }
    if (NULL == d_serverSocket_p) {
        callback(NULL, UNINITIALIZED);
        return UNINITIALIZED;
    }

    TcpTimedCbAcceptor_Reg *cb =
        new (d_callbackPool)
                   TcpTimedCbAcceptor_Reg(timeout, callback, flags);

    // For safety, we push the callback before registering the socket event,
    // although when this component is used by 'btlmt_channelpool' (as
    // intended), the event ('d_acceptFunctor') shouldn't be called until
    // *after* this function returns since both this function and the event are
    // processed in the I/O thread of the 'btlmt::ChannelPool' object.

    d_callbacks.push_front(cb);
    if (1 == d_callbacks.size()) {
        d_timerId = d_manager_p->registerTimer(timeout, d_timeoutFunctor);
        BSLS_ASSERT(d_timerId);
        if (0 != d_manager_p->registerSocketEvent(d_serverSocket_p->handle(),
                                                 btlso::EventType::BTESO_ACCEPT,
                                                 d_acceptFunctor)) {
            cb->invoke(CANCELLED);
            d_callbacks.pop_back();
            d_callbackPool.deleteObjectRaw(cb);
            return INVALID;
        }
    }
    return SUCCESS;
}

// ACCESSORS
int
TcpTimedCbAcceptor::getOption(int *result, int level, int option) const
{
    BSLS_ASSERT(d_serverSocket_p);
    return d_serverSocket_p->socketOption(result, option, level);
}

int TcpTimedCbAcceptor::isInvalid() const
{
    return d_isInvalidFlag;
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
