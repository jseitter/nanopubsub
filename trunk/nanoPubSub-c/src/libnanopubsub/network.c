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

#include "network.h"


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
	const struct sockaddr *destAddr, const nanoPubSub__Message *msg)
{
	size_t length;
	ssize_t sendSize = 0;
	char *stringbuffer;

	/* We only have to send messages that are longer than 0 bytes */
	if ((length = nanoPubSub__Message_length(msg)) > 0) {
		/* Allocate memory for the string representation of the message */
		if ((stringbuffer = (char*)malloc(length + 1)) != NULL) {
			/* Copy the string to the newly allocated memory */
			nanoPubSub__Message_writeString(msg, stringbuffer, length + 1);

			/* Send the message ov the socket to destAddr */
			sendSize = sendto(socket, stringbuffer, length, 0,
			                  destAddr, sizeof(struct sockaddr));

			/* Free the memory allocated for the message string */
			free(stringbuffer);
		} else {
			/* An error occurred while trying to allocate memory */
			sendSize = -1;
		}
	}

	return sendSize;
}


int nanoPubSub__Network_recvMessage(int socket, struct sockaddr* fromAddr,
		nanoPubSub__Message *msg)
{
	char buffer[NANOPUBSUB__MAX_MESSAGE_LENGTH];
	socklen_t fromAddrLength = sizeof(fromAddr);
	int bytesReceived;

	bytesReceived = recvfrom(socket, &buffer, NANOPUBSUB__MAX_MESSAGE_LENGTH,
		0, fromAddr, &fromAddrLength);
		
	if (bytesReceived == -1) {
		return 0;
	}
	
	if (nanoPubSub__Message_parseString(buffer, bytesReceived, msg) == 0) {
		return 0;
	}

	return 1;
}
