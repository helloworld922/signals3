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

namespace boost
{
    namespace signals3
    {
        // =========================================================================
        // Pre-declarations
        // =========================================================================

        template<typename Signature, typename Combiner = ::boost::signals3::optional_last_value<
                typename ::std::function< Signature >::result_type >, typename Group = int,
                typename GroupCompare = ::std::less< Group >,
                typename SlotFunction = ::std::function< Signature >,
                typename ExtendedSlotFunction = typename ::boost::signals3::detail::extended_signature<
                        Signature >::type, typename AtomicInt = ::std::atomic< int >,
                typename Mutex = ::boost::mutex, typename UniqueLock = ::boost::unique_lock< Mutex > >
            class signal;

        // =========================================================================
        // definitions
        // =========================================================================
        class signal_base
        {
            friend class connection;
        protected:
            // typedefs for slots
            typedef ::boost::signals3::detail::slot_wrapper slot_wrapper;
            virtual void
            disconnect(::boost::shared_ptr< slot_wrapper >&& node) = 0;
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
                typename AtomicInt, typename Mutex, typename UniqueLock>
            class signal< ResultType
            (Args...), Combiner, Group, GroupCompare, SlotFunction, ExtendedSlotFunction, AtomicInt,
                    Mutex, UniqueLock > : public ::boost::signals3::signal_base
            {
                friend class connection;

                typedef ::boost::signals3::detail::slot< ResultType
                (Args...), SlotFunction, AtomicInt > slot;

                typedef ::boost::signals3::detail::extended_slot< ResultType
                (Args...), ExtendedSlotFunction, AtomicInt > extended_slot;

                typedef ::boost::signals3::detail::grouped_slot< ResultType
                (Args...), SlotFunction, Group, AtomicInt > grouped_slot;

                typedef ::boost::signals3::detail::grouped_extended_slot< ResultType
                (Args...), ExtendedSlotFunction, Group, AtomicInt > grouped_extended_slot;

                class iterator;

                ::boost::shared_ptr< slot_wrapper > head;
                ::boost::shared_ptr< slot_wrapper > tail;
                // group nodes to be inserted in group_head->next
                ::boost::shared_ptr< slot_wrapper > group_head;

                Combiner combiner;

                // only used for write operations
                mutable Mutex _lock;

                GroupCompare comparator;

                void
                push_front_impl(::boost::shared_ptr< slot_wrapper >&& node)
                {
                    // acquire unique write lock
                    UniqueLock lock(_lock);
                    if (head == nullptr)
                    {
                        tail = node;
                        group_head = node;
                    }
                    else
                    {
                        node->next = head;
                        head->prev = node;
                    }
                    ::boost::atomic_store(&head, ::std::move(node));
                }

                void
                push_back_impl(::boost::shared_ptr< slot_wrapper >&& node)
                {
                    // acquire unique write lock
                    UniqueLock lock(_lock);
                    if (tail == nullptr)
                    {
                        // previously had no nodes
                        tail = node;
                        ::boost::atomic_store(&head, ::std::move(node));
                    }
                    else
                    {
                        node->prev = tail;
                        ::boost::atomic_store(&(tail->next), ::std::move(node));
                        tail = tail->next;
                    }
                }

                void
                insert_impl(::boost::shared_ptr< slot_wrapper >&& node, const Group& g)
                {
                    // acquire unique write lock
                    UniqueLock lock(_lock);

                }
            protected:
                virtual void
                disconnect(::boost::shared_ptr< slot_wrapper >&& node) override
                {
                    if (node == head)
                    {
                        pop_front();
                    }
                    else if (node == tail)
                    {
                        pop_back();
                    }
                    else
                    {
                        // acquire unique write lock
                        UniqueLock lock(_lock);
                        if (node->mark_disconnected())
                        {
                            // remove node in the middle
                            ::boost::shared_ptr< slot_wrapper > temp = node->prev.lock();
                            ::boost::atomic_store(&(temp->next), node->next);
                            node->next->prev = temp;
                            if (node == group_head)
                            {
                                // need to move group_head
                                group_head = std::move(temp);
                            }
                        }
                    }
                }

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
                        head(), tail(), group_head(), combiner(), _lock(), comparator()
                {
                }

                void
                erase(const SlotFunction& f)
                {
                    // acquire unique write lock
                    UniqueLock lock(_lock);
                    ::boost::shared_ptr< slot_wrapper > iter = head;
                    while (iter != nullptr)
                    {
                        if (!iter->extended())
                        {
                            ::boost::shared_ptr< slot > temp = ::boost::static_pointer_cast< slot >(
                                    iter);
                        }
                        iter = iter->next;
                    }
                }

                void
                erase_extended(const ExtendedSlotFunction& f)
                {
                    // acquire unique write lock
                    UniqueLock lock(_lock);
                    ::boost::shared_ptr< slot_wrapper > iter = head;
                    while (iter != nullptr)
                    {
                        if (iter->extended())
                        {
                            ::boost::shared_ptr< slot > temp = ::boost::static_pointer_cast< extended_slot >(
                                    iter);
                            // TODO
                        }
                        iter = iter->next;
                    }
                }

                connection
                insert(const SlotFunction& f, const Group& group)
                {
                    ::boost::shared_ptr< grouped_slot > node = ::boost::make_shared< grouped_slot >(
                            f, group);
                    connection conn(this, node);
                    insert_impl(::std::move(node), group);
                    return conn;
                }

                connection
                insert_extended(const ExtendedSlotFunction& f, const Group& group)
                {
                    ::boost::shared_ptr< grouped_extended_slot > node = ::boost::make_shared<
                            grouped_extended_slot >(f, group);
                    connection conn(this, node);
                    node->conn = conn;
                    insert_impl(::std::move(node), group);
                    return conn;
                }

                connection
                push_front(const SlotFunction& f)
                {
                    ::boost::shared_ptr< slot > node = ::boost::make_shared< slot >(f);
                    connection conn(this, node);
                    push_front_impl(::std::move(node));
                    return conn;
                }

                connection
                push_front_extended(const ExtendedSlotFunction& f)
                {
                    ::boost::shared_ptr< extended_slot > node =
                            ::boost::make_shared< extended_slot >(f);
                    connection conn(this, node);
                    node->conn = conn;
                    push_front_impl(::std::move(node));
                    return conn;
                }

                connection
                push_back(const SlotFunction& f)
                {
                    ::boost::shared_ptr< slot > node = ::boost::make_shared< slot >(f);
                    connection conn(this, node);
                    push_back_impl(::std::move(node));
                    return conn;
                }

                connection
                push_back_extended(const ExtendedSlotFunction& f)
                {
                    ::boost::shared_ptr< extended_slot > node =
                            ::boost::make_shared< extended_slot >(f);
                    connection conn(this, node);
                    node->conn = conn;
                    push_back_impl(::std::move(node));
                    return conn;
                }

                void
                pop_front(void)
                {
                    // acquire unique write lock
                    UniqueLock lock(_lock);
                    if (head != nullptr)
                    {
                        head->mark_disconnected();
                        if (group_head == head)
                        {
                            group_head = nullptr;
                        }
                        ::boost::atomic_store(&head, head->next);
                        if (head != nullptr)
                        {
                            head->prev.reset();
                        }
                        else
                        {
                            // now have no slots left
                            group_head = nullptr;
                            tail = nullptr;
                        }
                    }
                }

                void
                pop_back(void)
                {
                    // acquire unique write lock
                    UniqueLock lock(_lock);
                    if (tail != nullptr)
                    {
                        tail->mark_disconnected();
                        if (head != tail)
                        {
                            if (group_head == tail)
                            {
                                group_head = tail->prev.lock();
                            }
                            tail = tail->prev.lock();
                            ::boost::atomic_store(&(tail->next),
                                    ::std::move(::boost::shared_ptr< slot_wrapper >()));
                        }
                        else
                        {
                            // no slots left
                            tail = nullptr;
                            group_head = nullptr;
                            ::boost::atomic_store(&head, tail);
                        }
                    }
                }

                void
                clear(void)
                {
                    // acquire unique write lock
                    UniqueLock lock(_lock);

                    // mark all slots as disconnected
                    ::boost::shared_ptr< slot_wrapper > pos = head;
                    while (pos != nullptr)
                    {
                        pos->mark_disconnected();
                        pos = pos->next;
                    }
                    tail = nullptr;
                    group_head = nullptr;
                    ::boost::atomic_store(&head, tail);
                }

                typename Combiner::result_type
                operator()(Args ... args) const
                {
                    ::boost::shared_ptr< slot_wrapper > begin_ptr = ::boost::atomic_load(&head);
                    while (begin_ptr != nullptr && !begin_ptr->usable())
                    {
                        begin_ptr = ::boost::atomic_load(&(begin_ptr->next));
                    }
                    const ::std::tuple< Args... > params(args...);
                    iterator begin(::std::move(begin_ptr), params);
                    iterator end(::boost::shared_ptr< slot_wrapper >(), params);
                    return combiner(::std::move(begin), ::std::move(end));
                }
            };

        void
        connection::disconnect() const
        {
            ::boost::shared_ptr< slot_wrapper > n = node.lock();
            if (n != nullptr)
            {
                sig->disconnect(::std::move(n));
            }
        }

        // ----------------
        // signal::iterator definition
        // ----------------
        template<typename ResultType, typename ... Args, typename Combiner, typename Group,
                typename GroupCompare, typename SlotFunction, typename ExtendedSlotFunction,
                typename AtomicInt, typename Mutex, typename UniqueLock>
            class signal< ResultType
            (Args...), Combiner, Group, GroupCompare, SlotFunction, ExtendedSlotFunction, AtomicInt,
                    Mutex, UniqueLock >::iterator
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

            ::boost::shared_ptr< ::boost::signals3::detail::slot_wrapper > curr;
            const ::std::tuple<Args...>& params;

            template<int... S>
            ResultType call_func(seq<S...>) const
            {
                return static_cast< ::boost::signals3::detail::slot_base< ResultType
                (Args...), AtomicInt >& >(*curr)(::std::get<S>(params)...);
            }

        public:
            typedef ::std::input_iterator_tag iterator_category;
            typedef ResultType value_type;

            iterator(::boost::shared_ptr< ::boost::signals3::detail::slot_wrapper >&& n,
                    const ::std::tuple< Args... >& args) :
            curr(::std::move(n)), params(args)
            {
            }

            ResultType
            operator*(void) const
            {
                return call_func(typename gens< sizeof...(Args)>::type());
            }

            iterator&
            operator++(void)
            {
                if(curr != nullptr)
                {
                    do
                    {
                        curr = ::boost::atomic_load(&(curr->next));
                    }
                    while(curr != nullptr && !curr->usable());
                }

                return *this;
            }

            bool
            operator==(const iterator& iter) const
            {
                return curr == iter.curr;
            }

            bool
            operator!=(const iterator& iter) const
            {
                return curr != iter.curr;
            }
        };
    }
}

#endif /* SIGNAL_HPP_ */
