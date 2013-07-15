/*
 * atomic_extensions.hpp
 *
 * (c) 2013 helloworld922
 *
 *  Created on: Jul 14, 2013
 *      Author: helloworld922
 */

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
                            const T orig = ++snapshot;
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
                            const T orig = --snapshot;
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
