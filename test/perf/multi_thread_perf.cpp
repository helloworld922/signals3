//
// multi_thread_perf.cpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "multi_thread_perf.hpp"

#include <functional>
#include <boost/thread/thread.hpp>
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
            inline void
            perf_handler(void)
            {
            }

#if defined(SIGNALS3_MT_PERF_TESTING)
            struct signal3_perf_worker
            {
                const size_t call_times, samples;
                boost::signals3::signal< void
                (void) >& sig;
                std::vector< std::chrono::high_resolution_clock::duration > times;

                signal3_perf_worker(const size_t call_times, const size_t samples,
                        boost::signals3::signal< void
                        (void) >& sig) :
                        call_times(call_times), samples(samples), sig(sig)
                {
                    times.reserve(samples);
                }

                void
                operator()(void)
                {
                    std::chrono::high_resolution_clock clock;

                    for (size_t iter = 0; iter < samples; ++iter)
                    {
                        std::chrono::high_resolution_clock::time_point start = clock.now();
                        for (size_t i = 0; i < call_times; ++i)
                        {
                            sig.emit();
                        }
                        std::chrono::high_resolution_clock::time_point end = clock.now();
                        times.push_back(end - start);
                    }
                }
            };

            void
            signal3_perf(const size_t num_slots, const size_t call_times, const size_t samples,
                    const size_t num_threads)
            {
                boost::signals3::signal< void
                (void) > *perf_sig = new boost::signals3::signal< void
                (void) >();
                for (size_t i = 0; i < num_slots; ++i)
                {
                    perf_sig->push_back_unsafe(&perf_handler);
                }
                // create thread group and workers
                signal3_perf_worker** workers = new signal3_perf_worker*[num_threads];
                boost::thread_group my_group;
                std::cout << "num threads: " << num_threads << std::endl;
                std::cout << "num slots: " << num_slots << std::endl;
                for (size_t i = 0; i < num_threads; ++i)
                {
                    workers[i] = new signal3_perf_worker(call_times, samples, *perf_sig);
                    my_group.create_thread(boost::ref(*(workers[i])));
                }
                my_group.join_all();
                // collect results
                std::vector< std::chrono::high_resolution_clock::duration > safe_times;
                std::vector< std::chrono::high_resolution_clock::duration > unsafe_times;
                for (size_t i = 0; i < num_threads; ++i)
                {
                    safe_times.insert(safe_times.end(), workers[i]->times.begin(),
                            workers[i]->times.end());
                    delete workers[i];
                }
                std::sort(safe_times.begin(), safe_times.end());
//                std::sort(unsafe_times.begin(), unsafe_times.end());
                std::cout << "safe times: " << std::endl;
                std::cout << "\tmin: "
                        << std::chrono::duration_cast< std::chrono::nanoseconds >(safe_times[0]).count()
                                / (double) call_times << " ns" << std::endl;
                std::cout << "\tmedian: "
                        << std::chrono::duration_cast< std::chrono::nanoseconds >(
                                safe_times[safe_times.size() / 2]).count() / (double) call_times
                        << " ns" << std::endl;
                std::cout << "\tmax: "
                        << std::chrono::duration_cast< std::chrono::nanoseconds >(
                                safe_times[safe_times.size() - 1]).count() / (double) call_times
                        << " ns" << std::endl;
                // unsafe times
//                std::cout << "unsafe times: " << std::endl;
//                std::cout << "\tmin: "
//                        << std::chrono::duration_cast< std::chrono::nanoseconds >(unsafe_times[0]).count()
//                                / (double) call_times << " ns" << std::endl;
//                std::cout << "\tmedian: "
//                        << std::chrono::duration_cast< std::chrono::nanoseconds >(
//                                unsafe_times[unsafe_times.size() / 2]).count() / (double) call_times
//                        << " ns" << std::endl;
//                std::cout << "\tmax: "
//                        << std::chrono::duration_cast< std::chrono::nanoseconds >(
//                                unsafe_times[unsafe_times.size() - 1]).count() / (double) call_times
//                        << " ns" << std::endl;
                delete perf_sig;
                delete[] workers;
            }
#endif

#if defined(SIGNALS2_MT_PERF_TESTING)
            struct signal2_perf_worker
            {
                const size_t call_times, samples;
                boost::signals2::signal< void
                (void) >& sig;
                std::vector< std::chrono::high_resolution_clock::duration > times;

                signal2_perf_worker(const size_t call_times, const size_t samples,
                        boost::signals2::signal< void
                        (void) >& sig) :
                        call_times(call_times), samples(samples), sig(sig)
                {
                    times.reserve(samples);
                }

                void
                operator()(void)
                {
                    std::chrono::high_resolution_clock clock;
                    for (size_t iter = 0; iter < samples; ++iter)
                    {
                        std::chrono::high_resolution_clock::time_point start = clock.now();
                        for (size_t i = 0; i < call_times; ++i)
                        {
                            sig();
                        }
                        std::chrono::high_resolution_clock::time_point end = clock.now();
                        times.push_back(end - start);
                    }
                }
            };

            void
            signal2_perf(const size_t num_slots, const size_t call_times, const size_t samples,
                    const size_t num_threads)
            {
                boost::signals2::signal< void
                (void) > *perf_sig = new boost::signals2::signal< void
                (void) >();
                for (size_t i = 0; i < num_slots; ++i)
                {
                    perf_sig->connect(&perf_handler);
                }
                // create thread group and workers
                signal2_perf_worker** workers = new signal2_perf_worker*[num_threads];
                boost::thread_group my_group;
                std::cout << "num threads: " << num_threads << std::endl;
                std::cout << "num slots: " << num_slots << std::endl;
                for (size_t i = 0; i < num_threads; ++i)
                {
                    workers[i] = new signal2_perf_worker(call_times, samples, *perf_sig);
                    my_group.create_thread(boost::ref(*(workers[i])));
                }
                my_group.join_all();
                // collect results
                std::vector< std::chrono::high_resolution_clock::duration > times;
                for (size_t i = 0; i < num_threads; ++i)
                {
                    times.insert(times.end(), workers[i]->times.begin(),
                            workers[i]->times.end());

                    delete workers[i];
                }
                std::sort(times.begin(), times.end());
                std::cout << "signals2 times: " << std::endl;
                std::cout << "\tmin: "
                        << std::chrono::duration_cast< std::chrono::nanoseconds >(times[0]).count()
                                / (double) call_times << " ns" << std::endl;
                std::cout << "\tmedian: "
                        << std::chrono::duration_cast< std::chrono::nanoseconds >(
                                times[times.size() / 2]).count() / (double) call_times
                        << " ns" << std::endl;
                std::cout << "\tmax: "
                        << std::chrono::duration_cast< std::chrono::nanoseconds >(
                                times[times.size() - 1]).count() / (double) call_times
                        << " ns" << std::endl;
                delete perf_sig;
                delete[] workers;
            }
#endif
        }
    }
}
