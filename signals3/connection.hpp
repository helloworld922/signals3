//
// connection.hpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_CONNECTION_HPP
#define BOOST_SIGNALS3_CONNECTION_HPP

#include "detail/compiler_support.hpp"
//#include "detail/node_base.hpp"
#include "detail/signal_base.hpp"

namespace boost
{
    namespace signals3
    {
        class connection
        {
            // maintains no ownership over _sig
            ::boost::signals3::detail::signal_base* _sig;
            ::boost::signals3::detail::weak_ptr< ::boost::signals3::detail::node_base > _node;
        public:
            connection(void) :
                    _sig(nullptr)
            {
            }

            connection(::boost::signals3::detail::signal_base* _sig,
                    const ::boost::signals3::detail::weak_ptr< ::boost::signals3::detail::node_base >& _node) :
                    _sig(_sig), _node(_node)
            {
            }

            connection(::boost::signals3::detail::signal_base* _sig,
                    ::boost::signals3::detail::weak_ptr< ::boost::signals3::detail::node_base >&& _node) :
                    _sig(_sig), _node(boost::move(_node))
            {
            }

            virtual
            ~connection(void)
            {
            }

            bool
            blocked(void) const
            {
                ::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base > temp =
                        _node.lock();
                if (temp != nullptr)
                {
                    return temp->blocked();
                }
                return false;
            }

            bool
            block(void) const
            {
                ::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base > temp =
                        _node.lock();
                if (temp != nullptr)
                {
                    return temp->block();
                }
                return false;
            }
            bool
            unblock(void) const
            {
                ::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base > temp =
                        _node.lock();
                if (temp != nullptr)
                {
                    return temp->unblock();
                }
                return false;
            }

            void
            disconnect(void)
            {
                ::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base > temp =
                        _node.lock();
                if (temp != nullptr)
                {
                    _sig->disconnect(boost::move(temp));
                }
            }

            bool
            connected(void) const
            {
                ::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base > temp =
                        _node.lock();
                if (temp != nullptr)
                {
                    return temp->connected();
                }
                return false;
            }

            bool
            usable(void) const
            {
                ::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base > temp =
                        _node.lock();
                if (temp != nullptr)
                {
                    return temp->usable();
                }
                return false;
            }
        };
    }
}

#endif // BOOST_SIGNALS3_CONNECTION_HPP
