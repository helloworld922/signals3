//
// slots.hpp
//
// (c) 2013 helloworld922
//
//  Created on: Jul 13, 2013
//      Author: helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_SIGNAL_HPP
#define BOOST_SIGNALS3_SIGNAL_HPP

#include <boost/thread/thread.hpp>
//#include <boost/function.hpp>
#include <iterator>
#include <functional>
#include "optional_last_value.hpp"
#include "connection.hpp"
#include "detail/extended_signature.hpp"
#include "detail/slots.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>
#include <tuple>

namespace boost
{
    namespace signals3
    {
        // =========================================================================
        // Pre-declarations
        // =========================================================================
        class connection;
        namespace detail
        {
            template<typename Signature>
                struct extended_signature;
        }

        enum connect_position
        {
            at_front, at_back
        };

        template<typename Signature, typename Combiner = optional_last_value<
                typename std::function< Signature >::result_type >, typename Group = int,
                typename GroupCompare = std::less< Group >, typename SlotFunction = std::function<
                        Signature >,
                typename ExtendedSlotFunction = typename boost::signals3::detail::extended_signature<
                        Signature >::type, typename AtomicInt = std::atomic< int >,
                typename Mutex = boost::mutex>
            class signal;

        // =========================================================================
        // definitions
        // =========================================================================
        class signal_base
        {
        public:
            virtual
            ~signal_base(void)
            {
            }
        };

        // ----------------
        // signal definition
        // ----------------
        template<typename ResultType, typename ... Args, typename Combiner, typename Group,
                typename GroupCompare, typename SlotFunction, typename ExtendedSlotFunction,
                typename AtomicInt, typename Mutex>
            class signal< ResultType
            (Args...), Combiner, Group, GroupCompare, SlotFunction, ExtendedSlotFunction, AtomicInt,
                    Mutex > : public signal_base
            {
                // typedefs to help shorten templates
                typedef ::boost::signals3::signal< ResultType
                (Args...), Combiner, Group, GroupCompare, SlotFunction, ExtendedSlotFunction,
                        AtomicInt, mutex > sig_type;

                // dummy needed because we can't fully specialize nested classes
                template<typename RType = ResultType, int dummy = 0>
                    class iterator;

                // only used for write operations
                mutable Mutex _lock;

                boost::shared_ptr< ::boost::signals3::detail::slot_wrapper > head, tail;

                Combiner combiner;
            public:
                typedef typename Combiner::result_type result_type;
                typedef Combiner combiner_type;
                typedef Group group_type;
                typedef GroupCompare group_compare_type;
                typedef SlotFunction slot_type;
                typedef ExtendedSlotFunction extended_slot_type;
                typedef AtomicInt atomic_int_type;
                typedef Mutex mutex_type;

                signal(void) :
                        _lock(), head(
                                boost::make_shared<
                                        ::boost::signals3::detail::slot_base< AtomicInt > >()), tail(
                                boost::make_shared<
                                        ::boost::signals3::detail::slot_base< AtomicInt > >()), combiner()
                {
                    // mark head and tail as unusable
                    head->mark_disconnected();
                    tail->mark_disconnected();
                    head->next = tail;
                    tail->prev = head;
                }

                void
                connect(const SlotFunction& f, const connect_position pos = at_back)
                {
                    // create node
                    boost::shared_ptr< ::boost::signals3::detail::slot_wrapper > node =
                            boost::make_shared< ::boost::signals3::detail::slot< ResultType
                            (Args...), SlotFunction > >(f);
                    // acquire unique write lock
                    boost::unique_lock< Mutex > lock(_lock);
                    if (pos == at_back)
                    {
                        boost::shared_ptr< ::boost::signals3::detail::slot_wrapper > temp =
                                tail->prev.lock();
                        node->next = tail;
                        node->prev = temp;

                        tail->prev = node;
                        // TODO: needs to be atomic
                        boost::atomic_store(&(temp->next), std::move(node));
                        //temp->next = node;
                    }
                    else
                    {
                        node->next = head->next;
                        node->prev = head;

                        head->next->prev = node;
                        // TODO: needs to be atomic
                        boost::atomic_store(&(head->next), std::move(node));
                        //head->next = node;
                    }
                    // TODO: construct connection
                    //connection conn;
                    //return conn;
                }

                void
                disconnect_all_slots(void)
                {
                    // acquire unique write lock
                    boost::unique_lock< Mutex > lock(_lock);

                    // mark all slots as disconnected
                    boost::shared_ptr< ::boost::signals3::detail::slot_wrapper > pos = head->next;
                    while (pos != tail)
                    {
                        pos->mark_disconnected();
                        pos = pos->next;
                    }
                    // TODO: needs to be atomic
                    boost::atomic_store(&(head->next), tail);
                    //head->next = tail;
                    tail->prev = head;
                }

                typename Combiner::result_type
                operator()(Args ... args) const
                {
                    // TODO: for right now, manually calls each slot
                    std::tuple< Args... > params(args...);
                    iterator< > begin(boost::atomic_load(&(head->next)), tail, std::move(params));
                    return combiner(std::move(begin));
//                    while (pos != tail)
//                    {
//                        static_cast< ::boost::signals3::detail::callable< ResultType
//                        (Args...), ::boost::signals3::detail::slot_base< AtomicInt > >& >(*pos)(
//                                args...);
//                        pos = boost::atomic_load(&(pos->next));
//                    }
                }
            };

        // ----------------
        // signal::iterator definition
        // ----------------
        template<typename ResultType, typename ... Args, typename Combiner, typename Group,
                typename GroupCompare, typename SlotFunction, typename ExtendedSlotFunction,
                typename AtomicInt, typename Mutex>
            template<typename RType, int dummy>
                class signal< ResultType
                (Args...), Combiner, Group, GroupCompare, SlotFunction, ExtendedSlotFunction,
                        AtomicInt, Mutex >::iterator    // : std::iterator< std::input_iterator_tag,
                //ResultType >
                {
                    // used for unpacking tuple to function call
                    template<int...>
                struct seq
                {};
                template<int N, int... S>
                struct gens : gens<N - 1, N - 1, S...>
                {};
                template<int... S>
                struct gens<0, S...>
                {
                    typedef seq<S...> type;
                };

                boost::shared_ptr< ::boost::signals3::detail::slot_wrapper > curr;
                const boost::shared_ptr< ::boost::signals3::detail::slot_wrapper >& end;
                std::tuple<Args...> params;

                template<int... S>
                ResultType call_func(seq<S...>) const
                {
                    return static_cast< ::boost::signals3::detail::callable< ResultType
                    (Args...), ::boost::signals3::detail::slot_base< AtomicInt > >& >(*curr)(std::get<S>(params)...);
                }
            public:
                iterator(boost::shared_ptr< ::boost::signals3::detail::slot_wrapper > n, const boost::shared_ptr< ::boost::signals3::detail::slot_wrapper >& end, std::tuple<Args...>&& args) :
                curr(std::move(n)), end(end), params(std::move(args))
                {
                }

                ResultType
                operator*(void) const
                {
                    return call_func(typename gens<sizeof...(Args)>::type());
                }

                iterator&
                operator++(void)
                {
                    if(curr != end)
                    {
                        do
                        {
                            curr = boost::atomic_load(&(curr->next));
                        }
                        while(curr != end && !curr->usable());
                    }

                    return *this;
                }

                bool is_end(void) const
                {
                    return curr == end;
                }
            };
        }
    }

#endif /* SIGNAL_HPP_ */
