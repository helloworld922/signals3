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
            static int val[2];

            inline void
            perf_handler(void)
            {
                for (size_t i = 0; i < 2; ++i)
                {
                    ++val[i];
                }
            }

#if defined(SIGNALS3_ST_PERF_TESTING)
            void
            signal3_perf(const size_t num_slots, const size_t call_times, const size_t samples)
            {
                // timing stuff

                std::chrono::high_resolution_clock clock;

                std::vector< std::chrono::high_resolution_clock::duration > safe_times;
                std::vector< std::chrono::high_resolution_clock::duration > unsafe_times;

                boost::signals3::signal< void
                (void) > perf_sig;
                for (size_t i = 0; i < num_slots; ++i)
                {
                    perf_sig.push_back(&perf_handler);
                }
                std::cout << "signals3 thread-unsafe: " << num_slots << " slots" << std::endl;
                for (size_t iter = 0; iter < samples; ++iter)
                {
                    std::chrono::high_resolution_clock::time_point start = clock.now();
                    for (size_t i = 0; i < call_times; ++i)
                    {
                        perf_sig.emit_unsafe();
                    }
                    std::chrono::high_resolution_clock::time_point end = clock.now();
                    unsafe_times.push_back(end - start);
                }

                std::sort(unsafe_times.begin(), unsafe_times.end());
                std::cout << "min: "
                << std::chrono::duration_cast< std::chrono::nanoseconds >(unsafe_times[0]).count()
                / (double) call_times << " ns" << std::endl;
                std::cout << "median: "
                << std::chrono::duration_cast< std::chrono::nanoseconds >(
                        unsafe_times[unsafe_times.size() / 2]).count() / (double) call_times
                << " ns" << std::endl;
                std::cout << "max: "
                << std::chrono::duration_cast< std::chrono::nanoseconds >(
                        unsafe_times[unsafe_times.size() - 1]).count() / (double) call_times
                << " ns" << std::endl;

                std::cout << std::endl << "signals3 thread-safe: " << num_slots << " slots"
                << std::endl;
                for (size_t iter = 0; iter < samples; ++iter)
                {
                    std::chrono::high_resolution_clock::time_point start = clock.now();
                    for (size_t i = 0; i < call_times; ++i)
                    {
                        perf_sig.emit();
                    }
                    std::chrono::high_resolution_clock::time_point end = clock.now();
                    safe_times.push_back(end - start);
                }

                std::sort(safe_times.begin(), safe_times.end());
                std::cout << "min: "
                << std::chrono::duration_cast< std::chrono::nanoseconds >(safe_times[0]).count()
                / (double) call_times << " ns" << std::endl;
                std::cout << "median: "
                << std::chrono::duration_cast< std::chrono::nanoseconds >(
                        safe_times[safe_times.size() / 2]).count() / (double) call_times
                << " ns" << std::endl;
                std::cout << "max: "
                << std::chrono::duration_cast< std::chrono::nanoseconds >(
                        safe_times[safe_times.size() - 1]).count() / (double) call_times
                << " ns" << std::endl;
            }
#endif

#if defined(SIGNALS2_ST_PERF_TESTING)
            void
            signal2_perf(const size_t num_slots, const size_t call_times, const size_t samples)
            {
                // timing stuff
                std::chrono::high_resolution_clock clock;
                std::vector< std::chrono::high_resolution_clock::duration > times;
                boost::signals2::signal< void
                (void) > perf_sig;
                for (size_t i = 0; i < num_slots; ++i)
                {
                    perf_sig.connect(&perf_handler);
                }
                std::cout << "signals2: " << num_slots << " slots" << std::endl;
                for (size_t iter = 0; iter < samples; ++iter)
                {
                    std::chrono::high_resolution_clock::time_point start = clock.now();
                    for (size_t i = 0; i < call_times; ++i)
                    {
                        perf_sig();
                    }
                    std::chrono::high_resolution_clock::time_point end = clock.now();
                    times.push_back(end - start);
                }
                std::sort(times.begin(), times.end());
                std::cout << "min: "
                        << std::chrono::duration_cast< std::chrono::nanoseconds >(times[0]).count()
                                / (double) call_times << " ns" << std::endl;
                std::cout << "median: "
                        << std::chrono::duration_cast< std::chrono::nanoseconds >(
                                times[times.size() / 2]).count() / (double) call_times << " ns"
                        << std::endl;
                std::cout << "max: "
                        << std::chrono::duration_cast< std::chrono::nanoseconds >(
                                times[times.size() - 1]).count() / (double) call_times << " ns"
                        << std::endl;
            }
#endif
        }
    }
}
