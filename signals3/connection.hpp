//
// connection.hpp
//
// (c) 2013 helloworld922
//
//  Created on: Jul 13, 2013
//      Author: helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_CONNECTION_HPP
#define BOOST_SIGNALS3_CONNECTION_HPP

namespace boost
{
    namespace signals3
    {
        namespace detail
        {
            class signal_base;
            class node_base;
        }

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
        };
    }
}

#endif // BOOST_SIGNALS3_CONNECTION_HPP
