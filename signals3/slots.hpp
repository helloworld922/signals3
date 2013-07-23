//
// slots.hpp
//
// (c) 2013 helloworld922
//
//  Created on: Jul 14, 2013
//      Author: helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_SLOTS_HPP
#define BOOST_SIGNALS3_SLOTS_HPP

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
                typedef track_list< boost::weak_ptr< void > > _track_list;
                _track_list _tracking;

                static const int _disconnected = INT_MIN;
            public:
                typedef ResultType result_type;
                typedef boost::weak_ptr< void > weak_ptr_type;

                slot_base&
                track(const weak_ptr_type& obj)
                {
                    _tracking.push_front(obj);
                    return *this;
                }

                slot_base&
                track(weak_ptr_type&& obj)
                {
                    _tracking.push_front(std::move(obj));
                    return *this;
                }

                track_list< boost::weak_ptr< void > >&
                tracking(void)
                {
                    return _tracking;
                }
                const track_list< boost::weak_ptr< void > >&
                tracking(void) const
                {
                    return _tracking;
                }

                bool
                expired(void) const
                {
                    return _tracking.expired();
                }

                template<typename ForwardList = std::forward_list<
                        typename _track_list::shared_ptr_type > >
                    bool
                    try_lock(ForwardList& list) const
                    {
                        return _tracking.try_lock(list);
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
                    slot(const Callback& callback) :
                            callback(callback)
                    {
                    }

                template<typename Callback>
                    slot(Callback&& callback) :
                            callback(boost::move(callback))
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
            };
    }
}

#endif // BOOST_SIGNALS3_SLOTS_HPP