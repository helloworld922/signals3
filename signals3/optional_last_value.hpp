/*
 * optional_last_value.hpp
 *
 * (c) 2013 helloworld922
 *
 *  Created on: Jul 13, 2013
 *      Author: helloworld922
 */

#ifndef BOOST_SIGNALS3_OPTIONAL_LAST_VALUE_HPP
#define BOOST_SIGNALS3_OPTIONAL_LAST_VALUE_HPP

namespace boost
{
    namespace signals3
    {
        template<typename ResultType>
        struct optional_last_value
        {
            typedef ResultType result_type;

            template<typename Iter>
            ResultType operator()(Iter begin, Iter end)
            {
                ResultType result;
                while(begin != end)
                {
                    result = *begin;
                    ++begin;
                }
                return result;
            }
        };

        template<>
        struct optional_last_value<void>
        {
            typedef void result_type;

            template<typename Iter>
            void operator()(Iter begin, Iter end)
            {
                while(begin != end)
                {
                    *begin;
                    ++begin;
                }
            }
        };
    }
}


#endif /* OPTIONAL_LAST_VALUE_HPP_ */
