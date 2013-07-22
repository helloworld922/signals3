//
// slots.hpp
//
// (c) 2013 helloworld922
//
//  Created on: Jul 14, 2013
//      Author: helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_SLOTS_HPP
#define BOOST_SIGNALS3_SLOTS_HPP

namespace boost
{
    namespace signals3
    {
        template<typename Signature>
            class slot_base;

        template<typename ResultType, typename ... Args>
            class slot_base< ResultType
            (Args...) >
            {
            public:
                typedef ResultType result_type;

                virtual
                ~slot_base(void)
                {
                }
            };

        template<typename Signature, typename SlotFunction>
            class slot;

        template<typename ResultType, typename ... Args, typename SlotFunction>
            class slot< ResultType
            (Args...), SlotFunction > : public slot_base< ResultType
            (Args...) >
            {
                SlotFunction callback;
            public:
                typedef SlotFunction function_type;

                slot(const SlotFunction& callback) :
                        callback(callback)
                {
                }

                slot(SlotFunction&& callback) :
                        callback(std::move(callback))
                {
                }

                virtual
                ~slot(void)
                {
                }

                SlotFunction&
                slot_function(void)
                {
                    return callback;
                }

                const SlotFunction&
                slot_function(void) const
                {
                    return callback;
                }
            };
    }
}

#endif // BOOST_SIGNALS3_SLOTS_HPP
