/*
 * compiler_support.hpp
 *
 * (c) 2013 helloworld922
 *
 *  Created on: Jul 22, 2013
 *      Author: helloworld922
 */

#ifndef BOOST_SIGNALS3_COMPILER_SUPPORT_HPP
#define BOOST_SIGNALS3_COMPILER_SUPPORT_HPP

// TODO: right now hard-coded for Mingw-w64 gcc 4.8.1

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <numeric>
#include <boost/make_shared.hpp>
#include <atomic>
#include <boost/atomic.hpp>
#include <boost/move/move.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <iterator>
#include <functional>
#include <vector>
#include <forward_list>

#include <tuple>

namespace boost
{
    namespace signals3
    {
        namespace detail
        {
            // shared_ptr and atomic shared_ptr
            using ::boost::shared_ptr;
            using ::boost::weak_ptr;
            using ::boost::make_shared;
            using ::boost::atomic_compare_exchange;
            using ::boost::atomic_load;
            using ::boost::atomic_store;

            // thread mutex and unique locking
            using ::boost::mutex;
            using ::boost::unique_lock;

            // tuple
            using ::std::tuple;

            // atomic int
            using ::std::atomic;

            // function wrapper
            using ::std::function;

            // forward list
            using ::std::forward_list;
        }
    }
}

#endif // BOOST_SIGNALS3_COMPILER_SUPPORT_HPP
