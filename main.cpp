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
#include "test/perf/single_thread_perf.hpp"
//#include "test/perf/multi_thread_perf.hpp"
#include "test/forward_test.hpp"
#include <iostream>

int
main(void)
{
//  boost::signals3::test::forward::test();
//    boost::signals3::test::connection::compile_test();
//    boost::signals3::test::slots::compile_test();
  constexpr uint64_t WORK_CONST = 16;
  constexpr uint64_t NUM_SAMPLES = 1024ULL;

  for (uint64_t i = 0; i < 6; ++i)
    {
#if defined(SIGNALS3_ST_SAFE_PERF_TESTING)
      boost::signals3::perf::signal3_safe_perf((1ULL << i) >> 1, 1ULL << (WORK_CONST - i), NUM_SAMPLES);
      std::cout << std::endl;
#endif
#if defined(SIGNALS3_ST_UNSAFE_PERF_TESTING)
      boost::signals3::perf::signal3_unsafe_perf((1ULL << i) >> 1, 1ULL << (WORK_CONST - i), NUM_SAMPLES);
      std::cout << std::endl;
#endif
#if defined(SIGNALS2_ST_PERF_TESTING)
      boost::signals3::perf::signal2_perf((1ULL << i) >> 1, 1ULL << (WORK_CONST - 1 - i), NUM_SAMPLES);
      std::cout << std::endl;
#endif
      for (uint64_t num_threads = 1; num_threads <= 8; num_threads *= 2)
        {
#if defined(SIGNALS3_MT_PERF_TESTING)
          boost::signals3::perf::signal3_perf((1ULL << i) >> 1, 1ULL << (WORK_CONST - i - num_threads / 4), NUM_SAMPLES,
                                              num_threads);
          std::cout << "\t";
//			std::cout << std::endl;
#endif
#if defined(SIGNALS2_MT_PERF_TESTING)
          boost::signals3::perf::signal2_perf((1ULL << i) >> 1, 1ULL << (WORK_CONST - i - num_threads / 4), NUM_SAMPLES,
                                              num_threads);
          std::cout << std::endl;
#endif

        }
//      std::cout << std::endl;
    }
//  std::cout << "done" << std::endl;
  std::cin.ignore();
}
