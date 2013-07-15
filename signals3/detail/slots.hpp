/*
 * slots.hpp
 *
 * (c) 2013 helloworld922
 *
 *  Created on: Jul 14, 2013
 *      Author: helloworld922
 */

#ifndef BOOST_SIGNALS3_SLOTS_HPP
#define BOOST_SIGNALS3_SLOTS_HPP

#include "../connection.hpp"
#include "atomic_extensions.hpp"
#include <boost/variant.hpp>
#include <atomic>

namespace boost
{
    namespace signals3
    {
        namespace detail
        {
            template<typename Signature, typename SlotFunction, typename ExtendedSlotFunction,
                    typename AtomicInt = std::atomic<int> >
                class slot;

            template<typename ResultType, typename ... Args, typename SlotFunction,
                    typename ExtendedSlotFunction, typename AtomicInt>
                class slot<ResultType
                (Args...), SlotFunction, ExtendedSlotFunction, AtomicInt>
                {
                    const boost::variant<SlotFunction, ExtendedSlotFunction> callback;
                    mutable AtomicInt _unusable;

                    template<typename T>
                        slot(const T& func);

                    // TODO: what value?
                    const int disconnected = -1;

                public:
                    typedef ResultType result_type;
                    typedef SlotFunction slot_type;
                    typedef ExtendedSlotFunction extended_slot_type;
                    typedef AtomicInt atomic_int_type;

                    slot(const SlotFunction& func) :
                            callback(func), _unusable(0)
                    {
                    }

                    slot(const ExtendedSlotFunction& func) :
                            callback(func), _unusable(0)
                    {
                    }

                    bool
                    block(void) const
                    {
                        return compare_and_inc_not_equal(_unusable, disconnected);
                    }

                    bool
                    unblock(void) const
                    {
                        return compare_and_dec_not_equal(_unusable, disconnected);
                    }

                    /**
                     * @return true if was previously disconnected
                     */
                    bool
                    disconnect(void)
                    {
                        return _unusable.exchange(disconnected) != disconnected;
                    }

                    bool
                    usable(void) const
                    {
                        return _unusable.load() == 0;
                    }

                    bool
                    connected(void) const
                    {
                        return _unusable.load() != disconnected;
                    }

                    bool
                    extended(void) const
                    {
                        return callback.which() == 1;
                    }

                    ResultType
                    operator()(Args ... args) const
                    {
                        return boost::get<SlotFunction>(callback)(args...);
                    }

                    ResultType
                    operator()(const boost::signals3::connection& conn, Args ... args) const
                    {
                        return boost::get<ExtendedSlotFunction>(callback)(conn, args...);
                    }
                };
        }
    }
}

#endif // BOOST_SIGNALS3_SLOTS_HPP