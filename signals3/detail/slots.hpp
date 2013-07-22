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
        // =================================================
        // pre-declarations
        // =================================================
        class signal_base;
        template<typename Signature, typename Combiner, typename Group, typename GroupCompare,
                typename SlotFunction, typename ExtendedSlotFunction, typename AtomicInt,
                typename SharedMutex, typename UniqueLock>
            class signal
            {
                class iterator;
            };

        namespace detail
        {
            class slot_wrapper;

            template<typename Group = int>
                class groupable;

            template<typename Signature, typename AtomicInt = std::atomic< int > >
                class slot_base;

            template<typename Signature, typename FunctionType = std::function< Signature >,
                    typename AtomicInt = std::atomic< int > >
                class slot;

            template<typename Signature, typename FunctionType = std::function< Signature >,
                    typename Group = int, typename AtomicInt = std::atomic< int > >
                class grouped_slot;

            template<typename Signature,
                    typename ExtendedFunctionType = typename ::boost::signals3::detail::extended_signature<
                            Signature >::type, typename AtomicInt = std::atomic< int > >
                class extended_slot;

            template<typename Signature,
                    typename ExtendedFunctionType = typename ::boost::signals3::detail::extended_signature<
                            Signature >::type, typename Group = int,
                    typename AtomicInt = std::atomic< int > >
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
                        typename AtomicInt, typename Mutex, typename UniqueLock>
                    friend class signal;

                template<typename Signature, typename Combiner, typename Group,
                        typename GroupCompare, typename SlotFunction, typename ExtendedSlotFunction,
                        typename AtomicInt, typename Mutex, typename UniqueLock>
                    friend class signal< Signature, Combiner, Group, GroupCompare, SlotFunction,
                            ExtendedSlotFunction, AtomicInt, Mutex, UniqueLock >::iterator;

                friend class ::boost::shared_ptr< slot_wrapper >;
                friend class ::boost::weak_ptr< slot_wrapper >;

            public:
                // TODO: probably should be private, but for some reason is causing compile to fail
                ::boost::shared_ptr< slot_wrapper > next;
                ::boost::weak_ptr< slot_wrapper > prev;

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
                 * @return true if it is safe to cast this to a groupable.
                 */
                virtual bool
                grouped(void) const = 0;

                /**
                 * @return true if it is safe to cast this to extended_slot
                 */
                virtual bool
                extended(void) const = 0;
            };

            template<typename Group>
                class groupable
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

            template<typename ResultType, typename ... Args, typename AtomicInt>
                class slot_base< ResultType
                (Args...), AtomicInt > : public slot_wrapper
                {
                    mutable AtomicInt _unusable;

                    static const int _disconnected = INT_MIN;
                public:
                    std::vector< boost::weak_ptr< void > > tracking_list;

                    virtual
                    ~slot_base(void)
                    {
                        // TODO: slot_base destructor
                    }

                    typedef AtomicInt atomic_int_type;

                    bool
                    block(void) const final override
                    {
                        while (true)
                        {
                            int snapshot = _unusable.load();
                            if (_disconnected == snapshot)
                            {
                                return false;
                            }
                            else
                            {
                                int orig = ++snapshot;
                                if (_unusable.compare_exchange_weak(orig, snapshot))
                                {
                                    return true;
                                }
                            }
                        }
                    }
                    bool
                    unblock(void) const final override
                    {
                        while (true)
                        {
                            int snapshot = _unusable.load();
                            if (_disconnected == snapshot)
                            {
                                return false;
                            }
                            else
                            {
                                int orig = --snapshot;
                                if (_unusable.compare_exchange_weak(orig, snapshot))
                                {
                                    return true;
                                }
                            }
                        }
                    }

                    bool
                    mark_disconnected(void) final override
                    {
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
                    grouped(void) const override
                    {
                        return false;
                    }

                    virtual ResultType
                    operator()(Args ... args) const = 0;
                };

            template<typename ResultType, typename ... Args, typename FunctionType,
                    typename AtomicInt>
                class slot< ResultType
                (Args...), FunctionType, AtomicInt > : public slot_base< ResultType
                (Args...), AtomicInt >
                {
                    const FunctionType callback;
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

                    virtual bool
                    extended(void) const override
                    {
                        return false;
                    }
                };

            template<typename Signature, typename FunctionType, typename Group, typename AtomicInt>
                class grouped_slot : public slot< Signature, FunctionType, AtomicInt >,
                        public groupable< Group >
                {
                public:
                    grouped_slot(const FunctionType& f, const Group& gid) :
                            slot< Signature, FunctionType, AtomicInt >(f), groupable< Group >(gid)
                    {

                    }

                    virtual
                    ~grouped_slot(void)
                    {
                    }
                };

            template<typename ResultType, typename ... Args, typename ExtendedFunctionType,
                    typename AtomicInt>
                class extended_slot< ResultType
                (Args...), ExtendedFunctionType, AtomicInt > : public slot_base< ResultType
                (Args...), AtomicInt >
                {
                    const ExtendedFunctionType callback;
                public:
                    // TODO: probably should be private, but there are issues with friend and shared_ptr
                    ::boost::signals3::connection conn;

                    extended_slot(const ExtendedFunctionType& f) :
                            callback(f)
                    {
                    }

                    virtual
                    ~extended_slot(void)
                    {
                    }

                    virtual ResultType
                    operator()(Args ... args) const override
                    {
                        return callback(conn, args...);
                    }

                    virtual bool
                    extended(void) const
                    {
                        return true;
                    }

                    bool
                    operator ==(const ExtendedFunctionType& f) const
                    {
                        return callback == f;
                    }
                };

            template<typename Signature, typename ExtendedFunctionType, typename Group,
                    typename AtomicInt>
                class grouped_extended_slot : public extended_slot< Signature, ExtendedFunctionType,
                        AtomicInt >, public groupable< Group >
                {
                public:
                    grouped_extended_slot(const ExtendedFunctionType& f, const Group& gid) :
                            extended_slot< Signature, ExtendedFunctionType, AtomicInt >(f), groupable<
                                    Group >(gid)
                    {

                    }
                };
        }
    }
}

#endif // BOOST_SIGNALS3_SLOTS_HPP
