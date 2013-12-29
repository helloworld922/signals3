//
// signals.hpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_SIGNAL_HPP
#define BOOST_SIGNALS3_SIGNAL_HPP

//#include <boost/signals3/detail/compiler_support.hpp>
#include <boost/signals3/detail/extended_signature.hpp>
//#include <boost/signals3/detail/node_base.hpp>
//#include <boost/signals3/detail/signal_base.hpp>
#include <boost/signals3/slots.hpp>
#include <boost/signals3/connection.hpp>
#include <boost/signals3/optional_last_value.hpp>
#include <map>

namespace boost
{
  namespace signals3
  {
    template<typename Signature, typename Combiner = optional_last_value< typename ::boost::signals3::detail::function<Signature>::result_type >,
             typename Group = int, typename GroupCompare = std::less< Group >,
             typename FunctionType = ::boost::signals3::detail::function< Signature >,
             typename ExtendedFunctionType = typename ::boost::signals3::detail::extended_signature<
             Signature >::type, typename Mutex = ::boost::signals3::detail::mutex>
    class signal;

    namespace detail
    {
      class signal_base;

      class node_base;
    }

    template<typename ResultType, typename ... Args, typename Combiner, typename Group,
             typename GroupCompare, typename FunctionType, typename ExtendedFunctionType,
             typename Mutex>
    class signal< ResultType
      (Args...), Combiner, Group, GroupCompare, FunctionType, ExtendedFunctionType, Mutex > : public ::boost::signals3::detail::signal_base
    {
    public:
      // typedefs
      typedef Mutex mutex_type;
      typedef boost::signals3::detail::lock_guard< mutex_type > lock_guard_type;
      typedef ResultType result_type;
      typedef Combiner combiner_type;
      typedef Group group_type;
      typedef GroupCompare group_compare_type;
      typedef boost::signals3::slot< ResultType
      (Args...), FunctionType > slot_type;
      typedef boost::signals3::slot< ResultType
      (const connection&, Args...), ExtendedFunctionType > extended_slot_type;
      typedef ::boost::signals3::detail::atomic< int > atomic_int_type;
    private:
      // used for unpacking tuple to function call
      template<int...>
      struct seq
      {};
      template<int N, int... S>
      struct gens : gens<N - 1, N - 1, S...>
      {};
      template<int... S>
      struct gens<0, S...>
      {
        typedef seq<S...> type;
      };

      struct t_node_base;

      // private typedefs
      typedef std::multimap<group_type, ::boost::signals3::detail::weak_ptr<t_node_base>, group_compare_type > group_storage_type;

      struct t_node_base : public ::boost::signals3::detail::node_base
      {
        ::boost::signals3::detail::shared_ptr< t_node_base > next;
        ::boost::signals3::detail::weak_ptr< t_node_base > prev;

        enum state
        {
          NONE = 0, EXTENDED = 1<<0, GROUPED = 1<<1
        };

        const state state_;

        t_node_base(state state_ = NONE) : state_(state_)
        {}
        
        virtual ~t_node_base() = default;

        virtual bool
        try_lock(
          ::boost::signals3::detail::forward_list<
          ::boost::signals3::detail::shared_ptr< void > >& list) const = 0;

        virtual typename group_storage_type::iterator* const pos(void)
        {
          return nullptr;
        }

        inline bool grouped(void) const
        {
          return (state_ & state::GROUPED);
        }

        inline bool extended(void) const
        {
          return (state_ & state::EXTENDED);
        }

//        template<typename... U>
//        result_type operator()(::boost::signals3::detail::tuple<U...>& params) const
//        {
//          if(!extended())
//            {
//              return static_cast<const node&>(*this).call_func(typename gens<sizeof...(U)>::type(), params);
//            }
//          else
//            {
//              return static_cast<const extended_node&>(*this).call_func(typename gens<sizeof...(U)>::type(), params);
//            }
//        }
      };

      struct node : public t_node_base
      {
        slot_type callback;
        node(const slot_type& callback) :
          t_node_base(false), callback(callback)
        {
        }

        node(slot_type&& callback) :
          callback(boost::move(callback))
        {
        }

        virtual bool
        try_lock(
          ::boost::signals3::detail::forward_list<
          ::boost::signals3::detail::shared_ptr< void > >& list) const override
        {
          return callback.try_lock(list);
        }

        template<int... S, typename... U>
        inline ResultType call_func(seq<S...>, ::boost::signals3::detail::tuple<U...>& params) const
        {
          return callback.slot_function()(std::forward<U>(std::get<S>(params))...);
        }
      };

      struct extended_node : public t_node_base
      {
        extended_slot_type callback;
        connection conn;

        extended_node(const extended_slot_type& callback) :
          t_node_base(true), callback(callback)
        {
        }

        extended_node(extended_slot_type&& callback) :
          callback(boost::move(callback))
        {
        }

        virtual bool
        try_lock(
          ::boost::signals3::detail::forward_list<
          ::boost::signals3::detail::shared_ptr< void > >& list) const override
        {
          return callback.try_lock(list);
        }

        template<int... S, typename... U>
        inline ResultType call_func(seq<S...>, ::boost::signals3::detail::tuple<U...>& params) const
        {
          return callback.slot_function()(conn, std::forward<U>(std::get<S>(params))...);
        }
      };

      template<typename B>
      struct grouped_node : public B
      {
        typename group_storage_type::iterator iter;

        template<typename SlotType>
        grouped_node(const SlotType& callback) : B(callback)
        {}

        template<typename SlotType>
        grouped_node(SlotType&& callback) : B(std::forward<SlotType>(callback))
        {}

        grouped_node(const grouped_node& rhs) : B(rhs), iter(rhs.iter)
        {
        }

        grouped_node(grouped_node& rhs) : B(rhs), iter(rhs.iter)
        {
        }

        grouped_node(grouped_node&& rhs) : B(boost::move(rhs)), iter(boost::move(rhs.iter))
        {
        }

        typename group_storage_type::iterator * const pos(void)
        {
          return &iter;
        }

        bool grouped(void) const
        {
          return true;
        }
      };

      template<typename... U>
      class iterator;
      //friend class signal::iterator;

      template<typename... U>
      class unsafe_iterator;
      //friend class signal::unsafe_iterator;

      mutex_type _mutex;
      // fields
      ::boost::signals3::detail::shared_ptr< t_node_base > head;
      ::boost::signals3::detail::shared_ptr< t_node_base > tail;
      Combiner combiner;
      ::boost::signals3::detail::shared_ptr< t_node_base > group_head;

      group_storage_type group_storage;

      void
      push_back_impl(::boost::signals3::detail::shared_ptr< t_node_base >&& node)
      {
        lock_guard_type _lock(_mutex);
        if (tail == nullptr)
          {
            // only one node
            tail = node;
            ::boost::signals3::detail::atomic_store(&head, boost::move(node));
          }
        else
          {
            // more than one node
            node->prev = tail;
            ::boost::signals3::detail::atomic_store(&(tail->next), node);
            tail = boost::move(node);
          }
      }

      void
      push_front_impl(::boost::signals3::detail::shared_ptr< t_node_base >&& node)
      {
        lock_guard_type _lock(_mutex);
        if (head == nullptr)
          {
            // only one node
            tail = node;
            group_head = node;
          }
        else
          {
            // more than one node
            if (group_head == nullptr)
              {
                group_head = node;
              }
            node->next = head;
          }
        ::boost::signals3::detail::atomic_store(&head, boost::move(node));
      }

      void
      push_back_impl_unsafe(::boost::signals3::detail::shared_ptr< t_node_base >&& node)
      {
        if (tail == nullptr)
          {
            // only one node
            tail = node;
            head = boost::move(node);
          }
        else
          {
            // more than one node
            node->prev = tail;
            tail->next = node;
            tail = boost::move(node);
          }
      }

      void
      push_front_impl_unsafe(::boost::signals3::detail::shared_ptr< t_node_base >&& node)
      {
        if (head == nullptr)
          {
            // only one node
            tail = node;
            group_head = node;
          }
        else
          {
            // more than one node
            if (group_head == nullptr)
              {
                group_head = node;
              }
            node->next = head;
          }
        head = boost::move(node);
      }

      /**
       * Assumes caller already has a unique lock
       */
      void pop_back_impl(void)
      {
        if (tail != nullptr)
          {
            // actually have a node to remove
            tail->mark_disconnected();
            if(tail->grouped())
              {
                // handle group slots
                group_storage.erase(*(tail->pos()));
              }
            ::boost::signals3::detail::shared_ptr< t_node_base > prev = tail->prev.lock();
            if (prev != nullptr)
              {
                // more than 1 node
                tail->prev.reset();
                if (group_head == tail)
                  {
                    group_head = prev;
                  }
                ::boost::signals3::detail::atomic_store(&(prev->next), ::boost::signals3::detail::shared_ptr< t_node_base >());

                tail = boost::move(prev);
              }
            else
              {
                // only one node
                tail.reset();
                group_head.reset();
                ::boost::signals3::detail::atomic_store(&head, tail);
              }
          }
      }

      /**
       * Assumes caller already has a unique lock
       */
      void pop_front_impl(void)
      {
        if (head != nullptr)
          {
            // actually have a node to remove
            head->mark_disconnected();
            if(head->grouped())
              {
                // handle group slots
                group_storage.erase(*(head->pos()));
              }
            if (head == tail)
              {
                // only have one node
                tail.reset();
                group_head.reset();
                ::boost::signals3::detail::atomic_store(&head, tail);
              }
            else
              {
                // more than one node
                if (group_head == head)
                  {
                    group_head.reset();
                  }
                ::boost::signals3::detail::atomic_store(&head, head->next);
              }
          }
      }

      void disconnect(::boost::signals3::detail::shared_ptr< t_node_base >& node)
      {
        lock_guard_type _lock(_mutex);
        if(node->mark_disconnected())
          {
            if(node == head)
              {
                pop_front_impl();
              }
            else if(node == tail)
              {
                pop_back_impl();
              }
            else
              {
                if(node->grouped())
                  {
                    // handle group slots
                    group_storage.erase(*(node->pos()));
                  }

                ::boost::signals3::detail::shared_ptr< t_node_base > prev = node->prev.lock();

                if(node == group_head)
                  {
                    group_head = prev;
                  }
                ::boost::signals3::detail::atomic_store(&(prev->next), node->next);
                node->next->prev = boost::move(prev);
              }
          }
      }

      virtual void disconnect(::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base >&& n) override
      {
        ::boost::signals3::detail::shared_ptr<t_node_base> node = ::boost::signals3::detail::static_pointer_cast<t_node_base>(boost::move(n));
        disconnect(node);
      }

      virtual void disconnect_unsafe(::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base >&& n) override
      {
        ::boost::signals3::detail::shared_ptr<t_node_base> node = ::boost::signals3::detail::static_pointer_cast<t_node_base>(boost::move(n));
        disconnect_unsafe(node);
      }

      void disconnect_unsafe(::boost::signals3::detail::shared_ptr< t_node_base >& node)
      {
        if(node->mark_disconnected())
          {
            if(node == head)
              {
                pop_front_unsafe();
              }
            else if(node == tail)
              {
                pop_back_unsafe();
              }
            else
              {
                if(node->grouped())
                  {
                    // handle group slots
                    group_storage.erase(*(node->pos()));
                  }

                ::boost::signals3::detail::shared_ptr< t_node_base > prev = node->prev.lock();
                if(node == group_head)
                  {
                    group_head = prev;
                  }
                prev->next = node->next;
                node->next->prev = boost::move(prev);
              }
          }
      }

      template<typename B>
      void insert_impl(const group_type& group, ::boost::signals3::detail::shared_ptr< grouped_node<B> >&& n)
      {
        typename group_storage_type::value_type val(group, n);
        lock_guard_type _lock(_mutex);
        typename group_storage_type::iterator iter = group_storage.insert(boost::move(val));
        n->iter = iter;
        if(iter != group_storage.begin())
          {
            // n is not going to be the new group_head
            typename group_storage_type::iterator prev_iter = iter;
            --prev_iter;
            ::boost::signals3::detail::shared_ptr<t_node_base> prev = prev_iter->second.lock();
            n->prev = prev_iter->second;
            n->next = prev->next;
            if(prev->next == nullptr)
              {
                // push_back case, update tail
                tail = n;
              }
            ::boost::signals3::detail::atomic_store(&(prev->next), ::boost::signals3::detail::static_pointer_cast<t_node_base>(boost::move(n)));
          }
        else
          {
            // n is first grouped node, hook to group_head
            if(group_head != nullptr)
              {
                n->prev = group_head;
                n->next = group_head->next;
                ::boost::signals3::detail::atomic_store(&(group_head->next), ::boost::signals3::detail::static_pointer_cast<t_node_base>(boost::move(n)));
              }
            else
              {
                // push_front case, update head
                if(head != nullptr)
                  {
                    n->next = head;
                  }
                else
                  {
                    // only node
                    tail = n;
                  }
                ::boost::signals3::detail::atomic_store(&head, ::boost::signals3::detail::static_pointer_cast<t_node_base>(boost::move(n)));
              }
          }
      }

      template<typename B>
      void insert_impl_unsafe(const group_type& group, ::boost::signals3::detail::shared_ptr< grouped_node<B> >&& n)
      {
        typename group_storage_type::value_type val(group, n);
        typename group_storage_type::iterator iter = group_storage.insert(boost::move(val));
        n->iter = iter;
        if(iter != group_storage.begin())
          {
            // n is not going to be the new group_head
            typename group_storage_type::iterator prev_iter = iter;
            --prev_iter;
            ::boost::signals3::detail::shared_ptr<t_node_base> prev = prev_iter->second.lock();
            n->prev = prev_iter->second;
            n->next = prev->next;
            if(prev->next == nullptr)
              {
                // push_back case, update tail
                tail = n;
              }
            prev->next = boost::move(n);
          }
        else
          {
            // n is first grouped node, hook to group_head
            if(group_head != nullptr)
              {
                n->prev = group_head;
                n->next = group_head->next;
                group_head->next = boost::move(n);
              }
            else
              {
                // push_front case, update head
                if(head != nullptr)
                  {
                    n->next = head;
                  }
                else
                  {
                    // only node
                    tail = n;
                  }
                head = boost::move(n);
              }
          }
      }

    public:
      signal(void) = default;

      signal(const signal&) = delete;

      // TODO
      signal(signal&& s) = delete;

      connection insert(const group_type& group, const slot_type& callback)
      {
        ::boost::signals3::detail::shared_ptr< grouped_node<node> > n = ::boost::signals3::detail::make_shared< grouped_node<node> >(callback);
        connection conn(this, n);
        insert_impl(group, boost::move(n));
        return conn;
      }

      connection insert(const group_type& group, slot_type&& callback)
      {
        ::boost::signals3::detail::shared_ptr< grouped_node<node> > n = ::boost::signals3::detail::make_shared< grouped_node<node> >(boost::move(callback));
        connection conn(this, n);
        insert_impl(group, boost::move(n));
        return conn;
      }

      connection insert_unsafe(const group_type& group, const slot_type& callback)
      {
        ::boost::signals3::detail::shared_ptr< grouped_node<node> > n = ::boost::signals3::detail::make_shared< grouped_node<node> >(callback);
        connection conn(this, n);
        insert_impl_unsafe(group, boost::move(n));
        return conn;
      }

      connection insert_unsafe(const group_type& group, slot_type&& callback)
      {
        ::boost::signals3::detail::shared_ptr< grouped_node<node> > n = ::boost::signals3::detail::make_shared< grouped_node<node> >(boost::move(callback));
        connection conn(this, n);
        insert_impl_unsafe(group, boost::move(n));
        return conn;
      }

      connection insert_extended(const group_type& group, const extended_slot_type& callback)
      {
        ::boost::signals3::detail::shared_ptr< grouped_node<extended_node> > n = ::boost::signals3::detail::make_shared< grouped_node<extended_node> >(callback);
        connection conn(this, n);
        insert_impl(group, boost::move(n));
        return conn;
      }

      connection insert_extended(const group_type& group, extended_slot_type&& callback)
      {
        ::boost::signals3::detail::shared_ptr< grouped_node<extended_node> > n = ::boost::signals3::detail::make_shared< grouped_node<extended_node> >(boost::move(callback));
        connection conn(this, n);
        insert_impl(group, boost::move(n));
        return conn;
      }

      connection insert_extended_unsafe(const group_type& group, const extended_slot_type& callback)
      {
        ::boost::signals3::detail::shared_ptr< grouped_node<extended_node> > n = ::boost::signals3::detail::make_shared< grouped_node<extended_node> >(callback);
        connection conn(this, n);
        insert_impl_unsafe(group, boost::move(n));
        return conn;
      }

      connection insert_extended_unsafe(const group_type& group, extended_slot_type&& callback)
      {
        ::boost::signals3::detail::shared_ptr< grouped_node<extended_node> > n = ::boost::signals3::detail::make_shared< grouped_node<extended_node> >(boost::move(callback));
        connection conn(this, n);
        insert_impl_unsafe(group, boost::move(n));
        return conn;
      }

      connection
      push_back(const slot_type& callback)
      {
        ::boost::signals3::detail::shared_ptr < node > n = ::boost::signals3::detail::make_shared<node>(callback);
        ::boost::signals3::connection conn(this, n);
        push_back_impl(boost::move(n));
        return conn;
      }

      connection
      push_back(slot_type&& callback)
      {
        ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(boost::move(callback));
        ::boost::signals3::connection conn(this, n);
        push_back_impl(boost::move(n));
        return conn;
      }

      connection
      push_back_unsafe(const slot_type& callback)
      {
        ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(callback);
        ::boost::signals3::connection conn(this, n);
        push_back_impl_unsafe(n);
        return conn;
      }

      connection
      push_back_unsafe(slot_type&& callback)
      {
        ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(boost::move(callback));
        ::boost::signals3::connection conn(this, n);
        push_back_impl_unsafe(boost::move(n));
        return conn;
      }

      connection
      push_back_extended(const extended_slot_type& callback)
      {
        ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(callback);
        ::boost::signals3::connection conn(this, n);
        n->conn = conn;
        push_back_impl(boost::move(n));
        return conn;
      }

      connection
      push_back_extended(extended_slot_type&& callback)
      {
        ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(boost::move(callback));
        ::boost::signals3::connection conn(this, n);
        n->conn = conn;
        push_back_impl(boost::move(n));
        return conn;
      }

      connection
      push_back_extended_unsafe(const extended_slot_type& callback)
      {
        ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(callback);
        ::boost::signals3::connection conn(this, n);
        n->conn = conn;
        push_back_impl_unsafe(boost::move(n));
        return conn;
      }

      connection
      push_back_extended_unsafe(extended_slot_type&& callback)
      {
        ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(boost::move(callback));
        ::boost::signals3::connection conn(this, n);
        n->conn = conn;
        push_back_impl_unsafe(boost::move(n));
        return conn;
      }

      connection
      push_front(const slot_type& callback)
      {
        ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(callback);
        ::boost::signals3::connection conn(this, n);
        push_front_impl(boost::move(n));
        return conn;
      }

      connection
      push_front(slot_type&& callback)
      {
        ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(boost::move(callback));
        ::boost::signals3::connection conn(this, n);
        push_front_impl(boost::move(n));
        return conn;
      }

      connection
      push_front_unsafe(const slot_type& callback)
      {
        ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(callback);
        ::boost::signals3::connection conn(this, n);
        push_front_impl_unsafe(boost::move(n));
        return conn;
      }

      connection
      push_front_unsafe(slot_type&& callback)
      {
        ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(boost::move(callback));
        ::boost::signals3::connection conn(this, n);
        push_front_impl_unsafe(boost::move(n));
        return conn;
      }

      connection
      push_front_extended(const extended_slot_type& callback)
      {
        ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(callback);
        ::boost::signals3::connection conn(this, n);
        n->conn = conn;
        push_front_impl(boost::move(n));
        return conn;
      }

      connection
      push_front_extended(extended_slot_type&& callback)
      {
        ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(boost::move(callback));
        ::boost::signals3::connection conn(this, n);
        n->conn = conn;
        push_front_impl(boost::move(n));
        return conn;
      }

      connection
      push_front_extended_unsafe(const extended_slot_type& callback)
      {
        ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(callback);
        ::boost::signals3::connection conn(this, n);
        n->conn = conn;
        push_front_impl_unsafe(boost::move(n));
        return conn;
      }

      connection
      push_front_extended_unsafe(extended_slot_type&& callback)
      {
        ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(boost::move(callback));
        ::boost::signals3::connection conn(this, n);
        n->conn = conn;
        push_front_impl_unsafe(boost::move(n));
        return conn;
      }

      void
      pop_back(void)
      {
        lock_guard_type _lock(_mutex);
        pop_back_impl();
      }

      void
      pop_front(void)
      {
        lock_guard_type _lock(_mutex);
        pop_front_impl();
      }

      void
      pop_back_unsafe(void)
      {
        if (tail != nullptr)
          {
            // actually have a node to remove
            tail->mark_disconnected();
            if(tail->grouped())
              {
                // handle group slots
                group_storage.erase(*(tail->pos()));
              }
            ::boost::signals3::detail::shared_ptr< t_node_base > prev = tail->prev.lock();
            if (prev != nullptr)
              {
                // more than 1 node
                tail->prev.reset();
                if (group_head == tail)
                  {
                    group_head = prev;
                  }
                prev->next.reset();

                tail = boost::move(prev);
              }
            else
              {
                // only one node
                tail.reset();
                group_head.reset();
                head.reset();
              }
          }
      }

      void
      pop_front_unsafe(void)
      {
        if (head != nullptr)
          {
            // actually have a node to remove
            head->mark_disconnected();
            if(head->grouped())
              {
                // handle group slots
                group_storage.erase(*(head->pos()));
              }
            if (head == tail)
              {
                // only have one node
                tail.reset();
                group_head.reset();
                head.reset();
              }
            else
              {
                // more than one node
                if (group_head == head)
                  {
                    group_head.reset();
                  }
                head = head->next;
              }
          }
      }

      void clear(void)
      {
        lock_guard_type _lock(_mutex);
        if(head != nullptr)
          {
            ::boost::signals3::detail::shared_ptr<t_node_base> iter = head;
            while(iter != nullptr)
              {
                iter->mark_disconnected();
                iter = iter->next;
              }
            tail.reset();
            group_head.reset();
            ::boost::signals3::detail::atomic_store(&head, tail);
            group_storage.clear();
          }
      }

      void clear_unsafe(void)
      {
        if(head != nullptr)
          {
            ::boost::signals3::detail::shared_ptr<t_node_base> iter = head;
            while(iter != nullptr)
              {
                iter->mark_disconnected();
                iter = iter->next;
              }
            tail.reset();
            group_head.reset();
            head.reset();
            group_storage.clear();
          }
      }

      void erase(const group_type& group)
      {
        lock_guard_type _lock(_mutex);
        std::pair<typename group_storage_type::iterator, typename group_storage_type::iterator> bounds = group_storage.equal_range(group);
        if(bounds.first != group_storage.end())
          {
            // at least one slot to disconnect
            ::boost::signals3::detail::shared_ptr<t_node_base> iter = bounds.first->second.lock();
            // assume we're removing all nodes bounds.first to end
            ::boost::signals3::detail::shared_ptr<t_node_base> end_pos;
            if(bounds.second != group_storage.end())
              {
                // segment being removed is somewhere in the middle (not removing tail)
                end_pos = bounds.second->second.lock();
              }
            else
              {
                tail = iter->prev.lock();
              }
            if(iter == head)
              {
                // move head to end_pos
                ::boost::signals3::detail::atomic_store(&head, end_pos);
              }
            do
              {
                iter->mark_disconnected();
                ::boost::signals3::detail::shared_ptr<t_node_base> next = iter->next;
                ::boost::signals3::detail::atomic_store(&(iter->next), end_pos);

                iter = boost::move(next);
                ++bounds.first;
              }
            while(bounds.first != bounds.second);
            group_storage.erase(group);
          }
      }

      void erase_unsafe(const group_type& group)
      {
        std::pair<typename group_storage_type::iterator, typename group_storage_type::iterator> bounds = group_storage.equal_range(group);
        if(bounds.first != group_storage.end())
          {
            // at least one slot to disconnect
            ::boost::signals3::detail::shared_ptr<t_node_base> iter = bounds.first->second.lock();
            // assume we're removing all nodes bounds.first to end
            ::boost::signals3::detail::shared_ptr<t_node_base> end_pos;
            if(bounds.second != group_storage.end())
              {
                // segment being removed is somewhere in the middle (not removing tail)
                end_pos = bounds.second->second.lock();
              }
            else
              {
                tail = iter->prev.lock();
              }
            if(iter == head)
              {
                // move head to end_pos
                head = end_pos;
              }
            do
              {
                iter->mark_disconnected();
                ::boost::signals3::detail::shared_ptr<t_node_base> next = iter->next;
                iter->next = end_pos;

                iter = boost::move(next);
                ++bounds.first;
              }
            while(bounds.first != bounds.second);
            group_storage.erase(group);
          }
      }

      template<typename... U>
      typename Combiner::result_type
      emit(U&&... args)
      {
        ::boost::signals3::detail::forward_list
        < ::boost::signals3::detail::shared_ptr< void > > tracking_list;
        // TODO: use a lock_guard or some other RAII locking
//        _mutex.lock();
        auto begin_ptr = ::boost::signals3::detail::atomic_load(&head);
//        auto begin_ptr = head;
//        _mutex.unlock();
        while (begin_ptr != nullptr)
          {
            if (!begin_ptr->usable())
              {
//                lock_guard_type iter_lock(begin_ptr->_mutex);
//                begin_ptr = begin_ptr->next;
                begin_ptr = ::boost::signals3::detail::atomic_load(&(begin_ptr->next));
              }
            else if (!begin_ptr->try_lock(tracking_list))
              {
                // automatic disconnect
                tracking_list.clear();
                disconnect(begin_ptr);
//                lock_guard_type iter_lock(begin_ptr->_mutex);
//                begin_ptr = begin_ptr->next;
                begin_ptr = ::boost::signals3::detail::atomic_load(&(begin_ptr->next));
              }
            else
              {
                break;
              }
          }
        std::tuple<U...> params(std::forward<U>(args)...);
        iterator<U...> begin(boost::move(begin_ptr), tracking_list, params, *this);
        iterator<U...> end(nullptr, tracking_list, params, *this);
        return combiner(boost::move(begin), boost::move(end));
      }

      template<typename... U>
      typename Combiner::result_type
      emit_unsafe(U&&... args)
      {
        ::boost::signals3::detail::forward_list
        < ::boost::signals3::detail::shared_ptr< void > > tracking_list;
        ::boost::signals3::detail::shared_ptr< t_node_base > begin_ptr = head;
        while (begin_ptr != nullptr)
          {
            if (!begin_ptr->usable())
              {
                begin_ptr = begin_ptr->next;
              }
            else if (!begin_ptr->try_lock(tracking_list))
              {
                // automatic disconnect
                tracking_list.clear();
                disconnect_unsafe(begin_ptr);
                begin_ptr = begin_ptr->next;
              }
            else
              {
                break;
              }
          }
        std::tuple<U...> params(std::forward<U>(args)...);
        unsafe_iterator<U...> begin(boost::move(begin_ptr), tracking_list, params, *this);
        unsafe_iterator<U...> end(nullptr, tracking_list, params, *this);
        return combiner(boost::move(begin), boost::move(end));
      }
    };

    template<typename ResultType, typename ... Args, typename Combiner, typename Group,
             typename GroupCompare, typename FunctionType, typename ExtendedFunctionType,
             typename Mutex>
             template<typename... U>
    class signal< ResultType
      (Args...), Combiner, Group, GroupCompare, FunctionType, ExtendedFunctionType, Mutex >::iterator
    {
      ::boost::signals3::detail::shared_ptr< t_node_base > curr;
      ::boost::signals3::detail::forward_list<
      ::boost::signals3::detail::shared_ptr< void > >& tracking;
      std::tuple<U...>& params;
      signal< ResultType
      (Args...), Combiner, Group, GroupCompare, FunctionType, ExtendedFunctionType >& sig;

//				typedef boost::signals3::detail::lock_guard< mutex_type > lock_guard_type;

    public:
      template<typename... V>
      iterator(::boost::signals3::detail::shared_ptr< t_node_base >&& start_node,
               ::boost::signals3::detail::forward_list<
               ::boost::signals3::detail::shared_ptr< void > >& tracking,
               std::tuple<V...>& params,
               signal< ResultType
               (Args...), Combiner, Group, GroupCompare, FunctionType, ExtendedFunctionType >& sig) :
        curr(boost::move(start_node)), tracking(tracking), params(params), sig(sig)
      {
      }

      iterator(iterator&& rhs) :
        curr(boost::move(rhs.curr)), tracking(rhs.tracking), params(rhs.params), sig(
          rhs.sig)
      {
      }

      ResultType
      operator*() const
      {
        auto& tmp = *curr;
        if(!tmp.extended())
            {
              return static_cast<const node&>(tmp).call_func(typename gens<sizeof...(U)>::type(), params);
            }
          else
            {
              return static_cast<const extended_node&>(tmp).call_func(typename gens<sizeof...(U)>::type(), params);
            }
//        return (*curr)(params);
      }

      iterator&
      operator++(void)
      {
        // release any locks we might have
        tracking.clear();
        if (curr != nullptr)
          {
            while (true)
              {
                {
//                  lock_guard_type lock(curr->_mutex);
//                  curr = curr->next;
                  curr = ::boost::signals3::detail::atomic_load(&(curr->next));
                }
                if (curr == nullptr)
                  {
                    return *this;
                  }
                else if (curr->usable())
                  {
                    if (curr->try_lock(tracking))
                      {
                        return *this;
                      }
                    else
                      {
                        // automatic disconnect
                        sig.disconnect(curr);
                        tracking.clear();
                      }
                  }
              }
          }
        return *this;
      }

      bool
      operator ==(const iterator& rhs) const
      {
        return curr == rhs.curr;
      }

      bool
      operator !=(const iterator& rhs) const
      {
        return curr != rhs.curr;
      }
    };

    template<typename ResultType, typename ... Args, typename Combiner, typename Group,
             typename GroupCompare, typename FunctionType, typename ExtendedFunctionType,
             typename Mutex>
             template<typename... U>
    class signal< ResultType
      (Args...), Combiner, Group, GroupCompare, FunctionType, ExtendedFunctionType, Mutex >::unsafe_iterator
    {
      ::boost::signals3::detail::shared_ptr< t_node_base > curr;
      ::boost::signals3::detail::forward_list<
      ::boost::signals3::detail::shared_ptr< void > >& tracking;
      std::tuple<U...>& params;
      signal< ResultType
      (Args...), Combiner, Group, GroupCompare, FunctionType, ExtendedFunctionType >& sig;

    public:
      template<typename... V>
      unsafe_iterator(::boost::signals3::detail::shared_ptr< t_node_base >&& start_node,
                      ::boost::signals3::detail::forward_list<
                      ::boost::signals3::detail::shared_ptr< void > >& tracking,
                      std::tuple<V...>& params,
                      signal< ResultType
                      (Args...), Combiner, Group, GroupCompare, FunctionType, ExtendedFunctionType >&sig) :
        curr(boost::move(start_node)), tracking(tracking), params(params), sig(sig)
      {
      }

      unsafe_iterator(unsafe_iterator&& rhs) :
        curr(boost::move(rhs.curr)), tracking(rhs.tracking), params(rhs.params), sig(rhs.sig)
      {
      }

//      template<typename... U>
      ResultType
      operator*(void) const
      {
//        return (*curr)(params);
        auto& tmp = *curr;
        if(!tmp.extended())
            {
              return static_cast<const node&>(tmp).call_func(typename gens<sizeof...(U)>::type(), params);
            }
          else
            {
              return static_cast<const extended_node&>(tmp).call_func(typename gens<sizeof...(U)>::type(), params);
            }
      }

      unsafe_iterator&
      operator++(void)
      {
        // release any locks we might have
        tracking.clear();
        if (curr != nullptr)
          {
            while (true)
              {
                curr = curr->next;
                if (curr == nullptr)
                  {
                    return *this;
                  }
                if (curr->usable())
                  {
                    if (curr->try_lock(tracking))
                      {
                        return *this;
                      }
                    else
                      {
                        // automatic disconnect
                        tracking.clear();
                        sig.disconnect_unsafe(curr);
                      }
                  }
              }
          }
        return *this;
      }

      bool
      operator ==(const unsafe_iterator& rhs) const
      {
        return curr == rhs.curr;
      }

      bool
      operator !=(const unsafe_iterator& rhs) const
      {
        return curr != rhs.curr;
      }
    };
  }
}

#endif // BOOST_SIGNALS3_SIGNAL_HPP
