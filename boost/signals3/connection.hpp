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

#include <boost/signals3/detail/compiler_support.hpp>
//#include <boost/signals3/detail/node_base.hpp>
#include <boost/signals3/detail/signal_base.hpp>

namespace boost
{
    namespace signals3
    {
        class scoped_connection;

        template<typename Signature, typename Combiner, typename Group, typename GroupCompare,
                typename FunctionType, typename ExtendedFunctionType, typename Mutex>
            class signal;

        class connection
        {
            friend class scoped_connection;

            template<typename Signature, typename Combiner, typename Group, typename GroupCompare,
                    typename FunctionType, typename ExtendedFunctionType, typename Mutex>
                friend class ::boost::signals3::signal;

            // maintains no ownership over _sig
            ::boost::signals3::detail::signal_base* _sig;
            ::boost::signals3::detail::weak_ptr< ::boost::signals3::detail::node_base > _node;

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
        public:
            connection(void) :
                    _sig(nullptr)
            {
            }

            connection(const connection& conn) :
                    _sig(conn._sig), _node(conn._node)
            {
            }

            connection(connection&& conn) :
                    _sig(conn._sig), _node(boost::move(conn._node))
            {
            }

            connection&
            operator=(const connection& conn)
            {
                _sig = conn._sig;
                _node = conn._node;
                return *this;
            }

            connection&
            operator=(connection&& conn)
            {
                _sig = boost::move(conn._sig);
                _node = boost::move(conn._node);
                return *this;
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

            void
            disconnect_unsafe(void)
            {
                ::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base > temp =
                        _node.lock();
                if (temp != nullptr)
                {
                    _sig->disconnect_unsafe(boost::move(temp));
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

        class scoped_connection : public connection
        {
        public:
            scoped_connection(const connection& conn) :
                    connection(conn)
            {

            }

            scoped_connection(connection&& conn) :
                    connection(boost::move(conn))
            {

            }

            scoped_connection&
            operator=(const connection& conn)
            {
                disconnect();
                _sig = conn._sig;
                _node = conn._node;
                return *this;
            }

            scoped_connection&
            operator=(connection&& conn)
            {
                disconnect();
                _sig = boost::move(conn._sig);
                _node = boost::move(conn._node);
                return *this;
            }

            /**
             * Not thread safe!
             */
            connection
            release(void)
            {
                connection conn(*this);
                _sig = nullptr;
                _node.reset();
                return conn;
            }

            ~scoped_connection(void)
            {
                disconnect();
            }
        };
    }
}

#endif // BOOST_SIGNALS3_CONNECTION_HPP
