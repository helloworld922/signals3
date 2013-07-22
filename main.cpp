//
// main.cpp
//
// (c) 2013 helloworld922
//
//  Created on: Jul 13, 2013
//      Author: helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//#define STD_FUNC_TEST
#define SIG3_TEST
//#define SIG2_TEST

#ifdef STD_FUNC_TEST
#include <functional>
#endif
#ifdef SIG3_TEST
#include "signals3/signals3.hpp"
#endif
#ifdef SIG2_TEST
#include <boost/signals2.hpp>
#endif

#include <iostream>
#include <chrono>
//#include <boost/signals2.hpp>

int val[2];

template<size_t N>
void
test_handler(void)
{
    std::cout << "test" << N << std::endl;
}

void
basic_handler(void)
{
    for (size_t i = 0; i < 2; ++i)
    {
        val[i] += i;
    }
}

void
extended_handler(const boost::signals3::connection& conn)
{
    std::cout << "extended handler" << std::endl;
}

void
timing_test(const size_t num_slots)
{
    // timing stuff
    std::chrono::high_resolution_clock clock;
    auto start = clock.now();
    auto end = clock.now();
    const size_t count = 1ULL << (27 - num_slots);
    const size_t call_times = ((1 << num_slots) >> 1);

#ifdef STD_FUNC_TEST
    std::function< void
    (void) > test_func(&basic_handler);
#endif
#ifdef SIG3_TEST
    boost::signals3::signal< void
    (void) > test_sig;
    for (size_t i = 0; i < call_times; ++i)
    {
        test_sig.push_back(&basic_handler);
    }
#endif
#ifdef SIG2_TEST
    boost::signals2::signal< void
    (void) > sig2_test;
    for (size_t i = 0; i < call_times; ++i)
    {
        sig2_test.connect(&basic_handler);
    }
#endif

    for (size_t iter = 0; iter < 8; ++iter)
    {
#ifdef STD_FUNC_TEST
        start = clock.now();
        for (size_t i = 0; i < count; ++i)
        {
            for (size_t j = 0; j < call_times; ++j)
            {
                test_func();
            }
        }
        end = clock.now();
        std::cout << "std::function: "
        << std::chrono::duration_cast< std::chrono::nanoseconds >(end - start).count()
        / (double) (count) << "ns" << std::endl;
#endif
#ifdef SIG3_TEST
        start = clock.now();
        for (size_t i = 0; i < count; ++i)
        {
            test_sig();
        }
        end = clock.now();

        std::cout << "signals3: "
        << std::chrono::duration_cast< std::chrono::nanoseconds >(end - start).count()
        / (double) (count) << "ns" << std::endl;
#endif
#ifdef SIG2_TEST
        start = clock.now();
        for (size_t i = 0; i < count; ++i)
        {
            sig2_test();
        }
        end = clock.now();

        std::cout << "signals2: "
        << std::chrono::duration_cast< std::chrono::nanoseconds >(end - start).count()
        / (double) (count) << "ns" << std::endl;
#endif
    }
}

int
main(void)
{
//    for (size_t i = 0; i < 8; ++i)
//    {
//        std::cout << "i = " << i << std::endl;
//        timing_test(i);
//        std::cout << std::endl;
//    }
    boost::signals3::signal< void
    (void) > mysig;
    mysig.erase(&basic_handler);
//    const boost::signals3::connection conn = mysig.push_back_extended(&extended_handler);
//    mysig();
//    conn.disconnect();
//    mysig();
}
