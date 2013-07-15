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
#include <tuple>

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
            typename AtomicInt = std::atomic<int>,
            typename SharedLock = boost::shared_mutex >
            class signal;

        template<typename ResultType, typename ... Args, typename Combiner, typename Group,
                typename GroupCompare, typename SlotFunction, typename ExtendedSlotFunction,
                typename AtomicInt, typename SharedLock>
            class signal<ResultType(Args...), Combiner, Group, GroupCompare, SlotFunction, ExtendedSlotFunction, AtomicInt, SharedLock>
            {
                mutable SharedLock _lock;
                std::shared_ptr< boost::signals3::detail::slot<ResultType(Args...), SlotFunction, ExtendedSlotFunction, AtomicInt> > head;
                std::shared_ptr< boost::signals3::detail::slot<ResultType(Args...), SlotFunction, ExtendedSlotFunction, AtomicInt> > tail;
                Combiner combiner;

                class iterator
                {
                    template<int ...>
                    struct seq{};

                    template<int N, int... S>
                    struct gens : gens<N - 1, N - 1, S...> {};

                    template<int... S>
                    struct gens<0, S...> {
                        typedef seq<S...> type;
                    };

                    const boost::signals3::signal<ResultType(Args...), Combiner, Group, GroupCompare, SlotFunction, ExtendedSlotFunction, AtomicInt, SharedLock>& sig;
                    std::shared_ptr< boost::signals3::detail::slot<ResultType(Args...), SlotFunction, ExtendedSlotFunction, AtomicInt> > callback;
                    std::tuple<Args...> params;

                    template<int... S>
                    ResultType call_func(seq<S...>)
                    {
                        // TODO: differentiate between standard and extended slots
                        return (*callback)(std::get<S>(params)...);
                    }

                public:
                    iterator(const boost::signals3::signal<ResultType(Args...),
                            Combiner, Group, GroupCompare, SlotFunction,
                            ExtendedSlotFunction, AtomicInt, SharedLock>& sig,
                            std::shared_ptr<boost::signals3::detail::slot<ResultType(Args...), SlotFunction, ExtendedSlotFunction, AtomicInt> > cb,
                            Args&&... args) : sig(sig), callback(cb), params(std::forward(args)...)
                    {
                        sig._lock.lock_shared();
                    }

                    ~iterator(void)
                    {
                        sig._lock.unlock_shared();
                    }

                    bool operator ==(const iterator& rhs) const
                    {
                        return callback == rhs.callback;
                    }

                    bool operator !=(const iterator& rhs) const
                    {
                        return callback != rhs.callback;
                    }

                    /**
                     * Invokes the current slot
                     */
                    ResultType operator *(void)
                    {
                        return call_func(typename gens<sizeof...(Args)>::type());
                        //std::shared_ptr< boost::signals3::detail::slot<ResultType(Args...), SlotFunction, ExtendedSlotFunction, AtomicInt> > callback(s);
                    }

                    /**
                     * Moves this iterator to the next usable slot
                     */
                    iterator& operator++(void)
                    {
                        if(callback != nullptr)
                        {
                            do
                            {
                                callback = callback->next;
                            }
                            while(callback != nullptr && callback->usable());
                        }
                        return *this;
                    }
                };
            public:
                typedef typename Combiner::result_type result_type;
                typedef Combiner combiner_type;
                typedef Group group_type;
                typedef GroupCompare group_compare_type;
                typedef SlotFunction slot_type;
                typedef ExtendedSlotFunction extended_slot_type;
                typedef SharedLock shared_lock_type;

                signal(void) : _lock(), head(), tail(), combiner()
                {}

                typename Combiner::result_type
                operator()(Args ... args)
                {
                    // TODO: create begin/end iterators
                    iterator begin(*this, head, args...);
                    iterator end(*this, nullptr, args...);
                    return combiner(begin, end);
                }
            };
    }
}

#endif /* SIGNAL_HPP_ */
