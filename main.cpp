/*
 * main.cpp
 *
 * (c) 2013 helloworld922
 *
 *  Created on: Jul 13, 2013
 *      Author: helloworld922
 */

#include "signals3/signal.hpp"


int main(void)
{
    boost::signals3::signal<void(void)> mysig;
    mysig();
}
