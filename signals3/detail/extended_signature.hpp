//
// extended_signature.hpp
//
// Near-identical to boost extended_signature.hpp
//
// (c) 2013 helloworld922
//
//  Created on: Jul 14, 2013
//      Author: helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_EXTENDED_SIGNATURE_HPP
#define BOOST_SIGNALS3_EXTENDED_SIGNATURE_HPP

#include <functional>

namespace boost
{
    namespace signals3
    {
        namespace detail
        {
            template<typename Signature>
                struct extended_signature;

            template<typename ResultType, typename ... Args>
                struct extended_signature<ResultType
                (Args...)>
                {
                    typedef std::function<ResultType
                    (const connection&, Args...)> type;
                };
        }
    }
}

#endif /* EXTENDED_SIGNATURE_HPP_ */
