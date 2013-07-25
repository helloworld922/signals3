//
// slots_text.cpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "slots_test.hpp"
#include <iostream>

namespace boost
{
    namespace signals3
    {
        namespace test
        {
            namespace slots
            {
                void
                test_handler(void)
                {
                    std::cout << "test_handler" << std::endl;
                }

                extern void
                compile_test(void)
                {
                    std::cout << "should be slot constructor" << std::endl;
                    boost::signals3::slot< void
                    (void) > slot1(&test_handler);

                    std::cout << "temp slot should be copy slot" << std::endl;
                    boost::signals3::slot< void
                    (void) > temp_slot(slot1);
                    std::cout << "slot2 should be move slot" << std::endl;
                    boost::signals3::slot< void
                    (void) > slot2(boost::move(temp_slot));

                    std::cout << "should be slot_fuction" << std::endl;
                    slot1.slot_function();
                    std::cout << "should be invoke" << std::endl;
                    slot1();

                    std::cout << "check expired" << std::endl;
                    if (slot1.expired())
                    {
                        std::cout << "expired" << std::endl;
                    }

                    boost::signals3::detail::forward_list<
                            boost::signals3::detail::shared_ptr< void > > locks;
                    std::cout << "try_lock" << std::endl;
                    boost::signals3::detail::shared_ptr< int > lock =
                            boost::signals3::detail::make_shared< int >(5);
                    slot1.try_lock(locks);
                    slot1.tracking();
                    std::cout << "track" << std::endl;
                    slot1.track(lock);
                    boost::signals3::detail::weak_ptr<void> my_weak_ptr = lock;
                    slot2.track(my_weak_ptr);
                }
            }
        }
    }
}
