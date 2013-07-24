/*
 * signal_base.hpp
 *
 * (c) 2013 helloworld922
 *
 *  Created on: Jul 24, 2013
 *      Author: helloworld922
 */

#ifndef BOOST_SIGNALS3_SIGNAL_BASE_HPP
#define BOOST_SIGNALS3_SIGNAL_BASE_HPP

#include "compiler_support.hpp"
#include "node_base.hpp"

namespace boost
{
    namespace signals3
    {
        class connection;

        template<typename Signature, typename Combiner, typename Group, typename GroupCompare,
                typename FunctionType, typename ExtendedFunctionType, typename Mutex>
            class signal;

        namespace detail
        {
            class signal_base
            {
                friend class ::boost::signals3::connection;

                template<typename Signature, typename Combiner, typename Group,
                        typename GroupCompare, typename FunctionType, typename ExtendedFunctionType,
                        typename Mutex>
                    friend class ::boost::signals3::signal;

                virtual void
                disconnect(::boost::signals3::detail::shared_ptr< node_base >&& node) = 0;

            public:
                virtual
                ~signal_base(void)
                {
                }
            };
        }
    }
}

#endif // BOOST_SIGNALS3_SIGNAL_BASE_HPP
