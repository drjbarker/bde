// bdlb_transformiterator.h                                           -*-C++-*-
#ifndef INCLUDED_BDLB_TRANSFORMITERATOR
#define INCLUDED_BDLB_TRANSFORMITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: To be the best transform iterator possible, given the constraints.
//
//@CLASSES:
// bdlb::TransformIterator: Be a darn good transform iterator.
//
//@DESCRIPTION: 'bdlb::TransformIterator' iterates and transforms.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:
/// - - - - -
//
//..
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_CONDITIONAL
#include <bslmf_conditional.h>
#endif

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

#if __cplusplus < 201103L

#ifndef INCLUDED_BSLMF_RESULTTYPE
#include <bslmf_resulttype.h>
#endif

#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

namespace BloombergLP {
namespace bdlb {

                       // =============================
                       // class bdlb::TransformIterator
                       // =============================

template <class FUNCTOR, class ITERATOR>
class TransformIterator;

                   // =====================================
                   // struct bdlb::TransformIterator_Traits
                   // =====================================

template <class FUNCTOR, class ITERATOR>
struct TransformIterator_Traits {
    // This component-private class defines various types that will be used in
    // the implementation of the transform iterator.

    // PUBLIC TYPES

    // Define the result type returned by the functor.  This is not necessarily
    // the same type as the dereference of the iterator.
#if __cplusplus < 201103L
    // In C++03, the functor must have a 'result_type' type member.  The
    // specializations below transform function pointers to 'bsl::function' so
    // this works for those types as well.
    typedef typename bslmf::ResultType<FUNCTOR>::type functor_result_type;
#else
    // In C++11, the result type can be determined automatically.  Note that
    // various iterations of the language standard might want to instead use
    // 'std::result_of' or 'std::invoke_result' (which have been variously
    // added and then deprecated), but the following works from C++11 onwards.
    typedef decltype(bsl::declval<FUNCTOR>()(*bsl::declval<ITERATOR>()))
                                                      functor_result_type;
#endif

    // Define the iterator traits class of the underlying iterator.
    typedef typename bsl::iterator_traits<ITERATOR> base_iterator_traits;

    // Define the iterator category of the transform iterator.  If the functor
    // returns a reference type, we pass through the iterator category of the
    // underlying iterator, otherwise we use the input iterator tag (because
    // all the other tags require that dereferencing produces a reference).
    typedef typename bsl::conditional<
        bsl::is_reference<functor_result_type>::value,
        typename base_iterator_traits::iterator_category,
        bsl::input_iterator_tag>::type iterator_category;

    // Define the remaining standard types of the transform iterator.
    typedef typename base_iterator_traits::difference_type difference_type;
    typedef typename bsl::remove_cv<
        typename bsl::remove_reference<functor_result_type>::type>::type
                                                                value_type;
    typedef functor_result_type                                 reference;
    typedef typename bsl::remove_reference<functor_result_type>::type *pointer;

    // Define the standard iterator specialization that will apply to the
    // transform iterator.
    typedef bsl::iterator<iterator_category,
                          difference_type,
                          value_type,
                          reference,
                          pointer>
        iterator_type;
};

// Specialize the transform iterator traits template for functors that are
// function or function pointer types.  It is sufficient to inherit from the
// version of the traits class that corresponds to a 'bsl::function' of the
// function type parameter.

template <class RESULT, class ARGUMENT, class ITERATOR>
struct TransformIterator_Traits<RESULT (*)(ARGUMENT), ITERATOR>
: public TransformIterator_Traits<bsl::function<RESULT(ARGUMENT)>, ITERATOR> {
};

template <class RESULT, class ARGUMENT, class ITERATOR>
struct TransformIterator_Traits<RESULT(ARGUMENT), ITERATOR>
: public TransformIterator_Traits<bsl::function<RESULT(ARGUMENT)>, ITERATOR> {
};


// The transform iterator uses allocators only if at least one of its iterator
// or its functor do.  Retrieving the allocator of the transform iterator, if
// it exists, therefore can be implemented by querying subobjects.  We will use
// implementation inheritance to supply the transform iterator with an
// allocator method that will exist only when necessary.

          // ========================================================
          // struct bdlb::TransformIterator_AllocatorOfIteratorMethod
          // ========================================================

template <class BASE, bool>
struct TransformIterator_AllocatorOfIteratorMethod {
    // The 'TransformIterator_AllocatorOfIteratorMethod' class template has an
    // allocator method when its boolean template parameter is 'true', which
    // will be made to be the case when the iterator of the transform iterator
    // uses allocators.
};

template <class BASE>
struct TransformIterator_AllocatorOfIteratorMethod<BASE, true>
{
    // PUBLIC ACCESSORS
    bslma::Allocator *allocator() const;
        // Assume that this class is a base class of a transform iterator, and
        // return the allocator of the iterator of the class.
};

          // =======================================================
          // struct bdlb::TransformIterator_AllocatorOfFunctorMethod
          // =======================================================

template <class BASE, bool>
struct TransformIterator_AllocatorOfFunctorMethod {
    // The 'TransformIterator_AllocatorOfFunctorMethod' class template has an
    // allocator method when its boolean template parameter is 'true', which
    // will be made to be the case when the iterator of the transform iterator
    // does not use allocators and the functor of the transform iterator uses
    // allocators.
};

template <class BASE>
struct TransformIterator_AllocatorOfFunctorMethod<BASE, true>
{
    // PUBLIC ACCESSORS
    bslma::Allocator *allocator() const;
        // Assume that this class is a base class of a transform iterator, and
        // return the allocator of the functor of the class.
};

template <class FUNCTOR, class ITERATOR>
class TransformIterator
: TransformIterator_Traits<FUNCTOR, ITERATOR>::iterator_type,
  TransformIterator_AllocatorOfIteratorMethod<
      TransformIterator<FUNCTOR, ITERATOR>,
      bslma::UsesBslmaAllocator<ITERATOR>::value>,
  TransformIterator_AllocatorOfFunctorMethod<
      TransformIterator<FUNCTOR, ITERATOR>,
      !bslma::UsesBslmaAllocator<ITERATOR>::value &&
          bslma::UsesBslmaAllocator<FUNCTOR>::value> {
    // The transform iterator class itself.  Its job is to hold a functor and
    // an iterator, pass through all iterator-related operations to the held
    // iterator, and on dereference, call the functor on the result of
    // dereferencing the iterator and return the result of the call instead.
  private:
    // PRIVATE TYPES
    typedef TransformIterator_Traits<FUNCTOR, ITERATOR> Traits;
    typedef typename Traits::iterator_type              iterator_type;

    // PRIVATE DATA
    bslalg::ConstructorProxy<ITERATOR> d_iteratorProxy;
    bslalg::ConstructorProxy<FUNCTOR>  d_functorProxy;

  public:
    // PUBLIC TYPES

    // 'difference_type' is needed for 'operator[]' declarations.  The other
    // iterator types will be available to clients via inheritance.
    using typename iterator_type::difference_type;

    // PUBLIC CREATORS
    TransformIterator();
        // Construct an object of this type.

    explicit TransformIterator(bslma::Allocator *basicAllocator);
        // Construct an object of this type using the specified
        // 'basicAllocator' to allocate memory.

    TransformIterator(const ITERATOR&   iterator,
                      FUNCTOR           functor,
                      bslma::Allocator *basicAllocator = 0);
        // Create a 'TransformIterator' using the specified 'iterator' and
        // 'functor'.  Optionally specify 'basicAllocator' from which to
        // allocate memory.

    TransformIterator(const TransformIterator&  other,
                      bslma::Allocator         *basicAllocator = 0);
        // Create a copy of the specified 'TransformIterator' object 'other'.
        // Optionally specify 'basicAllocator' from which to allocate memory.

    // PUBLIC MANIPULATORS
    TransformIterator& operator=(const TransformIterator& other);
        // Assign the specified 'other' object to this one and return a
        // reference to this object.

    TransformIterator& operator++();
        // Increment the iterator of this object and return a reference to this
        // object.

    TransformIterator operator++(int);
        // Create a copy of this object, increment the iterator of this object,
        // then return the copy.

    TransformIterator& operator--();
        // Decrement the iterator of this object and return a reference to this
        // object.

    TransformIterator operator--(int);
        // Create a copy of this object, decrement the iterator of this object,
        // then return the copy.

    TransformIterator& operator+=(difference_type n);
        // Add the specified 'n' to the underlying iterator and return a
        // reference to this object.

    TransformIterator& operator-=(difference_type n);
        // Subtract the specified 'n' from the underlying iterator and return a
        // reference to this object.

    typename Traits::reference operator*();
        // Return the result of applying the functor to the dereferenced
        // iterator.

    typename Traits::pointer operator->();
        // Return the address of the result of applying the functor to the
        // dereferenced iterator.  Note that the functor must return a
        // reference type for this method to be used.
        //
        // To be done: For input iterators, have a proxy object that holds the
        // result of the functor call and implements operatr->, and return
        // that.  Alternatively, remove this method altogether as has been done
        // for 'istreambuf_iterator' in latest C++.

    typename Traits::reference operator[](difference_type n);
        // Return the result of applying the functor to the indexed dereference
        // by the specified 'n' of the underlying iterator.

    FUNCTOR& functor();
        // Return a reference to the functor of this object.

    ITERATOR& iterator();
        // Return a reference to the iterator of this object.

    // PUBLIC ACCESSORS
    typename Traits::reference operator*() const;
        // Return the result of applying the functor to the dereferenced
        // iterator.

    typename Traits::pointer operator->() const;
        // Return the address of the result applying the functor to the
        // dereferenced iterator.  Note that the functor must return a
        // reference type for this method to be used.
        //
        // To be done: For input iterators, have a proxy object that holds the
        // result of the functor call and implements operatr->, and return
        // that.  Alternatively, remove this method altogether as has been done
        // for 'istreambuf_iterator' in latest C++.

    typename Traits::reference operator[](difference_type n) const;
        // Return the result of applying the functor to the indexed dereference
        // by the specified 'n' of the underlying iterator.

    TransformIterator operator+(difference_type n) const;
        // Return a copy of this object with its iterator moved forward by the
        // specified 'n'.

    TransformIterator operator-(difference_type n) const;
        // Return a copy of this object with its iterator moved backward by
        // the specified 'n'.

    const FUNCTOR& functor() const;
        // Return a const reference to the functor of this object.

    const ITERATOR& iterator() const;
        // Return a const reference to the iterator of this object.

    // ASPECTS
    void swap(TransformIterator& other);
        // Exchange the functor and iterator of this object with those of the
        // specified 'other' object.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        TransformIterator,
        bslma::UsesBslmaAllocator,
        bslma::UsesBslmaAllocator<ITERATOR>::value ||
        bslma::UsesBslmaAllocator<FUNCTOR> ::value)
};

// FREE FUNCTIONS

template <class FUNCTOR, class ITERATOR>
bool operator==(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                const TransformIterator<FUNCTOR, ITERATOR>& rhs);
    // Return 'true' iff the specified 'lhs' and 'rhs' have equal iterators.
    // Note that the functors are not compared.

template <class FUNCTOR, class ITERATOR>
bool operator!=(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                const TransformIterator<FUNCTOR, ITERATOR>& rhs);
    // Return 'true' iff the specified 'lhs' and 'rhs' have unequal iterators.
    // Note that the functors are not compared.

template <class FUNCTOR, class ITERATOR>
bool operator<(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
               const TransformIterator<FUNCTOR, ITERATOR>& rhs);
    // Return 'true' iff the iterator of the specified 'lhs' is less than the
    // iterator of the specified 'rhs'.  Note that the functors are not
    // compared.

template <class FUNCTOR, class ITERATOR>
bool operator>(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
               const TransformIterator<FUNCTOR, ITERATOR>& rhs);
    // Return 'true' iff the iterator of the specified 'lhs' is greater than
    // the iterator of the specified 'rhs'.  Note that the functors are not
    // compared.

template <class FUNCTOR, class ITERATOR>
bool operator<=(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                const TransformIterator<FUNCTOR, ITERATOR>& rhs);
    // Return 'true' iff the iterator of the specified 'lhs' is less than or
    // equal to the iterator of the specified 'rhs'.  Note that the functors
    // are not compared.

template <class FUNCTOR, class ITERATOR>
bool operator>=(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                const TransformIterator<FUNCTOR, ITERATOR>& rhs);
    // Return 'true' iff the iterator of the specified 'lhs' is greater than or
    // equal to the iterator of the specified 'rhs'.  Note that the functors
    // are not compared.

          // --------------------------------------------------------
          // struct bdlb::TransformIterator_AllocatorOfIteratorMethod
          // --------------------------------------------------------

// PUBLIC ACCESSORS
template <class BASE>
inline
bslma::Allocator *TransformIterator_AllocatorOfIteratorMethod<BASE, true>
::allocator() const
{
    return static_cast<const BASE&>(*this).iterator().allocator();
}

          // -------------------------------------------------------
          // struct bdlb::TransformIterator_AllocatorOfFunctorMethod
          // -------------------------------------------------------

// PUBLIC ACCESSORS
template <class BASE>
inline
bslma::Allocator *TransformIterator_AllocatorOfFunctorMethod<BASE, true>
::allocator() const
{
    return static_cast<const BASE&>(*this).functor().allocator();
}

                          //------------------------
                          // class TransformIterator
                          //------------------------

// CREATORS
template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>::TransformIterator()
: d_iteratorProxy()
, d_functorProxy()
{
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>::TransformIterator(
                                              bslma::Allocator *basicAllocator)
: d_iteratorProxy(basicAllocator)
, d_functorProxy(basicAllocator)
{
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>::TransformIterator(
                                              const ITERATOR&   iterator,
                                              FUNCTOR           functor,
                                              bslma::Allocator *basicAllocator)
: d_iteratorProxy(iterator, basicAllocator)
, d_functorProxy(functor, basicAllocator)
{
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>::TransformIterator(
                                      const TransformIterator&  other,
                                      bslma::Allocator         *basicAllocator)
: d_iteratorProxy(other.iterator(), basicAllocator)
, d_functorProxy(other.functor(), basicAllocator)
{
}

// MANIPULATORS
template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>&
TransformIterator<FUNCTOR, ITERATOR>::operator=(const TransformIterator& other)
{
    iterator() = other.iterator();
    functor()  = other.functor();

    return *this;
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>&
TransformIterator<FUNCTOR, ITERATOR>::operator++()
{
    ++iterator();
    return *this;
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>
TransformIterator<FUNCTOR, ITERATOR>::operator++(int)
{
    return TransformIterator(iterator()++, functor());
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>&
TransformIterator<FUNCTOR, ITERATOR>::operator--()
{
    --iterator();
    return *this;
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>
TransformIterator<FUNCTOR, ITERATOR>::operator--(int)
{
    return TransformIterator(iterator()--, functor());
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>&
TransformIterator<FUNCTOR, ITERATOR>::operator+=(difference_type n)
{
    iterator() += n;
    return *this;
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>&
TransformIterator<FUNCTOR, ITERATOR>::operator-=(difference_type n)
{
    iterator() -= n;
    return *this;
}

template <class FUNCTOR, class ITERATOR>
inline
typename TransformIterator<FUNCTOR, ITERATOR>::Traits::reference
TransformIterator<FUNCTOR, ITERATOR>::operator*()
{
    return functor()(*iterator());
}

template <class FUNCTOR, class ITERATOR>
inline
typename TransformIterator<FUNCTOR, ITERATOR>::Traits::pointer
TransformIterator<FUNCTOR, ITERATOR>::operator->()
{
    return BSLS_UTIL_ADDRESSOF(functor()(*iterator()));
}

template <class FUNCTOR, class ITERATOR>
inline
typename TransformIterator<FUNCTOR, ITERATOR>::Traits::reference
TransformIterator<FUNCTOR, ITERATOR>::operator[](difference_type n)
{
    return functor()(iterator()[n]);
}

template <class FUNCTOR, class ITERATOR>
inline
FUNCTOR& TransformIterator<FUNCTOR, ITERATOR>::functor()
{
    return d_functorProxy.object();
}

template <class FUNCTOR, class ITERATOR>
inline
ITERATOR& TransformIterator<FUNCTOR, ITERATOR>::iterator()
{
    return d_iteratorProxy.object();
}

// ACCESSORS
template <class FUNCTOR, class ITERATOR>
inline
typename TransformIterator<FUNCTOR, ITERATOR>::Traits::reference
TransformIterator<FUNCTOR, ITERATOR>::operator*() const
{
    return functor()(*iterator());
}

template <class FUNCTOR, class ITERATOR>
inline
typename TransformIterator<FUNCTOR, ITERATOR>::Traits::pointer
TransformIterator<FUNCTOR, ITERATOR>::operator->() const
{
    return BSLS_UTIL_ADDRESSOF(functor()(*iterator()));
}

template <class FUNCTOR, class ITERATOR>
inline
typename TransformIterator<FUNCTOR, ITERATOR>::Traits::reference
TransformIterator<FUNCTOR, ITERATOR>::operator[](difference_type n) const
{
    return functor()(iterator()[n]);
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>
TransformIterator<FUNCTOR, ITERATOR>::operator+(difference_type n) const
{
    return TransformIterator(iterator() + n, functor());
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>
TransformIterator<FUNCTOR, ITERATOR>::operator-(difference_type n) const
{
    return TransformIterator(iterator() - n, functor());
}

template <class FUNCTOR, class ITERATOR>
inline
const FUNCTOR& TransformIterator<FUNCTOR, ITERATOR>::functor() const
{
    return d_functorProxy.object();
}

template <class FUNCTOR, class ITERATOR>
inline
const ITERATOR& TransformIterator<FUNCTOR, ITERATOR>::iterator() const
{
    return d_iteratorProxy.object();
}

// ASPECTS
template <class FUNCTOR, class ITERATOR>
inline
void TransformIterator<FUNCTOR, ITERATOR>::swap(
                                   TransformIterator<FUNCTOR, ITERATOR>& other)
{
    using bsl::swap;
    swap(functor(), other.functor());
    swap(iterator(), other.iterator());
}

}  // close package namespace

                            // INLINE DEFINITIONS

// FREE FUNCTIONS
template <class FUNCTOR, class ITERATOR>
inline
bool bdlb::operator==(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                      const TransformIterator<FUNCTOR, ITERATOR>& rhs)
{
    return lhs.iterator() == rhs.iterator();
}

template <class FUNCTOR, class ITERATOR>
inline
bool bdlb::operator!=(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                      const TransformIterator<FUNCTOR, ITERATOR>& rhs)
{
    return lhs.iterator() != rhs.iterator();
}

template <class FUNCTOR, class ITERATOR>
inline
bool bdlb::operator<(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                     const TransformIterator<FUNCTOR, ITERATOR>& rhs)
{
    return lhs.iterator() < rhs.iterator();
}

template <class FUNCTOR, class ITERATOR>
inline
bool bdlb::operator>(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                     const TransformIterator<FUNCTOR, ITERATOR>& rhs)
{
    return lhs.iterator() > rhs.iterator();
}

template <class FUNCTOR, class ITERATOR>
inline
bool bdlb::operator<=(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                      const TransformIterator<FUNCTOR, ITERATOR>& rhs)
{
    return lhs.iterator() <= rhs.iterator();
}

template <class FUNCTOR, class ITERATOR>
inline
bool bdlb::operator>=(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                      const TransformIterator<FUNCTOR, ITERATOR>& rhs)
{
    return lhs.iterator() >= rhs.iterator();
}

}  // close enterprise namespace
#endif

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
