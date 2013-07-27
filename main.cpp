//
// main.cpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//#include <boost/signals3/signals3.hpp>
//#include "test/connection_test.hpp"
//#include "test/slots_test.hpp"
//#include "test/perf/single_thread_perf.hpp"
#include "test/perf/multi_thread_perf.hpp"
#include <iostream>

int
main(void)
{
//    boost::signals3::test::connection::compile_test();
//    boost::signals3::test::slots::compile_test();

    for (size_t i = 0; i < 7; ++i)
    {
        for (size_t num_threads = 1; num_threads <= 8; num_threads *= 2)
        {
#if defined(SIGNALS3_ST_PERF_TESTING)
            boost::signals3::perf::signal3_perf((1ULL << i) >> 1, 1ULL << (26 - i), 16ULL);
            std::cout << std::endl;
#endif
#if defined(SIGNALS2_ST_PERF_TESTING)
            boost::signals3::perf::signal2_perf((1ULL << i) >> 1, 1ULL << (25 - i), 16ULL);
            std::cout << std::endl;
#endif
#if defined(SIGNALS3_MT_PERF_TESTING)
            boost::signals3::perf::signal3_perf((1ULL << i) >> 1, 1ULL << (23 - i - num_threads / 4), 64ULL,
                    num_threads);
            std::cout << std::endl;
#endif
#if defined(SIGNALS2_MT_PERF_TESTING)
            boost::signals3::perf::signal2_perf((1ULL << i) >> 1, 1ULL << (22 - i - num_threads / 4), 64ULL,
                    num_threads);
            std::cout << std::endl;
#endif

        }
    }
}
