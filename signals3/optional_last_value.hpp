//
// optional_last_value.hpp
//
// near-identical copy of Boost Signals2 optional_last_value.hpp
//
// (c) 2013 helloworld922
//
//  Created on: Jul 13, 2013
//      Author: helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_OPTIONAL_LAST_VALUE_HPP
#define BOOST_SIGNALS3_OPTIONAL_LAST_VALUE_HPP

#include <boost/optional.hpp>

namespace boost
{
    namespace signals3
    {
        template<typename ResultType>
        struct optional_last_value
        {
            typedef boost::optional<ResultType> result_type;

            template<typename Iter>
            result_type operator()(Iter iter)
            {
                result_type result;
                while(iter.is_end())
                {
                    result = *iter;
                    ++iter;
                }
                return result;
            }
        };

        template<>
        struct optional_last_value<void>
        {
            typedef void result_type;

            template<typename Iter>
            void operator()(Iter iter) const
            {
                while(!iter.is_end())
                {
                    *iter;
                    ++iter;
                }
            }
        };
    }
}


#endif /* OPTIONAL_LAST_VALUE_HPP_ */
