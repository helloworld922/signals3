#include "test/forward_test.hpp"
#include <boost/signals3/signals3.hpp>
#include <iostream>

namespace boost
{
  namespace signals3
  {
    namespace test
    {
      namespace forward
      {
        class test_class
        {
        public:
          test_class(void)
          {
            std::cout << "construct" << std::endl;
          }
          
          test_class(const test_class&)
          {
            std::cout << "copy" << std::endl;
          }
          
          test_class(test_class&&)
          {
            std::cout << "move" << std::endl;
          }
          
          test_class& operator = (const test_class&)
          {
            std::cout << "copy assign" << std::endl;
            return *this;
          }
          
          test_class& operator = (test_class&&)
          {
            std::cout << "move assign" << std::endl;
            return *this;
          }
        };
        
        void handler1(test_class)
        {
          std::cout << "handler(test_class)" << std::endl;
        }
        
        void handler2(const test_class&)
        {
          std::cout << "handler(const test_class&)" << std::endl;
        }
        
        void handler3(test_class&&)
        {
          std::cout << "handler(test_class&&)" << std::endl;
        }

        void test(void)
        {
          boost::signals3::signal<void(test_class&)> sig1;
          
          sig1.push_back(handler1);
//          sig1.push_back(handler1);
          sig1.push_back(handler2);
//          sig1.push_back(handler3);
          test_class a;
          
          test_class &b = a;
//          test_class &&c = boost::move(a);
//          sig1.emit(a);
          sig1.emit(b);
//          sig1.emit(std::forward<test_class>(c));
        }
      }
    }
  }
}
