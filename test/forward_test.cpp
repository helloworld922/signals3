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
          bool moved;
          test_class(void) : moved(false)
          {
            std::cout << "construct" << std::endl;
          }
          
          test_class(const test_class& c) : moved(c.moved)
          {
            if(c.moved)
            {
              std::cout << "problem! moving a moved object" << std::endl;
            }
            std::cout << "copy" << std::endl;
          }
          
          test_class(test_class&& c) : moved(c.moved)
          {
            if(c.moved)
            {
              std::cout << "problem! moving a moved object" << std::endl;
            }
            c.moved = true;
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
        
        void handler2(test_class&&)
        {
          std::cout << "handler(const test_class&)" << std::endl;
        }
        
        void handler3(int* arg)
        {
          std::cout << "handler(int*)" << std::endl;
          std:: cout << arg << std::endl;
        }
        
        void handler4(std::vector<int>& vals)
        {
          std::cout << "handler(vector<int>)" << std::endl;
          std::cout << vals.size() << std::endl;
        }

        void test(void)
        {
//          boost::signals3::signal<void(test_class&&)> sig1;
          
//          std::vector<int> vals = {1, 2, 3};
//          sig1.push_back(handler2);
//          sig1.push_back(handler2);
//          sig1.push_back(handler2);
//          sig1.push_back(handler2);
//          sig1.push_back(handler3);
//          test_class a;
//          sig1.emit_unsafe(test_class());
//          std::cout << vals.size() << std::endl;
          
//          test_class &b = a;
//          test_class &&c = boost::move(a);
//          sig1.emit(a);
//          sig1.emit(b);
//          sig1.emit(std::forward<test_class>(c));
        }
      }
    }
  }
}
