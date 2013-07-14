/*
 * signal.hpp
 *
 * (c) 2013 helloworld922
 *
 *  Created on: Jul 13, 2013
 *      Author: helloworld922
 */

#ifndef BOOST_SIGNALS3_SIGNAL_HPP
#define BOOST_SIGNALS3_SIGNAL_HPP

#include <boost/thread.hpp>
#include <functional>
#include "optional_last_value.hpp"
#include "connection.hpp"
#include "extended_signature.hpp"
#include "slots.hpp"

namespace boost
{
    namespace signals3
    {
        class connection;
        namespace detail
        {
            template<typename Signature>
                struct extended_signature;

            template<typename Signature>
                class slot;
        }

        template<typename Signature, typename Combiner = optional_last_value<
                typename std::function<Signature>::result_type>, typename Group = int,
                typename GroupCompare = std::less<Group>, typename SlotFunction = std::function<
                        Signature>,
                typename ExtendedSlotFunction = typename boost::signals3::detail::extended_signature<
                        Signature>::type>
            class signal;

        template<typename ResultType, typename ... Args, typename Combiner, typename Group,
                typename GroupCompare, typename SlotFunction, typename ExtendedSlotFunction>
            class signal<ResultType
            (Args...), Combiner, Group, GroupCompare, SlotFunction, ExtendedSlotFunction>
            {
            public:
                typedef typename Combiner::result_type result_type;
                typedef Combiner combiner_type;
                typedef Group group_type;
                typedef GroupCompare group_compare_type;
                typedef SlotFunction slot_type;
                typedef ExtendedSlotFunction extended_slot_type;

                typename Combiner::result_type
                operator()(Args ... args)
                {

                }
            };
    }
}

#endif /* SIGNAL_HPP_ */
