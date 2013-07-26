//
// compiler_support.hpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_COMPILER_SUPPORT_HPP
#define BOOST_SIGNALS3_COMPILER_SUPPORT_HPP

#include <boost/config.hpp>

#include <boost/move/move.hpp>

// testing defines
//#define BOOST_NO_CXX11_ATOMIC
//#define BOOST_NO_CXX11_HDR_FUNCTIONAL
//#define BOOST_NO_CXX11_HDR_TUPLE

#if defined(BOOST_NO_CXX11_ATOMIC_SMART_PTR) || defined(BOOST_NO_CXX11_SMART_PTR)
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/atomic.hpp>
#else
#include <memory>
#include <atomic>
#endif

#if defined(BOOST_NO_CXX11_ATOMIC)
#include <boost/atomic.hpp>
#else
#include <atomic>
#endif

#if defined(BOOST_NO_CXX11_HDR_MUTEX)
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#else
#include <mutex>
#endif

#if defined(BOOST_NO_CXX11_HDR_FUNCTIONAL)
#include <boost/function.hpp>
#else
#include <functional>
#endif

#include <numeric>

#if defined(BOOST_NO_CXX11_HDR_FORWARD_LIST)
// TODO: need an alternative for std::forward_list
#else
#include <forward_list>
#endif

#if defined(BOOST_NO_CXX11_HDR_TUPLE)
#include <boost/tuple/tuple.hpp>
#else
#include <tuple>
#endif

namespace boost
{
    namespace signals3
    {
        namespace detail
        {
            // shared_ptr and atomic shared_ptr
#if defined(BOOST_NO_CXX11_ATOMIC_SMART_PTR) || defined(BOOST_NO_CXX11_SMART_PTR)
            using ::boost::shared_ptr;
            using ::boost::weak_ptr;
            using ::boost::static_pointer_cast;
            using ::boost::make_shared;
            using ::boost::atomic_compare_exchange;
            using ::boost::atomic_load;
            using ::boost::atomic_store;

#else
            using ::std::shared_ptr;
            using ::std::weak_ptr;
            using ::std::static_pointer_cast;
            using ::std::make_shared;
            using ::std::atomic_compare_exchange;
            using ::std::atomic_load;
            using ::std::atomic_store;

#endif

            // atomic int
#if defined(BOOST_NO_CXX11_ATOMIC)
            using ::boost::atomic;
#else
            using ::std::atomic;
#endif

            // thread mutex and unique locking
#if defined(BOOST_NO_CXX11_HDR_MUTEX)

            using ::boost::mutex;
            using ::boost::lock_guard;
#else
            using ::std::mutex;
            using ::std::lock_guard;
#endif

            // tuple
#if defined(BOOST_NO_CXX11_HDR_TUPLE)
            using ::boost::tuple;
            using ::boost::get;
#else
            using ::std::tuple;
            using ::std::get;
#endif

            // function wrapper
#if defined(BOOST_NO_CXX11_HDR_FUNCTIONAL)
            using ::boost::function;
#else
            using ::std::function;
#endif

            // forward list
#if defined(BOOST_NO_CXX11_HDR_FORWARD_LIST)
// TODO: need an alternative for std::forward_list
#else
            using ::std::forward_list;
#endif
        }
    }
}

#endif // BOOST_SIGNALS3_COMPILER_SUPPORT_HPP
