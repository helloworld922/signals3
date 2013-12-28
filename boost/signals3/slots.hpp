//
// slots.hpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_SLOTS_HPP
#define BOOST_SIGNALS3_SLOTS_HPP

#include <boost/signals3/detail/compiler_support.hpp>

//#include <algorithm>

namespace boost
{
  namespace signals3
  {
    template<typename Signature>
    class slot_base;

    template<typename ResultType, typename ... Args>
    class slot_base< ResultType
      (Args...) >
    {
    public:
      typedef ::boost::signals3::detail::weak_ptr< void > weak_ptr_type;
    private:
      typedef ::boost::signals3::detail::forward_list< weak_ptr_type > _track_list;
      _track_list _tracking;
    public:
      typedef ResultType result_type;

      slot_base(void)
      {
      }

      slot_base(const slot_base& rhs) :
        _tracking(rhs._tracking)
      {
      }

      slot_base(slot_base&& rhs) :
        _tracking(boost::move(rhs._tracking))
      {
      }

      slot_base&
      track(const weak_ptr_type& obj)
      {
#if defined(BOOST_SIGNALS3_USE_FORWARD_LIST)
        _tracking.push_front(obj);
#else
        _tracking.push_back(obj);
#endif
        return *this;
      }

      slot_base&
      track(weak_ptr_type&& obj)
      {
#if defined(BOOST_SIGNALS3_USE_FORWARD_LIST)
        _tracking.push_front(std::move(obj));
#else
        _tracking.push_back(std::move(obj));
#endif
        return *this;
      }

      _track_list&
      tracking(void)
      {
        return _tracking;
      }
      const _track_list&
      tracking(void) const
      {
        return _tracking;
      }

      bool
      expired(void) const
      {
        for(const auto& iter : _tracking)
          {
            if (iter.expired())
              {
                return true;
              }
          }
        return false;
      }

      /**
       * Thread safe call to try_lock
       */
      template<typename ForwardList = ::boost::signals3::detail::forward_list<
      typename ::boost::signals3::detail::shared_ptr< void > > >
      bool
      try_lock(ForwardList& list) const
      {
        for(const auto& i : _tracking)
          {
            auto item = i.lock();
            if (item == nullptr)
              {
                return false;
              }
#if defined(BOOST_SIGNALS3_USE_FORWARD_LIST)
            list.push_front(boost::move(item));
#else
            list.push_back(boost::move(item));
#endif
          }
        return true;
      }

      virtual
      ~slot_base(void)
      {
      }
    };

    template<typename Signature, typename SlotFunction = ::boost::signals3::detail::function<
    Signature > >
    class slot;

    template<typename ResultType, typename ... Args, typename SlotFunction>
    class slot< ResultType
      (Args...), SlotFunction > : public slot_base< ResultType
      (Args...) >
    {
      SlotFunction callback;
    public:
      typedef SlotFunction function_type;

      template<typename Callback>
      slot(Callback&& callback) :
        callback(std::forward< Callback >(callback))
      {
      }

      template<typename Callback>
      slot(const Callback& callback) :
        callback(callback)
      {
      }

      slot(slot< ResultType
           (Args...), SlotFunction >& rhs) :
        slot_base< ResultType
        (Args...) >(rhs), callback(rhs.callback)
      {
      }

      slot(const slot< ResultType
           (Args...), SlotFunction >& rhs) :
        slot_base< ResultType
        (Args...) >(rhs), callback(rhs.callback)
      {
      }

      slot(slot< ResultType
           (Args...), SlotFunction > && rhs) :
        slot_base< ResultType
        (Args...) >(boost::move(rhs)), callback(boost::move(rhs.callback))
      {
      }

      virtual
      ~slot(void)
      {
      }

      SlotFunction&
      slot_function(void)
      {
        return callback;
      }

      const SlotFunction&
      slot_function(void) const
      {
        return callback;
      }

      template<typename... U>
      ResultType
      operator()(U&&... args) const
      {
//        std::cout << "slot call" << std::endl;
        // TODO: conform better to Signals2
        return callback(std::forward<U>(args)...);
      }
    };
  }
}

#endif // BOOST_SIGNALS3_SLOTS_HPP
