/*
 *   nanoPubSub - embedded Publish Subscribe Messaging
 *
 *   Version: 0.1 (2008-09-19)
 *   Author:  Sebastian Boschert <sebastian@2007.org>
 *
 *   (c) 2008 STZ Building Technology
 * 
 *   nanoPubSub is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 2 of the License.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 */

#include <stdlib.h>
#include <sys/socket.h>

#include "message.h"


#ifndef __LIBNANOPUBSUB__NETWORK_H
#define __LIBNANOPUBSUB__NETWORK_H


/**
 * Sends a given nanoPubSub message to another socket with the given
 * destination address.
 *
 * @param socket The file descriptor of the socket to use for the transmission
 * @param destAddr The address of the target
 * @param msg The message to send
 *
 * @return Upon successful completion, the number of bytes which were sent is
 *         returned. Otherwise, -1 is returned and the global variable errno is
 *         set to indicate the error.
 */
ssize_t nanoPubSub__Network_sendMessage(int socket,
	const struct sockaddr *destAddr, const nanoPubSub__Message *msg);


int nanoPubSub__Network_recvMessage(int socket, struct sockaddr* fromAddr,
	nanoPubSub__Message *msg);


#endif /* _LIBNANOPUBSUB__NETWORK_H */