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

static int var[2];

void
basic_handler(void)
{
    for (size_t i = 0; i < 2; ++i)
    {
        var[i] = i;
    }
}

int
main(void)
{
    constexpr size_t count = 1ULL << 24;

    typedef std::function<void
    (void)> function1_type;
    typedef std::function<void
    (const boost::signals3::connection&)> function2_type;

    function1_type basic_slot(&basic_handler);
    boost::signals3::detail::slot<void
    (void), function1_type, function2_type> my_slot(basic_slot);

    std::chrono::high_resolution_clock clock;

    for (size_t iter = 0; iter < 10; ++iter)
    {
        auto start = clock.now();
        for (size_t i = 0; i < count; ++i)
        {
            basic_handler();
        }
        auto end = clock.now();
        std::cout << "raw call: " << std::chrono::duration_cast < std::chrono::nanoseconds
                > (end - start).count() / (double) count << "ns" << std::endl;

        start = clock.now();
        for (size_t i = 0; i < count; ++i)
        {
            basic_slot();
        }
        end = clock.now();
        std::cout << "std::function: " << std::chrono::duration_cast < std::chrono::nanoseconds
                > (end - start).count() / (double) count << "ns" << std::endl;

        start = clock.now();
        for (size_t i = 0; i < count; ++i)
        {
            my_slot();
        }
        end = clock.now();
        std::cout << "slot: " << std::chrono::duration_cast < std::chrono::nanoseconds
                > (end - start).count() / (double) count << "ns" << std::endl;
    }

}
