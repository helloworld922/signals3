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

#ifndef CONNECTION_HPP_
#define CONNECTION_HPP_

namespace boost
{
    namespace signals3
    {
        class signal_base;
        namespace detail
        {
            class slot_wrapper;
        }

        class connection
        {
            template<typename Signature, typename Combiner, typename Group, typename GroupCompare,
                    typename SlotFunction, typename ExtendedSlotFunction, typename AtomicInt,
                    typename Mutex, typename UniqueLock>
                friend class signal;

            typedef ::boost::signals3::detail::slot_wrapper slot_wrapper;

            // maintains no ownership over sig
            ::boost::signals3::signal_base* sig;
            ::boost::weak_ptr< slot_wrapper > node;

            connection(::boost::signals3::signal_base* const sig, ::boost::weak_ptr< slot_wrapper > node) :
                    sig(sig), node(node)
            {
            }
        public:
            connection(void) :
                    sig(nullptr), node()
            {
            }

            connection(const connection& c) :
                    sig(c.sig), node(c.node)
            {

            }

            connection&
            operator=(const connection& c)
            {
                sig = c.sig;
                node = c.node;
                return *this;
            }

            connection&
            operator=(connection&& c)
            {
                sig = c.sig;
                node = ::std::move(c.node);
                return *this;
            }

            virtual
            ~connection(void)
            {
            }

            void
            disconnect(void) const;

            bool
            connected(void) const;

            void
            block(void);
            void
            unblock(void);
            bool
            blocked(void) const;

            bool
            usable(void) const;
        };

        class scoped_connection : public connection
        {
        public:
            scoped_connection(void)
            {}
            scoped_connection(const connection& conn) : connection(conn)
            {}
            scoped_connection(connection&& conn) : connection(::std::move(conn))
            {}

            virtual
            ~scoped_connection(void)
            {
                disconnect();
            }
        };
    }
}

#endif /* CONNECTION_HPP_ */
