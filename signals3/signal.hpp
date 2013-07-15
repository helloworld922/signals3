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
#include <boost/function.hpp>
#include <functional>
#include "optional_last_value.hpp"
#include "connection.hpp"
#include "detail/extended_signature.hpp"
#include "detail/slots.hpp"
#include <memory>

namespace boost
{
    namespace signals3
    {
        class connection;
        namespace detail
        {
            template<typename Signature>
                struct extended_signature;

            template<typename Signature, typename SlotFunction, typename ExtendedSlotFunction, typename AtomicInt>
                class slot;
        }

        template<typename Signature,
            typename Combiner = optional_last_value< typename std::function<Signature>::result_type >,
            typename Group = int,
            typename GroupCompare = std::less<Group>,
            typename SlotFunction = std::function<Signature>,
            typename ExtendedSlotFunction = typename boost::signals3::detail::extended_signature<Signature>::type,
            typename SharedLock = boost::shared_mutex >
            class signal;

        template<typename ResultType, typename ... Args, typename Combiner, typename Group,
                typename GroupCompare, typename SlotFunction, typename ExtendedSlotFunction,
                typename SharedLock>
            class signal<ResultType(Args...), Combiner, Group, GroupCompare, SlotFunction, ExtendedSlotFunction, SharedLock>
            {
                SharedLock _lock;
                std::shared_ptr< boost::signals3::detail::slot<ResultType(Args...), SlotFunction, ExtendedSlotFunction> > head;
                std::shared_ptr< boost::signals3::detail::slot<ResultType(Args...), SlotFunction, ExtendedSlotFunction> > tail;
            public:
                typedef typename Combiner::result_type result_type;
                typedef Combiner combiner_type;
                typedef Group group_type;
                typedef GroupCompare group_compare_type;
                typedef SlotFunction slot_type;
                typedef ExtendedSlotFunction extended_slot_type;
                typedef SharedLock shared_lock_type;

                typename Combiner::result_type
                operator()(Args ... args)
                {

                }
            };
    }
}

#endif /* SIGNAL_HPP_ */
