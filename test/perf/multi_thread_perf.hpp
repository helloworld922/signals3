//
// multi_thread_perf.hpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef MULTI_THREAD_PERF_HPP_
#define MULTI_THREAD_PERF_HPP_

#define SIGNALS3_MT_PERF_TESTING
//#define SIGNALS2_MT_PERF_TESTING

#if defined(SIGNALS3_MT_PERF_TESTING)
#include <boost/signals3/signals3.hpp>
#endif

#if (defined SIGNALS2_MT_PERF_TESTING)
#include <boost/signals2.hpp>
#endif

namespace boost
{
    namespace signals3
    {
        namespace perf
        {
#if defined(SIGNALS3_MT_PERF_TESTING)
            extern void
            signal3_perf(const size_t num_slots, const size_t call_times, const size_t samples,
                    const size_t num_thread);
#endif

#if (defined SIGNALS2_MT_PERF_TESTING)
        extern void
        signal2_perf(const size_t num_slots, const size_t call_times, const size_t samples,
                const size_t num_thread);
#endif
    }
}
}

#endif // MULTI_THREAD_PERF_HPP_
