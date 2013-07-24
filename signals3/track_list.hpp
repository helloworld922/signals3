//
// track_list.hpp
//
// (c) 2013 helloworld922
//
//  Created on: Jul 2, 2013
//      Author: helloworld922
//
// currently unused
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_TRACK_LIST_HPP
#define BOOST_SIGNALS3_TRACK_LIST_HPP

namespace boost
{
    namespace signals3
    {
        /**
         * A thread-safe singly linked list for tracking object lifetimes.
         */
        template<typename T = ::boost::signals3::detail::weak_ptr< void > >
            class track_list
            {
                struct node
                {
                    ::boost::signals3::detail::shared_ptr< node > next;
                    T data;

                    node(const T& d) :
                            data(d)
                    {
                    }

                    node(T&& d) :
                            data(std::move(d))
                    {
                    }
                };

                ::boost::signals3::detail::shared_ptr< node > head;

                void
                push_front_impl(::boost::signals3::detail::shared_ptr< node >&& n)
                {
                    ::boost::signals3::detail::shared_ptr < node > empty;
                    if (!::boost::signals3::detail::atomic_compare_exchange(&head, &empty, n))
                    {
                        // already has a head
                        do
                        {
                            n->next = ::boost::signals3::detail::atomic_load(&(head->next));
                        }
                        while (!::boost::signals3::detail::atomic_compare_exchange(&(head->next),
                                &(n->next), n));
                    }
                }

            public:
                typedef T weak_ptr_type;
                typedef ::boost::signals3::detail::shared_ptr< void > shared_ptr_type;

                track_list(void)
                {
                }

                track_list(const track_list& list)
                {
                    ::boost::signals3::detail::shared_ptr< node > iter =
                            ::boost::signals3::detail::atomic_load(&(list.head));
                    while (iter != nullptr)
                    {
                        push_front(iter->data);
                        iter = iter->next;
                    }
                }

                track_list(track_list&& list) :
                        head(std::move(list.head))
                {
                }

                track_list&
                push_front(const T& ptr)
                {
                    ::boost::signals3::detail::shared_ptr< node > n =
                            ::boost::signals3::detail::make_shared < node > (ptr);
                    push_front_impl(boost::move(n));
                    return *this;
                }

                track_list&
                push_front(T&& ptr)
                {
                    ::boost::signals3::detail::shared_ptr< node > n =
                            ::boost::signals3::detail::make_shared < node > (boost::move(ptr));
                    push_front_impl(boost::move(n));
                    return *this;
                }

                bool
                expired_unsafe(void) const
                {
                    ::boost::signals3::detail::shared_ptr< node > iter = head;
                    while (iter != nullptr)
                    {
                        if (iter->data.expired())
                        {
                            return false;
                        }
                        iter = iter->next;
                        // ::boost::signals3::detail::atomic_load(&(iter->next));
                    }
                    return true;
                }

                bool
                expired(void) const
                {
                    ::boost::signals3::detail::shared_ptr< node > iter =
                            ::boost::signals3::detail::atomic_load(&head);
                    while (iter != nullptr)
                    {
                        if (iter->data.expired())
                        {
                            return false;
                        }
                        iter = ::boost::signals3::detail::atomic_load(&(iter->next));
                    }
                    return true;
                }

                template<typename ForwardList = ::boost::signals3::detail::forward_list<
                        shared_ptr_type > >
                    bool
                    try_lock_unsafe(ForwardList& list) const
                    {
                        ::boost::signals3::detail::shared_ptr< node > iter = head;
                        while (iter != nullptr)
                        {
                            ::boost::signals3::detail::shared_ptr< void > item = iter->data.lock();
                            if (item == nullptr)
                            {
                                return false;
                            }
                            list.push_front(boost::move(item));
                            iter = iter->next;
                        }
                        return true;
                    }

                template<typename ForwardList = ::boost::signals3::detail::forward_list<
                        shared_ptr_type > >
                    bool
                    try_lock(ForwardList& list) const
                    {
                        ::boost::signals3::detail::shared_ptr< node > iter =
                                ::boost::signals3::detail::atomic_load(&head);
                        while (iter != nullptr)
                        {
                            ::boost::signals3::detail::shared_ptr< void > item = iter->data.lock();
                            if (item == nullptr)
                            {
                                return false;
                            }
                            list.push_front(boost::move(item));
                            iter = ::boost::signals3::detail::atomic_load(&(iter->next));
                        }
                        return true;
                    }
            };
    }
}

#endif // BOOST_SIGNALS3_TRACK_LIST_HPP
