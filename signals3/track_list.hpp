/*
 * track_list.hpp
 *
 * (c) 2013 helloworld922
 *
 *  Created on: Jul 22, 2013
 *      Author: helloworld922
 */

#ifndef BOOST_SIGNALS3_TRACK_LIST_HPP
#define BOOST_SIGNALS3_TRACK_LIST_HPP

namespace boost
{
    namespace signals3
    {
        /**
         * A thread-safe singly linked list for tracking object lifetimes.
         */
        template<typename T = boost::weak_ptr< void > >
            class track_list
            {
                struct node
                {
                    boost::shared_ptr< node > next;
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

                boost::shared_ptr< node > head;

                void
                push_front_impl(boost::shared_ptr< node >&& n)
                {
                    static boost::shared_ptr< node > empty;
                    if (!boost::atomic_compare_exchange(&head, &empty, n))
                    {
                        // already has a head
                        do
                        {
                            n->next = boost::atomic_load(&(head->next));
                        }
                        while (!boost::atomic_compare_exchange(&(head->next), &(n->next), n));
                    }
                }

            public:
                typedef T weak_ptr_type;
                typedef boost::shared_ptr< void > shared_ptr_type;

                track_list(void)
                {
                }

                track_list(const track_list& list)
                {
                    boost::shared_ptr< node > iter = boost::atomic_load(&(list.head));
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
                    boost::shared_ptr< node > n = boost::make_shared< node >(ptr);
                    push_front_impl(std::move(n));
                    return *this;
                }

                track_list&
                push_front(T&& ptr)
                {
                    boost::shared_ptr< node > n = boost::make_shared< node >(std::move(ptr));
                    push_front_impl(std::move(n));
                    return *this;
                }

                template<typename ForwardList = std::forward_list< shared_ptr_type > >
                    bool
                    try_lock(ForwardList& list) const
                    {
                        boost::shared_ptr< node > iter = boost::atomic_load(&head);
                        while (iter != nullptr)
                        {
                            boost::shared_ptr< void > item = iter->data.lock();
                            if (item == nullptr)
                            {
                                return false;
                            }
                            list.push_front(std::move(item));
                            iter = iter->next;
                        }
                        return true;
                    }
            };
    }
}

#endif // BOOST_SIGNALS3_TRACK_LIST_HPP
