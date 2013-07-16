/*
 * main.cpp
 *
 * (c) 2013 helloworld922
 *
 *  Created on: Jul 13, 2013
 *      Author: helloworld922
 */

#include "signals3/signal.hpp"
#include <iostream>
#include <chrono>
#include <boost/signals2.hpp>

int val[2];

template<size_t N>
void test_handler(void)
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

    boost::signals3::signal<void(void)> test_sig;
    test_sig.connect(&test_handler<0>);
    test_sig.connect(&test_handler<1>);
    test_sig.connect(&test_handler<2>);
    test_sig();

    boost::signals3::signal<void
    (void)> mysig;

    boost::signals2::signal<void
    (void)> sig2;
    mysig.connect(&basic_handler);
    sig2.connect(&basic_handler);
    const size_t count = 1ULL << 18;

    std::cout << std::endl;

    for (size_t iter = 0; iter < 10; ++iter)
    {
        auto begin = clock.now();
        for (size_t i = 0; i < count; ++i)
        {
            mysig();
        }
        auto end = clock.now();
        std::cout << "signals3: " << std::chrono::duration_cast < std::chrono::nanoseconds
                > (end - begin).count() / (double) count << std::endl;

        begin = clock.now();
        for (size_t i = 0; i < count; ++i)
        {
            sig2();
        }
        end = clock.now();
        std::cout << "signals2: " << std::chrono::duration_cast < std::chrono::nanoseconds
                > (end - begin).count() / (double) count << std::endl;
    }
}
