/*
 * signals3.hpp
 *
 * (c) 2013 helloworld922
 *
 *  Created on: Jul 21, 2013
 *      Author: helloworld922
 */

#ifndef BOOST_SIGNALS3_HPP
#define BOOST_SIGNALS3_HPP

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <numeric>
#include <boost/make_shared.hpp>
#include <atomic>
#include <boost/atomic.hpp>
#include <vector>

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <iterator>
#include <functional>
#include <forward_list>

#include <tuple>

#include "track_list.hpp"

#include "connection.hpp"
#include "optional_last_value.hpp"
#include "detail/extended_signature.hpp"
#include "slots.hpp"
#include "signal.hpp"

#include "detail/signals3_impl.hpp"

#endif // BOOST_SIGNALS3_HPP
