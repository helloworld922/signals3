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

#define USE_TIMINGS

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

//#include "signals3/signals3.hpp"
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
param_handler(int n)
{
    std::cout << n << std::endl;
}

#ifdef USE_TIMINGS

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
        test_sig.push_back_unsafe(&basic_handler);
    }
#endif
#ifdef SIG2_TEST
    typename boost::signals2::signal_type<void(void), boost::signals2::keywords::mutex_type<boost::mutex> >::type sig2_test;
//    boost::signals2::signal< void
//    (void) > sig2_test;
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
            test_sig.emit();
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

#endif

struct my_class
{
    int val;
    my_class(void)
    {
        val = 5;
        std::cout << "constructor" << std::endl;
    }
    my_class(const my_class& rhs) :
            val(rhs.val)
    {
        std::cout << "copy constructor" << std::endl;
    }

    my_class(my_class&& rhs) :
            val(rhs.val)
    {
        rhs.val = 0;
        std::cout << "move constructor" << std::endl;
    }

    ~my_class(void)
    {
        val = 0;
        std::cout << "destructor" << std::endl;
    }
};

void
test_forwarding(my_class val)
{
    std::cout << val.val << std::endl;
    val.val = 3;
}

void
test_conn(const boost::signals3::connection& conn)
{
    std::cout << "test conn" << std::endl;
}

int
main(void)
{
    boost::signals3::signal< void
    (void) > mysig;
    boost::signals3::slot< void
    (void) > my_slot(&test_handler< 0 >);
    boost::signals3::detail::shared_ptr< int > var = boost::signals3::detail::make_shared< int >(5);
    my_slot.track(var);
    mysig.push_back_unsafe(boost::move(my_slot));
    std::cout << "still valid" << std::endl;
    mysig.emit_unsafe();

    var.reset();
    std::cout << "invalid" << std::endl;
    mysig.emit_unsafe();

//    for (size_t i = 0; i < 8; ++i)
//    {
//        std::cout << "i = " << i << std::endl;
//        timing_test(i);
//        std::cout << std::endl;
//    }
}
