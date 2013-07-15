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
    boost::signals3::signal<void(void)> mysig;
    mysig();
}
