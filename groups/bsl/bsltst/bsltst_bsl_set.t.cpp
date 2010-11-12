// bsltst_set.t.cpp                  -*-C++-*-
#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
#include <bsltst_bsl_set.h>

#include <set>
#include <vector>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace BloombergLP;
using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::atoi;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// [ 6] Multiset Manipulator / Accessor / Iterator Test
// [ 5] Set Manipulator / Accessor / Iterator Test
// [ 4] Multiset Memory Consumption and C'tor Test
// [ 3] Multiset Breathing Test
// [ 2] Set Memory Consumption & C'tor Test
// [ 1] Set Breathing Test
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//--------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define Q_(X) cout << "<| " #X " |>" << ", " << flush;  // Quote identifier lit
#define PV(X) { if (verbose) { cout << #X " = " << (X) << endl; }}
#define QV(X) { if (verbose) { cout << "<| " #X " |>" << endl; }}
#define PV_(X) { if (verbose) { cout << #X " = " << (X) << ", " << flush; }}
#define QV_(X) { if (verbose) { cout << "<| " #X " |>" << ", " << flush; }}
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBALS, TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose         = 0;
int veryVerbose     = 0;
int veryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// sameType -- returns 'true' if objects 'lhs' and 'rhs' are of exactly the
// same type and 'false' otherwise.  Note that C++ performs no implicit
// conversions on parameters to template functions.

template<typename LHSTYPE, typename RHSTYPE>
bool sameType(const LHSTYPE& lhs, const RHSTYPE rhs)
{
    (void) lhs;    (void) rhs;

    return false;
}

template<typename TYPE>
bool sameType(const TYPE& lhs, const TYPE& rhs)
{
    (void) lhs;    (void) rhs;

    return true;
}

// ----------------------------------------------------------------------------

template<typename TYPE>
bool usesBslmaAllocator(const TYPE& arg)
    // returns 'true' if 'TYPE' uses bslma_Allocator and 'false' otherwise.
{
    (void) arg;

    return bslalg_HasTrait<TYPE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE;
}

struct Cargo {
    // This struct dynamically allocates memory, for verifying that the
    // memory allocator is properly passed to elements within a container.

    int              d_i;
    void            *d_p;
    bslma_Allocator *d_alloc;

    enum { BULK_STORAGE = 4000 };

    BSLALG_DECLARE_NESTED_TRAITS(Cargo, bslalg_TypeTraitUsesBslmaAllocator);
      // Declare nested type traits for this class.

    explicit
    Cargo(int i, bslma_Allocator *a = 0) {
        QV_("Default:"); PV(a);
        d_i = i;
        d_alloc = bslma_Default::allocator(a);
        d_p = d_alloc->allocate(BULK_STORAGE);
    }
    Cargo(const Cargo& in, bslma_Allocator* a = 0) {
        QV_("Copy:"); PV(a);
        d_alloc = bslma_Default::allocator(a);
        d_i = in.d_i;
        d_p = d_alloc->allocate(BULK_STORAGE);
        std::memcpy(d_p, in.d_p, BULK_STORAGE);
    }
    Cargo& operator=(const Cargo& in) {
        QV("Assign:");
        d_i = in.d_i;
        std::memcpy(d_p, in.d_p, BULK_STORAGE);
        return *this;
    }
    ~Cargo() {
        d_alloc->deallocate(d_p);
    }
};

// ----------------------------------------------------------------------------

bool operator<(const Cargo& lhs, const Cargo& rhs)
{
    return lhs.d_i < rhs.d_i;
}

struct Greaterp {
    bool operator()(const Cargo& lhs, const Cargo& rhs) const {
        return rhs < lhs;
    }
};

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator ta;
    bslma_TestAllocator tda;

    bslma_DefaultAllocatorGuard defaultGuard(&tda);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // MULTISET MANIPULATOR / ACCESSOR TEST
        //
        // Concerns:
        //   That the basic manipulators, accessors and iterators work as
        //   documented.
        //
        // Plan:
        //   First, repeat all the tests done on set that will also work on
        //   multiset.  Then branch out into things that only multisets can
        //   do.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                         "\nMULTISET MANIPULATOR / ACCESSOR / ITERATOR TEST\n"
                           "===============================================\n";

        bsl::multiset<int> s(&ta);
        bsl::multiset<int>::iterator it, itb, itc, itd;

        for (int i = 0; i < 20; ++i) {
            it = s.insert(i);
            ASSERT(s.end() != it);
            ASSERT(i == *it);
        }
        ASSERT(s.size() == 20);

//      for (int i = 0; i < 20; ++i) {
//          bsl::pair<bsl::set<int>::iterator,bool> p = s.insert(i);
//          ASSERT(!p.second);
//          ASSERT(*p.first == i);
//      }
//      ASSERT(s.size() == 20);

        it = s.find(5);
        ASSERT(s.end() != it);
        itb = s.insert(it, 20);
        ASSERT(s.end() != itb);
        ASSERT(it != itb);
        ASSERT(s.size() == 21);
        ASSERT(s.count(20));

        int j;
        for (it = s.begin(), j = 0; s.end() != it; ++it, ++j) {
            ASSERT(*it == j);
        }
        ASSERT(21 == j);

        s.erase(20);
        ASSERT(s.size() == 20);
        for (it = s.begin(), j = 0; s.end() != it; ++it, ++j) {
            ASSERT(*it == j);
        }
        ASSERT(20 == j);

        bsl::multiset<int> t(s, &ta);

        ASSERT(s == t);

        t.clear();

        for (int i = 0; i < 100; ++i) {
            t.insert(i);
        }
        ASSERT(t.size() == 100);

        itb = t.find(20);
        itc = t.find(30);
        s.insert(itb, itc);
        ASSERT(t.size() == 100);
        ASSERT(s.size() == 30);

        for (it = s.begin(), j = 0; s.end() != it; ++it, ++j) {
            ASSERT(*it == j);
        }
        ASSERT(30 == j);

        bsl::multiset<int>::reverse_iterator rit;
        for (rit = s.rbegin(), j = 29; s.rend() != rit; ++rit, --j) {
            ASSERT(*rit == j);
        }
        ASSERT(-1 == j);

        it = s.lower_bound(15);
        ASSERT(15 == *it);

        it = s.lower_bound(-5);
        ASSERT(0 == *it);

        it = s.upper_bound(15);
        ASSERT(16 == *it);

        it = s.upper_bound(29);
        ASSERT(s.end() == it);

        {
            bsl::pair<bsl::multiset<int>::iterator,
                      bsl::multiset<int>::iterator> pr = s.equal_range(12);

            ASSERT(12 == *pr.first);
            ASSERT(13 == *pr.second);

            pr = s.equal_range(40);
            ASSERT(s.end() == pr.first);
            ASSERT(s.end() == pr.second);

            pr = s.equal_range(-40);
            ASSERT(s.begin() == pr.first);
            ASSERT(s.begin() == pr.second);

            s.erase(10);

            pr = s.equal_range(10);
            ASSERT(11 == *pr.first);
            ASSERT(11 == *pr.second);

            s.insert(10);
        }

        s.clear();
        t.clear();

        s.insert(20);
        t.insert(21);

        ASSERT(s <  t);
        ASSERT(s <= t);
        ASSERT(s != t);
        ASSERT(!(s >  t));
        ASSERT(!(s >= t));
        ASSERT(!(s == t));

        s.insert(21);
        t.insert(20);

        ASSERT(s == t);
        ASSERT(!(s != t));
        ASSERT(s <= t);
        ASSERT(!(s < t));
        ASSERT(s >= t);
        ASSERT(!(s > t));

        s.clear();
        t.clear();

        for (int i = 0; i < 10; ++i) {
            s.insert(i);
        }
        for (int i = 3; i <= 7; ++i) {
            s.insert(i);
        }

        ASSERT(0 == s.count(-5));
        ASSERT(0 == s.count(20));
        ASSERT(1 == s.count( 0));
        ASSERT(2 == s.count( 3));
        ASSERT(2 == s.count( 7));

        {
            bsl::pair<bsl::multiset<int>::iterator,
                      bsl::multiset<int>::iterator> pr = s.equal_range(3);

            it = s.find(2);
            ++it;

            ASSERT(it == pr.first);
            ASSERT(it != pr.second);

            for (j = 3; j <= 7; ++j) {
                ASSERT(j == *it++);
                ASSERT(j == *it++);
                if (3 == j) {
                    ASSERT(it == pr.second);
                }
            }
            ASSERT(8 == *it);

            pr = s.equal_range(40);
            ASSERT(s.end() == pr.first);
            ASSERT(s.end() == pr.second);

            pr = s.equal_range(-40);
            ASSERT(s.begin() == pr.first);
            ASSERT(s.begin() == pr.second);

            s.erase(5);         // note this erases *ALL* elements with key 5
            ASSERT(0 == s.count(5));

            pr = s.equal_range(5);
            ASSERT(6 == *pr.first);
            ASSERT(6 == *pr.second);

            s.erase(s.find(4), s.find(7));
            ASSERT(2 == s.count(3));
            ASSERT(0 == s.count(4));
            ASSERT(0 == s.count(5));
            ASSERT(0 == s.count(6));
            ASSERT(2 == s.count(7));

            pr = s.equal_range(5);
            ASSERT(7 == *pr.first);
            ASSERT(7 == *pr.second);

            s.clear();
            t.clear();
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // SET MANIPULATOR / ACCESSOR / ITERATOR TEST
        //
        // Concerns:
        //   That the basic manipulators, accessors, and iterators work as
        //   documented.
        //
        // Plan:
        //   Create a couple of containers and store things in them, verify
        //   with accessors that the contents are as expected.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nSET MANIPULATOR / ACCESSOR / ITERATOR TEST\n"
                               "==========================================\n";

        bsl::set<int> s(&ta);

        for (int i = 0; i < 20; ++i) {
            bsl::pair<bsl::set<int>::iterator,bool> p = s.insert(i);
            ASSERT(p.second);
            ASSERT(*p.first == i);
        }
        ASSERT(s.size() == 20);

        for (int i = 0; i < 20; ++i) {
            bsl::pair<bsl::set<int>::iterator,bool> p = s.insert(i);
            ASSERT(!p.second);
            ASSERT(*p.first == i);
        }
        ASSERT(s.size() == 20);

        bsl::set<int>::iterator it, itb, itc, itd;
        it = s.find(5);
        ASSERT(s.end() != it);
        itb = s.insert(it, 20);
        ASSERT(s.end() != itb);
        ASSERT(it != itb);
        ASSERT(s.size() == 21);
        ASSERT(s.count(20));

        int j;
        for (it = s.begin(), j = 0; s.end() != it; ++it, ++j) {
            ASSERT(*it == j);
        }
        ASSERT(21 == j);

        s.erase(20);
        ASSERT(s.size() == 20);
        for (it = s.begin(), j = 0; s.end() != it; ++it, ++j) {
            ASSERT(*it == j);
        }
        ASSERT(20 == j);

        bsl::set<int> t(s, &ta);

        ASSERT(s == t);

        t.clear();

        for (int i = 0; i < 100; ++i) {
            t.insert(i);
        }
        ASSERT(t.size() == 100);

        itb = t.find(20);
        itc = t.find(30);
        s.insert(itb, itc);
        ASSERT(t.size() == 100);
        ASSERT(s.size() == 30);

        for (it = s.begin(), j = 0; s.end() != it; ++it, ++j) {
            ASSERT(*it == j);
        }
        ASSERT(30 == j);

        bsl::set<int>::reverse_iterator rit;
        for (rit = s.rbegin(), j = 29; s.rend() != rit; ++rit, --j) {
            ASSERT(*rit == j);
        }
        ASSERT(-1 == j);

        it = s.lower_bound(15);
        ASSERT(15 == *it);

        it = s.lower_bound(-5);
        ASSERT(0 == *it);

        it = s.upper_bound(15);
        ASSERT(16 == *it);

        it = s.upper_bound(29);
        ASSERT(s.end() == it);

        {
            bsl::pair<bsl::set<int>::iterator, bsl::set<int>::iterator> pr =
                                                             s.equal_range(12);
            ASSERT(12 == *pr.first);
            ASSERT(13 == *pr.second);
        }

        s.clear();
        t.clear();

        s.insert(20);
        t.insert(21);

        ASSERT(s <  t);
        ASSERT(s <= t);
        ASSERT(s != t);
        ASSERT(!(s >  t));
        ASSERT(!(s >= t));
        ASSERT(!(s == t));

        s.insert(21);
        t.insert(20);

        ASSERT(s == t);
        ASSERT(!(s != t));
        ASSERT(s <= t);
        ASSERT(!(s < t));
        ASSERT(s >= t);
        ASSERT(!(s > t));
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // MULTISET MEMORY CONSUMPTION AND C'TOR TEST
        //
        // Concerns:
        //   That the memory allocator is getting properly passed to objects
        //   in the container, and that all c'tors compile.
        //
        // Plan:
        //   Store the 'Cargo' struct, which dynamically allocates large
        //   amounts of memory, and verify that large amounts of memory are
        //   consumed in the allocator that was passed to the container.
        //   Repeat this test with all c'tors in the standard.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                        "\nMULTISET MEMORY CONSUMPTION AND CONSTRUCTOR TEST\n"
                          "================================================\n";

        bsl::less<Cargo> lessCargo;
        Greaterp    greaterCargo;

        {
            bsl::multiset<Cargo> ms;

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 50; ++i) {
                ms.insert(Cargo(i));
            }
            for (int i = 0; i < 50; ++i) {
                ms.insert(Cargo(50));
            }

            for (int i = 0; i < 100; ++i) {
                ASSERT(ms.count(Cargo(i)) == (50 == i ? 50 : (i < 50)));
            }

            ASSERT(ta.numBytesInUse() == memUsed);
            ASSERT(tda.numBytesInUse() >=
                                   defaultMemUsed + 100 * Cargo::BULK_STORAGE);
        }

        {
            bsl::multiset<Cargo> ms(lessCargo);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 50; ++i) {
                ms.insert(Cargo(i));
            }
            for (int i = 0; i < 50; ++i) {
                ms.insert(Cargo(50));
            }

            for (int i = 0; i < 100; ++i) {
                ASSERT(ms.count(Cargo(i)) == (50 == i ? 50 : (i < 50)));
            }

            ASSERT(ta.numBytesInUse() == memUsed);
            ASSERT(tda.numBytesInUse() >=
                                   defaultMemUsed + 100 * Cargo::BULK_STORAGE);
        }

        {
            bsl::multiset<Cargo, Greaterp> ms(greaterCargo, &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 50; ++i) {
                ms.insert(Cargo(i));
            }
            for (int i = 0; i < 50; ++i) {
                ms.insert(Cargo(50));
            }

            for (int i = 0; i < 100; ++i) {
                ASSERT(ms.count(Cargo(i)) == (50 == i ? 50 : (i < 50)));
            }

            ASSERT(ta.numBytesInUse() >= memUsed + 100 * Cargo::BULK_STORAGE);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::vector<Cargo> vC;

            for (int i = 0; i < 50; ++i) {
                vC.push_back(Cargo(i));
            }

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::multiset<Cargo> ms(vC.begin(), vC.end());

            for (int i = 0; i < 50; ++i) {
                ms.insert(Cargo(50));
            }

            for (int i = 0; i < 100; ++i) {
                ASSERT(ms.count(Cargo(i)) == (50 == i ? 50 : (i < 50)));
            }

            ASSERT(ta.numBytesInUse() == memUsed);
            ASSERT(tda.numBytesInUse() >=
                                   defaultMemUsed + 100 * Cargo::BULK_STORAGE);
        }

        {
            bsl::vector<Cargo> vC;

            for (int i = 0; i < 50; ++i) {
                vC.push_back(Cargo(i));
            }

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::multiset<Cargo, Greaterp>
                                        ms(vC.begin(), vC.end(), greaterCargo);

            for (int i = 0; i < 50; ++i) {
                ms.insert(Cargo(50));
            }

            for (int i = 0; i < 100; ++i) {
                ASSERT(ms.count(Cargo(i)) == (50 == i ? 50 : (i < 50)));
            }

            ASSERT(ta.numBytesInUse() == memUsed);
            ASSERT(tda.numBytesInUse() >=
                                   defaultMemUsed + 100 * Cargo::BULK_STORAGE);
        }

        {
            bsl::vector<Cargo> vC;

            for (int i = 0; i < 50; ++i) {
                vC.push_back(Cargo(i));
            }

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::multiset<Cargo, Greaterp>
                                   ms(vC.begin(), vC.end(), greaterCargo, &ta);

            for (int i = 0; i < 50; ++i) {
                ms.insert(Cargo(50));
            }

            for (int i = 0; i < 100; ++i) {
                ASSERT(ms.count(Cargo(i)) == (50 == i ? 50 : (i < 50)));
            }

            ASSERT(ta.numBytesInUse() >= memUsed + 100 * Cargo::BULK_STORAGE);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::multiset<Cargo> msA;
            const bsl::multiset<Cargo>& MSA = msA;

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 50; ++i) {
                msA.insert(Cargo(i));
            }
            for (int i = 0; i < 50; ++i) {
                msA.insert(Cargo(50));
            }

            for (int i = 0; i < 100; ++i) {
                ASSERT(MSA.count(Cargo(i)) == (50 == i ? 50 : (i < 50)));
            }

            bsl::multiset<Cargo> msB(MSA);

            for (int i = 0; i < 100; ++i) {
                ASSERT(msB.count(Cargo(i)) == (50 == i ? 50 : (i < 50)));
            }

            ASSERT(ta.numBytesInUse() == memUsed);
            ASSERT(tda.numBytesInUse() >=
                               defaultMemUsed + 2 * 100 * Cargo::BULK_STORAGE);
        }

        {
            bsl::multiset<Cargo> ms(&ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 50; ++i) {
                ms.insert(Cargo(i));
            }
            for (int i = 0; i < 50; ++i) {
                ms.insert(Cargo(50));
            }

            for (int i = 0; i < 100; ++i) {
                ASSERT(ms.count(Cargo(i)) == (50 == i ? 50 : (i < 50)));
            }

            ASSERT(ta.numBytesInUse() >= memUsed + 100 * Cargo::BULK_STORAGE);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::multiset<Cargo> msA;
            const bsl::multiset<Cargo>& MSA = msA;

            for (int i = 0; i < 50; ++i) {
                msA.insert(Cargo(i));
            }
            for (int i = 0; i < 50; ++i) {
                msA.insert(Cargo(50));
            }

            for (int i = 0; i < 100; ++i) {
                ASSERT(MSA.count(Cargo(i)) == (50 == i ? 50 : (i < 50)));
            }

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::multiset<Cargo> msB(MSA, &ta);

            for (int i = 0; i < 100; ++i) {
                ASSERT(msB.count(Cargo(i)) == (50 == i ? 50 : (i < 50)));
            }

            ASSERT(ta.numBytesInUse() >= memUsed + 100 * Cargo::BULK_STORAGE);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // MULTISET BREATHING TEST
        //
        // Concerns:
        //   Verify that the container and its iterator are the same type in
        //   bsl and std, and that the native_std containter is different
        //   from the std container.  Also verify that the bsl container
        //   uses the bslma allocator, and that the native_std container
        //   doesn't.  Verify that iterators don't use the bslma allocator.
        //
        // Plan:
        //   Use the templates 'sameType' and 'usesBslmaAllocator' (defined
        //   in this file) to do the testing.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMULTISET BREATHING TEST\n"
                               "=======================\n";

        bsl::multiset<int> ms(&ta);

        ASSERT(true  == sameType(ms, std::multiset<int>()));
        ASSERT(true  == usesBslmaAllocator(bsl::multiset<int>()));
        ASSERT(true  == usesBslmaAllocator(std::multiset<int>()));
        ASSERT(false == usesBslmaAllocator(native_std::multiset<int>()));

        ASSERT(true  == sameType(bsl::multiset<int>::iterator(),
                                 std::multiset<int>::iterator()));
        ASSERT(false == sameType(bsl::multiset<int>::iterator(),
                          native_std::multiset<int>::iterator()));

        ASSERT(false == usesBslmaAllocator(bsl::multiset<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(std::multiset<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(
                                    native_std::multiset<int>::iterator()));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // SET MEMORY CONSUMPTION AND CONSTRUCTOR TEST
        //
        // Concerns:
        //   That the memory allocator is getting properly passed to objects
        //   in the container, and that all c'tors compile.
        //
        // Plan:
        //   Store the 'Cargo' struct, which dynamically allocates large
        //   amounts of memory, and verify that large amounts of memory are
        //   consumed in the allocator that was passed to the container, and
        //   try this for all allocators.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nSET MEMORY CONSUMPTION AND CONSTRUCTOR TEST\n"
                               "===========================================\n";

        bsl::less<Cargo> lessCargo;
        Greaterp greaterCargo;

        {
            bsl::set<Cargo> s;

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 100; ++i) {
                s.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() == memUsed);
            ASSERT(tda.numBytesInUse() >=
                                   defaultMemUsed + 100 * Cargo::BULK_STORAGE);
        }

        {
            bsl::set<Cargo, Greaterp> s(greaterCargo);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 100; ++i) {
                s.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() == memUsed);
            ASSERT(tda.numBytesInUse() >=
                                   defaultMemUsed + 100 * Cargo::BULK_STORAGE);
        }

        {
            bsl::set<Cargo, Greaterp> s(greaterCargo, &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 100; ++i) {
                s.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() >= memUsed + 100 * Cargo::BULK_STORAGE);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::vector<Cargo> vC;

            for (int i = 0; i < 10; ++i) {
                vC.push_back(Cargo(i));
            }

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::set<Cargo> s(vC.begin(), vC.end());
            for (int i = 10; i < 100; ++i) {
                s.insert(Cargo(i));
            }

            for (int i = 0; i < 200; ++i) {
                ASSERT(s.count(Cargo(i)) == (i < 100));
            }

            ASSERT(ta.numBytesInUse() == memUsed);
            ASSERT(tda.numBytesInUse() >=
                                   defaultMemUsed + 100 * Cargo::BULK_STORAGE);
        }

        {
            bsl::vector<Cargo> vC;

            for (int i = 0; i < 10; ++i) {
                vC.push_back(Cargo(i));
            }

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::set<Cargo> s(vC.begin(), vC.end(), lessCargo);
            for (int i = 10; i < 100; ++i) {
                s.insert(Cargo(i));
            }

            for (int i = 0; i < 200; ++i) {
                ASSERT(s.count(Cargo(i)) == (i < 100));
            }

            ASSERT(ta.numBytesInUse() == memUsed);
            ASSERT(tda.numBytesInUse() >=
                                   defaultMemUsed + 100 * Cargo::BULK_STORAGE);
        }

        {
            bsl::vector<Cargo> vC;

            for (int i = 0; i < 10; ++i) {
                vC.push_back(Cargo(i));
            }

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::set<Cargo> s(vC.begin(), vC.end(), lessCargo, &ta);
            for (int i = 10; i < 100; ++i) {
                s.insert(Cargo(i));
            }

            for (int i = 0; i < 200; ++i) {
                ASSERT(s.count(Cargo(i)) == (i < 100));
            }

            ASSERT(ta.numBytesInUse() >= memUsed + 100 * Cargo::BULK_STORAGE);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::set<Cargo> sA;    const bsl::set<Cargo>& SA = sA;

            for (int i = 0; i < 100; ++i) {
                sA.insert(Cargo(i));
            }

            for (int i = 0; i < 200; ++i) {
                ASSERT(SA.count(Cargo(i)) == (i < 100));
            }

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::set<Cargo> sB(SA);

            for (int i = 0; i < 200; ++i) {
                ASSERT(sB.count(Cargo(i)) == (i < 100));
            }

            ASSERT(ta.numBytesInUse() == memUsed);
            ASSERT(tda.numBytesInUse() >=
                                   defaultMemUsed + 100 * Cargo::BULK_STORAGE);
        }

        {
            bsl::set<Cargo> s(&ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 100; ++i) {
                s.insert(Cargo(i));
            }

            for (int i = 0; i < 200; ++i) {
                ASSERT(s.count(Cargo(i)) == (i < 100));
            }

            ASSERT(ta.numBytesInUse() >= memUsed + 100 * Cargo::BULK_STORAGE);
            ASSERT(defaultMemUsed == tda.numBytesInUse());
        }

        {
            bsl::set<Cargo> sA;    const bsl::set<Cargo>& SA = sA;

            for (int i = 0; i < 100; ++i) {
                sA.insert(Cargo(i));
            }

            for (int i = 0; i < 200; ++i) {
                ASSERT(SA.count(Cargo(i)) == (i < 100));
            }

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::set<Cargo> sB(SA, &ta);

            for (int i = 0; i < 200; ++i) {
                ASSERT(sB.count(Cargo(i)) == (i < 100));
            }

            ASSERT(ta.numBytesInUse() >= memUsed + 100 * Cargo::BULK_STORAGE);
            ASSERT(defaultMemUsed == tda.numBytesInUse());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // SET BREATHING TEST
        //
        // Concerns:
        //   Verify that the container and its iterator are the same type in
        //   bsl and std, and that the native_std containter is different
        //   from the std container.  Also verify that the bsl container
        //   uses the bslma allocator, and that the native_std container
        //   doesn't.  Verify that iterators don't use the bslma allocator.
        //
        // Plan:
        //   Use the templates 'sameType' and 'usesBslmaAllocator' (defined
        //   in this file) to do the testing.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nSET BREATHING TEST\n"
                               "==================\n";

        bsl::set<int> s(&ta);

        ASSERT(true  == sameType(s, std::set<int>()));
        ASSERT(true  == usesBslmaAllocator(bsl::set<int>()));
        ASSERT(true  == usesBslmaAllocator(std::set<int>()));
        ASSERT(false == usesBslmaAllocator(native_std::set<int>()));

        ASSERT(true  == sameType(bsl::set<int>::iterator(),
                                 std::set<int>::iterator()));
        ASSERT(false == sameType(bsl::set<int>::iterator(),
                          native_std::set<int>::iterator()));

        ASSERT(false == usesBslmaAllocator(bsl::set<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(std::set<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(
                                    native_std::set<int>::iterator()));
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
