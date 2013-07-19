/*
 * connection.hpp
 *
 * (c) 2013 helloworld922
 *
 *  Created on: Jul 13, 2013
 *      Author: helloworld922
 */

#ifndef CONNECTION_HPP_
#define CONNECTION_HPP_

#include <memory>

namespace boost
{
    namespace signals3
    {

        class connection
        {
        public:
            connection(void);
            connection(const connection& c);
            connection(connection&& c);

            connection& operator=(const connection& c);
            connection& operator=(connection&& c);

            virtual ~connection(void);

            void disconnect(void);
            bool connected(void) const;

            void block(void);
            void unblock(void);
            bool blocked(void) const;

            bool usable(void) const;
        };

        class scoped_connection : connection
        {
        public:
            scoped_connection(void);
            scoped_connection(const connection& conn);

            virtual ~scoped_connection(void)
            {
                disconnect();
            }
        };
    }
}


#endif /* CONNECTION_HPP_ */
