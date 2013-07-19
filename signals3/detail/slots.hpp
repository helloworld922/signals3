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

#include "atomic_extensions.hpp"
#include "extended_signature.hpp"
#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>
#include <atomic>
#include <boost/atomic.hpp>

namespace boost
{
    namespace signals3
    {
        // =================================================
        // pre-declarations
        // =================================================
        template<typename Signature, typename Combiner, typename Group, typename GroupCompare,
                typename SlotFunction, typename ExtendedSlotFunction, typename AtomicInt,
                typename SharedMutex>
            class signal;

        namespace detail
        {
            class slot_wrapper;

            template<typename Signature, typename B = slot_wrapper>
                struct callable;

            template<typename Group = int, typename B = slot_wrapper>
                class groupable;

            template<typename AtomicInt = std::atomic< int >, typename B = slot_wrapper>
                class slot_base;

            template<typename Signature, typename FunctionType = std::function< Signature >,
                    typename AtomicInt = std::atomic< int >, typename B = callable< Signature,
                            slot_base< AtomicInt > > >
                class slot;

            template<typename Signature, typename FunctionType = std::function< Signature >,
                    typename Group = int, typename AtomicInt = std::atomic< int >,
                    typename B = slot< Signature, FunctionType, AtomicInt,
                            groupable< Group, callable< Signature, slot_base< AtomicInt > > > > >
                class grouped_slot;

            template<typename Signature,
                    typename ExtendedFunctionType = typename ::boost::signals3::detail::extended_signature<
                            Signature >::type, typename AtomicInt = std::atomic< int >,
                    typename B = callable< Signature, slot_base< AtomicInt > > >
                class extended_slot;

            template<typename Signature,
                    typename ExtendedFunctionType = typename ::boost::signals3::detail::extended_signature<
                            Signature >::type, typename Group = int,
                    typename AtomicInt = std::atomic< int >, typename B = extended_slot< Signature,
                            ExtendedFunctionType, AtomicInt,
                            groupable< Group, callable< Signature, slot_base< AtomicInt > > > > >
                class grouped_extended_slot;
        }

        // =================================================
        // implementations
        // =================================================

        namespace detail
        {
            /**
             * Used for type erasures, signal linked-list node.
             */
            class slot_wrapper
            {
                // unfortunately, there's no partial template specialization allowed in friend declarations.
                template<typename Signature, typename Combiner, typename Group,
                        typename GroupCompare, typename SlotFunction, typename ExtendedSlotFunction,
                        typename AtomicInt, typename SharedMutex>
                    friend class signal;

            public:
                // TODO: should be private, for some reason something isn't compiling nicely
                boost::shared_ptr< slot_wrapper > next;
                boost::weak_ptr< slot_wrapper > prev;

                virtual
                ~slot_wrapper(void)
                {
                }

                /**
                 * increases the block count.
                 * @return: true if was able to increase count.
                 */
                virtual bool
                block(void) const = 0;
                /**
                 * decrease the block count.
                 * @return: true if was able to decrease count.
                 */
                virtual bool
                unblock(void) const = 0;

                /**
                 * Marks this node as being disconnected.
                 * @return: true if slot was not previously marked disconnected
                 */
                virtual bool
                mark_disconnected(void) = 0;

                /**
                 * The slot is blocked if the block count is greater than 0.
                 * @return: true if the slot is blocked.
                 */
                virtual bool
                blocked(void) const = 0;
                /**
                 * @return: true if the slot is connected
                 */
                virtual bool
                connected(void) const = 0;
                /**
                 * @return: true if the slot is usable.
                 */
                virtual bool
                usable(void) const = 0;

                /**
                 * @return: true if it is safe to cast this to a callable.
                 */
                virtual bool
                valid(void) const = 0;

                /**
                 * @return true if it is safe to cast this to a groupable.
                 */
                virtual bool
                grouped(void) const = 0;
            };

            template<typename ResultType, typename ... Args, typename B>
                struct callable< ResultType
                (Args...), B > : public B
                {
                public:
                    typedef ResultType result_type;

                    virtual
                    ~callable(void)
                    {
                    }

                    virtual ResultType
                    operator()(Args ... args) const = 0;
                };

            template<typename Group, typename B>
                class groupable : public B
                {
                public:
                    typedef Group group_type;

                    const Group gid;

                    groupable(const Group gid) :
                            gid(gid)
                    {
                    }

                    virtual
                    ~groupable(void)
                    {
                    }
                };

            template<typename AtomicInt, typename B>
                class slot_base : public B
                {
                    mutable AtomicInt _unusable;

                    //const static int _disconnected = INT_MIN;
                public:
                    virtual
                    ~slot_base(void)
                    {
                        // TODO: slot_base destructor
                    }

                    typedef AtomicInt atomic_int_type;

                    bool
                    block(void) const final override
                    {
                        static int _disconnected = INT_MIN;
                        return compare_and_inc_not_equal(_unusable, _disconnected);
                    }
                    bool
                    unblock(void) const final override
                    {
                        static int _disconnected = INT_MIN;
                        return compare_and_dec_not_equal(_unusable, _disconnected);
                    }

                    bool
                    mark_disconnected(void) final override
                    {
                        static int _disconnected = INT_MIN;
                        return _unusable.exchange(_disconnected) != _disconnected;
                    }

                    bool
                    blocked(void) const final override
                    {
                        return _unusable.load() > 0;
                    }
                    bool
                    connected(void) const final override
                    {
                        static int _disconnected = INT_MIN;
                        return _unusable.load() != _disconnected;
                    }
                    bool
                    usable(void) const final override
                    {
                        return _unusable.load() == 0;
                    }

                    virtual bool
                    valid(void) const override
                    {
                        return false;
                    }

                    virtual bool
                    grouped(void) const override
                    {
                        return false;
                    }
                };

            template<typename ResultType, typename ... Args, typename FunctionType,
                    typename AtomicInt, typename B>
                class slot< ResultType
                (Args...), FunctionType, AtomicInt, B > : public B
                {
                    // TODO: implement slot class
                    FunctionType callback;
                public:
                    slot(const FunctionType& f) :
                            callback(f)
                    {
                    }

                    virtual
                    ~slot(void)
                    {
                    }

                    virtual ResultType
                    operator()(Args ... args) const override
                    {
                        return callback(args...);
                    }
                };

            template<typename Signature, typename FunctionType, typename Group, typename, typename B>
                class grouped_slot : public B
                {
                public:
                    // TODO: implement grouped_slot
                };

            template<typename ResultType, typename ... Args, typename ExtendedFunctionType,
                    typename AtomicInt, typename B>
                class extended_slot< ResultType
                (Args...), ExtendedFunctionType, AtomicInt, B > : public B
                {
                public:
                    // TODO: implement extended_slot
                };

            template<typename Signature, typename ExtendedFunctionType, typename Group,
                    typename AtomicInt, typename B>
                class grouped_extended_slot : public B
                {
                public:
                    // TODO: implement grouped_extended_slot
                };
        }
    }
}

#endif // BOOST_SIGNALS3_SLOTS_HPP
