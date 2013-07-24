//
// connection_test.cpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "connection_test.hpp"
#include <iostream>

namespace boost
{
    namespace signals3
    {
        namespace test
        {
            /**
             * Does a basic compile test by including/invoking all ops
             */
            void
            compile_test(void)
            {
                std::cout << "build conn1" << std::endl;
                boost::signals3::connection conn1;
                std::cout << "copy conn1" << std::endl;
                boost::signals3::connection temp_conn(conn1);
                std::cout << "move build conn2" << std::endl;
                boost::signals3::connection conn2(boost::move(temp_conn));
                std::cout << "block" << std::endl;
                if (conn2.block())
                {
                    std::cout << "error: block successful" << std::endl;
                }
                std::cout << "unblock" << std::endl;
                if (conn2.unblock())
                {
                    std::cout << "error: unblock successful" << std::endl;
                }
                std::cout << "connected" << std::endl;
                if (conn2.connected())
                {
                    std::cout << "error: conn2 connected" << std::endl;
                }
                std::cout << "usable" << std::endl;
                if (conn2.usable())
                {
                    std::cout << "error: conn2 usable" << std::endl;
                }
                std::cout << "disconnect" << std::endl;
                conn2.disconnect();
                if (conn2.connected())
                {
                    std::cout << "error: conn2 connected" << std::endl;
                }
                std::cout << "disconnect_unsafe" << std::endl;
                conn1.disconnect_unsafe();
                if (conn1.connected())
                {
                    std::cout << "error: conn1 connected" << std::endl;
                }
            }
        }
    }
}
