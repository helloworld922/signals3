//
// extended_signature.hpp
//
// Near-identical to boost extended_signature.hpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_EXTENDED_SIGNATURE_HPP
#define BOOST_SIGNALS3_EXTENDED_SIGNATURE_HPP

#include "compiler_support.hpp"

namespace boost
{
    namespace signals3
    {
        class connection;

        namespace detail
        {
            template<typename Signature>
                struct extended_signature;

            template<typename ResultType, typename ... Args>
                struct extended_signature<ResultType
                (Args...)>
                {
                    typedef ::boost::signals3::detail::function<ResultType
                    (const connection&, Args...)> type;
                };
        }
    }
}

#endif // BOOST_SIGNALS3_EXTENDED_SIGNATURE_HPP
