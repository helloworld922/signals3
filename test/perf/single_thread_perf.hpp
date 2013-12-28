//
// single_thread_perf.hpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef SINGLE_THREAD_PERF_HPP_
#define SINGLE_THREAD_PERF_HPP_

#define SIGNALS3_ST_UNSAFE_PERF_TESTING
//#define SIGNALS3_ST_SAFE_PERF_TESTING
//#define SIGNALS2_ST_PERF_TESTING

#if defined(SIGNALS3_ST_SAFE_PERF_TESTING) || defined(SIGNALS3_ST_UNSAFE_PERF_TESTING)
#include <boost/signals3/signals3.hpp>
#endif
#if defined(SIGNALS2_ST_PERF_TESTING)
#include <boost/signals2.hpp>
#endif

namespace boost
{
    namespace signals3
    {
        namespace perf
        {
#if defined(SIGNALS3_ST_SAFE_PERF_TESTING)
            extern void
            signal3_safe_perf(uint64_t num_slots, uint64_t call_times, uint64_t samples);
#endif
#if defined(SIGNALS3_ST_UNSAFE_PERF_TESTING)
            extern void
            signal3_unsafe_perf(uint64_t num_slots, uint64_t call_times, uint64_t samples);
#endif
#if defined(SIGNALS2_ST_PERF_TESTING)
            extern void
            signal2_perf(const size_t num_slots, const size_t call_times, const size_t samples);
#endif
        }
    }
}

#endif // SINGLE_THREAD_PERF_HPP_
