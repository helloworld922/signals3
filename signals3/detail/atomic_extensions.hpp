//
// atomic_extensions.hpp
//
// (c) 2013 helloworld922
//
//  Created on: Jul 14, 2013
//      Author: helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_ATOMIC_EXTENSIONS_HPP
#define BOOST_SIGNALS3_ATOMIC_EXTENSIONS_HPP

namespace boost
{
    namespace signals3
    {
        namespace detail
        {
            template<typename T, typename AtomicType>
                inline bool
                compare_and_inc_not_equal(AtomicType& atom, const T& val)
                {
                    while (true)
                    {
                        T snapshot = atom.load();
                        if (val == snapshot)
                        {
                            return false;
                        }
                        else
                        {
                            T orig = ++snapshot;
                            if (atom.compare_exchange_weak(orig, snapshot))
                            {
                                return true;
                            }
                        }
                    }
                }

            template<typename T, typename AtomicType>
                inline bool
                compare_and_dec_not_equal(AtomicType& atom, const T& val)
                {
                    while (true)
                    {
                        T snapshot = atom.load();
                        if (val == snapshot)
                        {
                            return false;
                        }
                        else
                        {
                            T orig = --snapshot;
                            if (atom.compare_exchange_weak(orig, snapshot))
                            {
                                return true;
                            }
                        }
                    }
                }
        }
    }
}

#endif // BOOST_SIGNALS3_ATOMIC_EXTENSIONS_HPP
