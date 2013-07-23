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
        template<typename Signature, typename Combiner = optional_last_value<
                typename std::function< Signature >::result_type >, typename Group = int,
                typename GroupCompare = std::less< Group >, typename FunctionType = std::function<
                        Signature > >
            class signal;

        namespace detail
        {
            class signal_base
            {
            public:
                virtual
                ~signal_base(void)
                {
                }
            };

            class node_base
            {
                template<typename Signature, typename Combiner, typename Group,
                        typename GroupCompare, typename FunctionType>
                    friend class ::boost::signals3::signal;

            public:
                virtual
                ~node_base(void)
                {
                }
            };
        }

        template<typename ResultType, typename ... Args, typename Combiner, typename Group,
                typename GroupCompare, typename FunctionType>
            class signal< ResultType
            (Args...), Combiner, Group, GroupCompare, FunctionType > : public ::boost::signals3::detail::signal_base
            {
            public:
                // typedefs
                typedef boost::signals3::detail::mutex mutex_type;
                typedef boost::signals3::detail::unique_lock< mutex_type > unique_lock_type;
                typedef ResultType result_type;
                typedef Combiner combiner_type;
                typedef Group group_type;
                typedef GroupCompare group_compare_type;
                typedef boost::signals3::slot< ResultType
                (Args...), FunctionType > slot_type;
                typedef std::atomic< int > atomic_int_type;
            public:
                struct t_node_base : public ::boost::signals3::detail::node_base
                {
                    ::boost::signals3::detail::shared_ptr< t_node_base > next;
                    ::boost::signals3::detail::weak_ptr< t_node_base > prev;
                    ::boost::signals3::detail::atomic< int > _unusable;

                    static const int _disconnected = INT_MIN;

                    virtual bool
                    try_lock(
                            ::boost::signals3::detail::forward_list<
                                    ::boost::signals3::detail::shared_ptr< void > >& list) const = 0;

                    bool
                    usable(void) const
                    {
                        int val = _unusable.load();
                        return val != _disconnected && val <= 0;
                        //return _unusable.load() == 0;
                    }

                    virtual ResultType
                    operator()(Args ... args) const = 0;
                };

                struct node : public t_node_base
                {
                    slot_type callback;
                    node(const slot_type& callback) :
                            callback(callback)
                    {
                    }

                    node(slot_type&& callback) :
                            callback(boost::move(callback))
                    {
                    }

                    virtual bool
                    try_lock(
                            ::boost::signals3::detail::forward_list<
                                    ::boost::signals3::detail::shared_ptr< void > >& list) const override
                    {
                        return callback.try_lock(list);
                    }

                    virtual ResultType
                    operator()(Args&&... args) const override
                    {
                        return callback.slot_function()(std::forward(args)...);
                    }
                };

                struct grouped_node : public node
                {
                };

                struct extended_node : public t_node_base
                {
                };

                struct extended_grouped_node : public extended_node
                {
                };

                class iterator;

                class unsafe_iterator;

                // fields
                ::boost::signals3::detail::shared_ptr< t_node_base > head;
                ::boost::signals3::detail::shared_ptr< t_node_base > tail;
                ::boost::signals3::detail::shared_ptr< t_node_base > group_head;

                mutex_type _mutex;

                Combiner combiner;

                GroupCompare group_compare;

                void
                push_back_impl(::boost::signals3::detail::shared_ptr< t_node_base >&& node)
                {
                    unique_lock_type _lock(_mutex);
                    if (tail == nullptr)
                    {
                        // only one node
                        tail = node;
                        ::boost::signals3::detail::atomic_store(&head, boost::move(node));
                    }
                    else
                    {
                        // more than one node
                        node->prev = tail;
                        ::boost::signals3::detail::atomic_store(&(tail->next), node);
                        tail = boost::move(node);
                    }
                }

                void
                push_front_impl(::boost::signals3::detail::shared_ptr< t_node_base >&& node)
                {
                    unique_lock_type _lock(_mutex);
                    if (head == nullptr)
                    {
                        // only one node
                        tail = node;
                        group_head = node;
                    }
                    else
                    {
                        // more than one node
                        if (group_head == nullptr)
                        {
                            group_head = node;
                        }
                        node->next = head;
                    }
                    ::boost::signals3::detail::atomic_store(&head, boost::move(node));
                }

                void
                push_back_impl_unsafe(::boost::signals3::detail::shared_ptr< t_node_base >&& node)
                {
                    if (tail == nullptr)
                    {
                        // only one node
                        tail = node;
                        head = boost::move(node);
                    }
                    else
                    {
                        // more than one node
                        node->prev = tail;
                        tail->next = node;
                        tail = boost::move(node);
                    }
                }

                void
                push_front_impl_unsafe(::boost::signals3::detail::shared_ptr< t_node_base >&& node)
                {
                    if (head == nullptr)
                    {
                        // only one node
                        tail = node;
                        group_head = node;
                    }
                    else
                    {
                        // more than one node
                        if (group_head == nullptr)
                        {
                            group_head = node;
                        }
                        node->next = head;
                    }
                    head = boost::move(node);
                }

            public:
                connection
                push_back(const slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr < node > n(callback);
                    ::boost::signals3::connection conn(this, n);
                    push_back_impl(boost::move(n));
                    return conn;
                }

                connection
                push_back(slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr< node > n =
                            ::boost::signals3::detail::make_shared < node > (callback);
                    ::boost::signals3::connection conn(this, n);
                    push_back_impl(boost::move(n));
                    return conn;
                }

                connection
                push_front(const slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr < node > n(callback);
                    ::boost::signals3::connection conn(this, n);
                    push_front_impl(boost::move(n));
                    return conn;
                }

                connection
                push_front(slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr< node > n =
                            ::boost::signals3::detail::make_shared < node > (callback);
                    ::boost::signals3::connection conn(this, n);
                    push_front_impl(boost::move(n));
                    return conn;
                }

                void
                pop_back(void)
                {
                    unique_lock_type _lock(_mutex);
                    if (tail != nullptr)
                    {
                        // actually have a node to remove
                        ::boost::signals3::detail::shared_ptr< t_node_base > prev =
                                tail->prev.lock();
                        if (prev != nullptr)
                        {
                            // more than 1 node
                            tail->prev.reset();
                            if (group_head == tail)
                            {
                                group_head = prev;
                            }
                            ::boost::signals3::detail::atomic_store(&(prev->next),
                                    ::boost::signals3::detail::shared_ptr< t_node_base >());

                            tail = boost::move(prev);
                        }
                        else
                        {
                            // only one node
                            tail.reset();
                            group_head.reset();
                            ::boost::signals3::detail::atomic_store(&head, tail);
                        }
                    }
                }

                void
                pop_front(void)
                {
                    unique_lock_type _lock(_mutex);
                    if (head != nullptr)
                    {
                        // actually have a node to remove
                        if (head == tail)
                        {
                            // only have one node
                            tail.reset();
                            group_head.reset();
                            ::boost::signals3::detail::atomic_store(&head, tail);
                        }
                        else
                        {
                            // more than one node
                            if (group_head == head)
                            {
                                group_head.reset();
                            }
                            ::boost::signals3::detail::atomic_store(&head, head->next);
                        }
                    }
                }

                connection
                push_back_unsafe(const slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr < node > n(callback);
                    ::boost::signals3::connection conn(this, n);
                    push_back_impl_unsafe(boost::move(n));
                    return conn;
                }

                connection
                push_back_unsafe(slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr< node > n =
                            ::boost::signals3::detail::make_shared < node > (callback);
                    ::boost::signals3::connection conn(this, n);
                    push_back_impl_unsafe(boost::move(n));
                    return conn;
                }

                connection
                push_front_unsafe(const slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr < node > n(callback);
                    ::boost::signals3::connection conn(this, n);
                    push_front_impl_unsafe(boost::move(n));
                    return conn;
                }

                connection
                push_front_unsafe(slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr< node > n =
                            ::boost::signals3::detail::make_shared < node > (callback);
                    ::boost::signals3::connection conn(this, n);
                    push_front_impl_unsafe(boost::move(n));
                    return conn;
                }

                void
                pop_back_unsafe(void)
                {
                    if (tail != nullptr)
                    {
                        // actually have a node to remove
                        ::boost::signals3::detail::shared_ptr< t_node_base > prev =
                                tail->prev.lock();
                        if (prev != nullptr)
                        {
                            // more than 1 node
                            tail->prev.reset();
                            if (group_head == tail)
                            {
                                group_head = prev;
                            }
                            prev->next.reset();

                            tail = boost::move(prev);
                        }
                        else
                        {
                            // only one node
                            tail.reset();
                            group_head.reset();
                            head.reset();
                        }
                    }
                }

                void
                pop_front_unsafe(void)
                {
                    if (head != nullptr)
                    {
                        // actually have a node to remove
                        if (head == tail)
                        {
                            // only have one node
                            tail.reset();
                            group_head.reset();
                            head.reset();
                        }
                        else
                        {
                            // more than one node
                            if (group_head == head)
                            {
                                group_head.reset();
                            }
                            head = head->next;
                        }
                    }
                }

                /**
                 * Thread safe emit
                 */
                typename Combiner::result_type
                operator()(Args ... args)
                {
                    ::boost::signals3::detail::forward_list
                            < ::boost::signals3::detail::shared_ptr< void > > tracking_list;
                    ::boost::signals3::detail::tuple< Args... > params(args...);
                    ::boost::signals3::detail::shared_ptr< t_node_base > begin_ptr =
                            ::boost::signals3::detail::atomic_load(&head);
                    while (begin_ptr != nullptr)
                    {
                        if (!begin_ptr->usable())
                        {
                            begin_ptr = ::boost::signals3::detail::atomic_load(&(begin_ptr->next));
                        }
                        else if (!begin_ptr->try_lock(tracking_list))
                        {
                            // TODO: automatic disconnect
                            tracking_list.clear();
                            begin_ptr = ::boost::signals3::detail::atomic_load(&(begin_ptr->next));
                        }
                        else
                        {
                            break;
                        }
                    }
                    iterator begin(boost::move(begin_ptr), params, tracking_list);
                    iterator end(nullptr, params, tracking_list);
                    return combiner(boost::move(begin), boost::move(end));
                }

                typename Combiner::result_type
                emit(Args ... args)
                {
                    ::boost::signals3::detail::forward_list
                            < ::boost::signals3::detail::shared_ptr< void > > tracking_list;
                    ::boost::signals3::detail::tuple< Args... > params(args...);
                    ::boost::signals3::detail::shared_ptr< t_node_base > begin_ptr =
                            ::boost::signals3::detail::atomic_load(&head);
                    while (begin_ptr != nullptr)
                    {
                        if (!begin_ptr->usable())
                        {
                            begin_ptr = ::boost::signals3::detail::atomic_load(&(begin_ptr->next));
                        }
                        else if (!begin_ptr->try_lock(tracking_list))
                        {
                            // TODO: automatic disconnect
                            tracking_list.clear();
                            begin_ptr = ::boost::signals3::detail::atomic_load(&(begin_ptr->next));
                        }
                        else
                        {
                            break;
                        }
                    }
                    iterator begin(boost::move(begin_ptr), params, tracking_list);
                    iterator end(nullptr, params, tracking_list);
                    return combiner(boost::move(begin), boost::move(end));
                }

                typename Combiner::result_type
                emit_unsafe(Args ... args)
                {
                    ::boost::signals3::detail::forward_list
                            < ::boost::signals3::detail::shared_ptr< void > > tracking_list;
                    ::boost::signals3::detail::tuple< Args... > params(args...);
                    ::boost::signals3::detail::shared_ptr< t_node_base > begin_ptr =
                            ::boost::signals3::detail::atomic_load(&head);
                    while (begin_ptr != nullptr)
                    {
                        if (!begin_ptr->usable())
                        {
                            begin_ptr = begin_ptr->next;
                        }
                        else if (!begin_ptr->try_lock(tracking_list))
                        {
                            // TODO: automatic disconnect
                            tracking_list.clear();
                            begin_ptr = begin_ptr->next;
                        }
                        else
                        {
                            break;
                        }
                    }
                    unsafe_iterator begin(boost::move(begin_ptr), params, tracking_list);
                    unsafe_iterator end(nullptr, params, tracking_list);
                    return combiner(boost::move(begin), boost::move(end));
                }
            };

        template<typename ResultType, typename ... Args, typename Combiner, typename Group,
                typename GroupCompare, typename FunctionType>
            class signal< ResultType
            (Args...), Combiner, Group, GroupCompare, FunctionType >::iterator
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

                ::boost::signals3::detail::shared_ptr< t_node_base > curr;
                ::boost::signals3::detail::tuple< Args... >& params;
                ::boost::signals3::detail::forward_list<
                ::boost::signals3::detail::shared_ptr< void > >& tracking;

                template<int... S>
                ResultType call_func(seq<S...>) const
                {
                    return (*curr)(std::forward(std::get<S>(params))...);
                }
            public:
                iterator(::boost::signals3::detail::shared_ptr< t_node_base >&& start_node,
                        ::boost::signals3::detail::tuple< Args... >& params,
                        ::boost::signals3::detail::forward_list<
                        ::boost::signals3::detail::shared_ptr< void > >& tracking) :
                curr(boost::move(start_node)), params(params), tracking(tracking)
                {

                }

                ResultType
                operator*(void) const
                {
                    // TODO
                    return call_func(typename gens<sizeof...(Args)>::type());
                }

                iterator&
                operator++(void)
                {
                    // release any locks we might have
                    //tracking.clear();
                    if (curr != nullptr)
                    {
                        while (true)
                        {
                            curr = ::boost::signals3::detail::atomic_load(&(curr->next));
                            if (curr == nullptr)
                            {
                                return *this;
                            }
                            else if (curr->usable())
                            {
                                if (curr->try_lock(tracking))
                                {
                                    return *this;
                                }
                                else
                                {
                                    // TODO: automatic disconnect
                                    tracking.clear();
                                }
                            }
                        }
                    }
                    return *this;
                }

                bool
                operator ==(const iterator& rhs) const
                {
                    return curr == rhs.curr;
                }

                bool
                operator !=(const iterator& rhs) const
                {
                    return curr != rhs.curr;
                }
            };

        template<typename ResultType, typename ... Args, typename Combiner, typename Group,
                typename GroupCompare, typename FunctionType>
            class signal< ResultType
            (Args...), Combiner, Group, GroupCompare, FunctionType >::unsafe_iterator
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

            ::boost::signals3::detail::shared_ptr< t_node_base > curr;
            ::boost::signals3::detail::tuple< Args... >& params;
            ::boost::signals3::detail::forward_list<
            ::boost::signals3::detail::shared_ptr< void > >& tracking;

            template<int... S>
            ResultType call_func(seq<S...>) const
            {
                return (*curr)(std::forward(std::get<S>(params))...);
            }
        public:
            unsafe_iterator(::boost::signals3::detail::shared_ptr< t_node_base >&& start_node,
                    ::boost::signals3::detail::tuple< Args... >& params,
                    ::boost::signals3::detail::forward_list<
                    ::boost::signals3::detail::shared_ptr< void > >& tracking) :
            curr(boost::move(start_node)), params(params), tracking(tracking)
            {

            }

            ResultType
            operator*(void) const
            {
                // TODO
                return call_func(typename gens<sizeof...(Args)>::type());
            }

            unsafe_iterator&
            operator++(void)
            {
                // release any locks we might have
                tracking.clear();
                if (curr != nullptr)
                {
                    while (true)
                    {
                        curr = curr->next;
                        if (curr == nullptr)
                        {
                            return *this;
                        }
                        if (curr->usable())
                        {
                            if (curr->try_lock(tracking))
                            {
                                return *this;
                            }
                            else
                            {
                                // TODO: automatic disconnect
                                tracking.clear();
                            }
                        }
                    }
                }
                return *this;
            }

            bool
            operator ==(const unsafe_iterator& rhs) const
            {
                return curr == rhs.curr;
            }

            bool
            operator !=(const unsafe_iterator& rhs) const
            {
                return curr != rhs.curr;
            }
        };
    }
}

#endif /* SIGNAL_HPP_ */
