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

                ::boost::signals3::detail::shared_ptr< node_base > next;
                ::boost::signals3::detail::weak_ptr< node_base > prev;
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

                struct node : public ::boost::signals3::detail::node_base
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
                };

                struct grouped_node : public node
                {
                };

                struct extended_node : public ::boost::signals3::detail::node_base
                {
                };

                struct extended_grouped_node : public extended_node
                {
                };

                // fields
                ::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base > head;
                ::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base > tail;
                ::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base > group_head;

                mutex_type _mutex;

                void
                push_back_impl(
                        ::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base >&& node)
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
                push_front_impl(
                        ::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base >&& node)
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
                        ::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base > prev =
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
                                    ::boost::signals3::detail::shared_ptr<
                                            ::boost::signals3::detail::node_base >());

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
            };
    }
}

#endif /* SIGNAL_HPP_ */
