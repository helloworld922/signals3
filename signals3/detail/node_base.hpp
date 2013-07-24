//
// node_base.hpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_NODE_BASE_HPP
#define BOOST_SIGNALS3_NODE_BASE_HPP

#include "compiler_support.hpp"

namespace boost
{
    namespace signals3
    {
        namespace detail
        {
            class node_base
            {
                mutable ::boost::signals3::detail::atomic< int > _unusable;

                static const int _disconnected = INT_MIN;
            public:
                virtual
                ~node_base(void)
                {
                }

                bool
                block(void) const
                {
                    while (true)
                    {
                        int snapshot = _unusable.load();
                        if (snapshot == _disconnected)
                        {
                            return false;
                        }
                        else
                        {
                            int old = snapshot;
                            ++snapshot;
                            if (_unusable.compare_exchange_weak(old, snapshot))
                            {
                                return true;
                            }
                        }
                    }
                    return false;
                }

                bool
                unblock(void) const
                {
                    while (true)
                    {
                        int snapshot = _unusable.load();
                        if (snapshot == _disconnected)
                        {
                            return false;
                        }
                        else
                        {
                            int old = snapshot;
                            --snapshot;
                            if (_unusable.compare_exchange_weak(old, snapshot))
                            {
                                return true;
                            }
                        }
                    }
                    return false;
                }

                bool
                blocked(void) const
                {
                    return _unusable.load() > 0;
                }

                bool
                connected(void) const
                {
                    return _unusable.load() != _disconnected;
                }

                /**
                 * @return true if was not previously marked disconnectd
                 */
                bool
                mark_disconnected(void) const
                {
                    return _unusable.exchange(_disconnected) != _disconnected;
                }

                bool
                usable(void) const
                {
                    int val = _unusable.load();
                    return val != _disconnected && val <= 0;
                    //return _unusable.load() == 0;
                }
            };
        }
    }
}

#endif // BOOST_SIGNALS3_NODE_BASE_HPP
