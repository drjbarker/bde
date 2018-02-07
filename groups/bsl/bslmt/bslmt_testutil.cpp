// bslmt_testutil.cpp                                                 -*-C++-*-
#include <bslmt_testutil.h>

#include <bslmt_once.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>

char bslmt_testutil_guard_object = 0;

namespace BloombergLP {
namespace bslmt {

                              // ---------------
                              // struct TestUtil
                              // ---------------

// CLASS DATA
TestUtil::Func TestUtil::s_func = 0;

// CLASS METHODS
void *TestUtil::callFunc(void *arg)
{
    BSLS_ASSERT(s_func);

    return (*s_func)(arg);
}

bool TestUtil::compareText(bslstl::StringRef a,
                           bslstl::StringRef b,
                           bsl::ostream&     errorStream)
{
    bslstl::StringRef::size_type i;

    for (i = 0; i < a.length() && i < b.length(); ++i) {
        if (a[i] != b[i]) {
            errorStream << "a: \"" << a << "\"\n"
                        << "b: \"" << b << "\"\n"
                        << "Strings differ at index (" << i << ") "
                        << "a[i] = " << a[i] << "(" << (int)a[i] << ") "
                        << "b[i] = " << b[i] << "(" << (int)b[i] << ")"
                        << bsl::endl;
            return false;                                             // RETURN
        }
    }

    if (i < b.length()) {
        errorStream << "a: \"" << a << "\"\n"
                    << "b: \"" << b << "\"\n"
                    << "Strings differ at index (" << i << ") "
                    << "a[i] = END-OF-STRING "
                    << "b[i] = " << b[i] << "(" << (int)b[i] << ")"
                    << bsl::endl;
        return false;                                                 // RETURN
    }

    if (i < a.length()) {
        errorStream << "a: \"" << a << "\"\n"
                    << "b: \"" << b << "\"\n"
                    << "Strings differ at index (" << i << ") "
                    << "a[i] = " << a[i] << "(" << (int)a[i] << ") "
                    << "b[i] = END-OF-STRING"
                    << bsl::endl;
        return false;                                                 // RETURN
    }

    return true;
}

bslmt::Mutex& TestUtil::outputMutexSingleton()
{
    static bslmt::Mutex *mutex_p;

    BSLMT_ONCE_DO {
        static bslmt::Mutex mutex;

        mutex_p = &mutex;
    }

    return *mutex_p;
}

void TestUtil::setFunc(TestUtil::Func func)
{
    s_func = func;
}

                          // ---------------------------
                          // class TestUtil::GuardObject
                          // ---------------------------

TestUtil::GuardObject::GuardObject()
: d_mutex(&TestUtil::outputMutexSingleton())
{
    d_mutex->lock();
}

TestUtil::GuardObject::~GuardObject()
{
    d_mutex->unlock();
}

                          // ---------------------------
                          // class TestUtil::NestedGuard
                          // ---------------------------

// CREATORS
TestUtil::NestedGuard::NestedGuard(char *)
: d_mutex(&TestUtil::outputMutexSingleton())
{
    d_mutex->lock();
}

TestUtil::NestedGuard::NestedGuard(GuardObject *)
: d_mutex(0)
{}

TestUtil::NestedGuard::~NestedGuard()
{
    if (d_mutex) {
        d_mutex->unlock();
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
