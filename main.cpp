/*
 * main.cpp
 *
 * (c) 2013 helloworld922
 *
 *  Created on: Jul 13, 2013
 *      Author: helloworld922
 */

#include "signals3/signal.hpp"
#include <atomic>
#include <iostream>
#include <chrono>
#include <boost/signals2.hpp>

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

int
main(void)
{
    // timing stuff
    std::chrono::high_resolution_clock clock;
    auto start = clock.now();
    auto end = clock.now();
    const size_t num_slots = 11;
    const size_t count = 1ULL << (28 - num_slots);

    std::function< void
    (void) > test_func(&basic_handler);

    boost::signals3::signal< void
    (void) > test_sig;
    boost::signals2::signal< void
    (void) > sig2_test;
    for (size_t i = 0; i < ((1<<num_slots)>>1); ++i)
    {
        test_sig.connect(&basic_handler);
        sig2_test.connect(&basic_handler);
    }

    for (size_t iter = 0; iter < 4; ++iter)
    {
//        start = clock.now();
//        for (size_t i = 0; i < count; ++i)
//        {
//            test_func();
//        }
//        end = clock.now();
//        std::cout << "std::function: "
//                << std::chrono::duration_cast< std::chrono::nanoseconds >(end - start).count()
//                        / (double) (count) << "ns" << std::endl;

        start = clock.now();
        for (size_t i = 0; i < count; ++i)
        {
            test_sig();
        }
        end = clock.now();

        std::cout << "signals3: "
                << std::chrono::duration_cast< std::chrono::nanoseconds >(end - start).count()
                        / (double) (count) << "ns" << std::endl;

        start = clock.now();
        for (size_t i = 0; i < count; ++i)
        {
            sig2_test();
        }
        end = clock.now();

        std::cout << "signals2: "
                << std::chrono::duration_cast< std::chrono::nanoseconds >(end - start).count()
                        / (double) (count) << "ns" << std::endl;

        std::cout << std::endl;
    }
}
