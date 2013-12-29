//
// single_thread_perf.cpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "single_thread_perf.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

namespace boost
{
  namespace signals3
  {
    namespace perf
    {
//      int val[2];
      inline void
      perf_handler()
      {
//        for (size_t i = 0; i < 2; ++i)
//          {
//            val[i] += i;
//          }
      }

#define FUNC_SIG void(void)

#if defined(SIGNALS3_ST_SAFE_PERF_TESTING)
      void
      signal3_safe_perf(uint64_t num_slots, uint64_t call_times, uint64_t samples)
      {
        // timing stuff

        std::chrono::high_resolution_clock clock;

        std::vector< std::chrono::high_resolution_clock::duration > times;
        times.reserve(samples);

        boost::signals3::signal< FUNC_SIG > perf_sig;
        for (size_t i = 0; i < num_slots; ++i)
          {
            perf_sig.push_back_unsafe(&perf_handler);
          }

//        std::cout << "signals3 thread-safe: " << num_slots << " slots"
//                  << std::endl;
        for (size_t iter = 0; iter < samples; ++iter)
          {
            std::chrono::high_resolution_clock::time_point start = clock.now();
            for (size_t i = 0; i < call_times; ++i)
              {
                perf_sig.emit();
              }
            std::chrono::high_resolution_clock::time_point end = clock.now();
            times.push_back(end - start);
          }

        std::sort(times.begin(), times.end());
        std::cout << std::chrono::duration_cast< std::chrono::nanoseconds >(times[0]).count()
                  / (double) call_times << std::endl;
        std::cout
            << std::chrono::duration_cast< std::chrono::nanoseconds >(
              times[times.size() / 2]).count() / (double) call_times
            << std::endl;
        std::cout
            << std::chrono::duration_cast< std::chrono::nanoseconds >(
              times[times.size() - 1]).count() / (double) call_times
            << std::endl;
      }
#endif
#if defined(SIGNALS3_ST_UNSAFE_PERF_TESTING)
      void
      signal3_unsafe_perf(uint64_t num_slots, uint64_t call_times, uint64_t samples)
      {
        // timing stuff

        std::chrono::high_resolution_clock clock;

        std::vector< std::chrono::high_resolution_clock::duration > times;
        times.reserve(samples);

        boost::signals3::signal< FUNC_SIG > perf_sig;
        for (size_t i = 0; i < num_slots; ++i)
          {
            perf_sig.push_back_unsafe(&perf_handler);
          }

//        std::cout << "signals3 thread-unsafe: " << num_slots << " slots"
//                  << std::endl;
        for (size_t iter = 0; iter < samples; ++iter)
          {
            std::chrono::high_resolution_clock::time_point start = clock.now();
            for (size_t i = 0; i < call_times; ++i)
              {
                perf_sig.emit_unsafe();
              }
            std::chrono::high_resolution_clock::time_point end = clock.now();
            times.push_back(end - start);
          }

        std::sort(times.begin(), times.end());
        std::cout
            << std::chrono::duration_cast< std::chrono::nanoseconds >(times[0]).count()
            / (double) call_times << std::endl;
        std::cout
            << std::chrono::duration_cast< std::chrono::nanoseconds >(
              times[times.size() / 2]).count() / (double) call_times
            << std::endl;
        std::cout
            << std::chrono::duration_cast< std::chrono::nanoseconds >(
              times[times.size() - 1]).count() / (double) call_times
            << std::endl;
      }
#endif

#if defined(SIGNALS2_ST_PERF_TESTING)
      void
      signal2_perf(const size_t num_slots, const size_t call_times, const size_t samples)
      {
        // timing stuff
        std::chrono::high_resolution_clock clock;
        std::vector< std::chrono::high_resolution_clock::duration > times;
        times.reserve(samples);
        boost::signals2::signal< FUNC_SIG > perf_sig;
        for (size_t i = 0; i < num_slots; ++i)
          {
            perf_sig.connect(&perf_handler);
          }
//        std::cout << "signals2: " << num_slots << " slots" << std::endl;
        for (size_t iter = 0; iter < samples; ++iter)
          {
            std::chrono::high_resolution_clock::time_point start = clock.now();
            for (size_t i = 0; i < call_times; ++i)
              {
                perf_sig(i);
              }
            std::chrono::high_resolution_clock::time_point end = clock.now();
            times.push_back(end - start);
          }
        std::sort(times.begin(), times.end());
        std::cout
            << std::chrono::duration_cast< std::chrono::nanoseconds >(times[0]).count()
            / (double) call_times << std::endl;
        std::cout
            << std::chrono::duration_cast< std::chrono::nanoseconds >(
              times[times.size() / 2]).count() / (double) call_times
            << std::endl;
        std::cout
            << std::chrono::duration_cast< std::chrono::nanoseconds >(
              times[times.size() - 1]).count() / (double) call_times
            << std::endl;
      }
#endif
    }
  }
}
