//
// optional_last_value.hpp
//
// near-identical copy of Boost Signals2 optional_last_value.hpp
//
// (c) 2013 helloworld922
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
    template<typename Signature>
    struct optional_last_value;

    template<typename ResultType, typename... Args>
    struct optional_last_value<ResultType(Args...)>
    {
      typedef boost::optional< ResultType > result_type;

      template<typename Iter, typename... U>
      result_type
      operator()(Iter begin, Iter end, U&&... args)
      {
        result_type result;
        while (begin != end)
        {
          std::cout << "combiner call" << std::endl;
          begin(args...);
          ++begin;
        }
        return result;
      }
    };

    template<typename... Args>
    struct optional_last_value< void(Args...) >
    {
      typedef void result_type;

      template<typename Iter, typename... U>
      void
      operator()(Iter begin, Iter end, U&&... args) const
      {
        while (begin != end)
        {
//          std::cout << "combiner call" << std::endl;
          begin(args...);
          ++begin;
        }
      }
    };
  }
}

#endif // BOOST_SIGNALS3_OPTIONAL_LAST_VALUE_HPP
