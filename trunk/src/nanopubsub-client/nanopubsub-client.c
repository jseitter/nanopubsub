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

#include "nanopubsub-client.h"


int main(int argc, char **argv)
{
	/* Initialize program options with safe defaults */
	options.programMode = NANOPUBSUB__CLIENT_MODE_LISTEN;
	options.host        = NULL;
	options.port        = NANOPUBSUB__CLIENT_DEFAULT_PORT;
	options.clientid    = NULL;
	options.topic       = NULL;
	options.body        = NULL;
	options.version     = false;
	options.help        = false;

	/* Parse command line parameters */
	if (nanoPubSub__ClientIO_getCLOptions(argc, argv, &options) == 0) {
		nanoPubSub__ClientIO_printUsage();
		return 1;
	}
	
	/* Display version information if requested */
	if (options.version) {
		nanoPubSub__ClientIO_printVersion();
	}
	
	/* Display a help message if requested */
	if (options.help) {
		nanoPubSub__ClientIO_printUsage();
		return 0;
	}

	/* Make sure we have a valid port number */
	if (options.port == 0) {
		options.port = NANOPUBSUB__CLIENT_DEFAULT_PORT;
	}

	/* Check the user-supplied options for completeness and call the correct
	   function to continue the program */
	switch (options.programMode)
	{
		case NANOPUBSUB__CLIENT_MODE_MSG:
			if ((char*)(options.body && options.clientid && options.topic)) {
				return sendMessage();
			} else {
				nanoPubSub__ClientIO_printErrMsgOptions();
				return 1;
			}
			break;

		case NANOPUBSUB__CLIENT_MODE_SUB:
		case NANOPUBSUB__CLIENT_MODE_UNSUB:
			if ((char*)(options.clientid && options.topic)) {
				return sendMessage();
			} else {
				nanoPubSub__ClientIO_printErrSubOptions();
				return 1;
			}
			break;

		case NANOPUBSUB__CLIENT_MODE_LISTEN:
			return receiveMessages();

		default:
			break;
	}

	return 0;
}


/**
 * Sends a message over the network as specified in the static variable
 * options.
 * @return 0 on success, -1 otherwise
 */
inline static int sendMessage(void)
{
	struct hostent *hostinfo;
	struct sockaddr_in remoteAddr;
	int socketfd;
	ssize_t bytesSent;
	nanoPubSub__Message msg;

	/* Create the message to send */
	switch (options.programMode)
	{
		case NANOPUBSUB__CLIENT_MODE_MSG:
			msg.type     = NANOPUBSUB__STANDARD_MESSAGE;
			msg.clientId = options.clientid;
			msg.topic    = options.topic;
			msg.body     = options.body;
			break;

		case NANOPUBSUB__CLIENT_MODE_SUB:
			msg.type     = NANOPUBSUB__SUBSCRIBE_MESSAGE;
			msg.clientId = options.clientid;
			msg.topic    = options.topic;
			break;

		case NANOPUBSUB__CLIENT_MODE_UNSUB:
			msg.type     = NANOPUBSUB__UNSUBSCRIBE_MESSAGE;
			msg.clientId = options.clientid;
			msg.topic    = options.topic;
			break;
			
		default:
			return 1;	/* This line should never be reached */
			break;
	}
	
	/* Look up the host name */
	if ((hostinfo = gethostbyname2(options.host, AF_INET)) == NULL) {
		/* An error occurred. Print an error message and exit */
		nanoPubSub__ClientIO_printErrHostNameLookup();
		return 1;
	}

	/* Create a socket */
	if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		free(hostinfo);
		nanoPubSub__ClientIO_printErrSocket();
		return 1;
	}

	/* Send the message */
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port   = htons(options.port);
	remoteAddr.sin_addr   = *((struct in_addr *)hostinfo->h_addr);
	memset(remoteAddr.sin_zero, '\0', sizeof(remoteAddr.sin_zero));

	bytesSent = nanoPubSub__Network_sendMessage(
					socketfd, (const struct sockaddr*)&remoteAddr, &msg);

	/* Close the socket and free dynamically allocated memory */
	close(socketfd);
	free(hostinfo);

	/* Check if an error occurred while sending the message */
	if (bytesSent < 0) {
		nanoPubSub__ClientIO_printErrSend();
		return 1;
	}

	nanoPubSub__ClientIO_printSuccessSend(bytesSent);
	return 0;
}


/**
 * Listens for incoming messages and prints them to the standard output
 * (stdout).
 */
inline static int receiveMessages(void)
{
	int socketfd;
	struct sockaddr_in myAddr, fromAddr;
	nanoPubSub__Message msg;

	/* Create a socket */
	if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		nanoPubSub__ClientIO_printErrSocket();
		return 1;
	}

	/* Bind the socket to a network address */
	myAddr.sin_family      = AF_INET;
	myAddr.sin_port        = htons(options.port);
	myAddr.sin_addr.s_addr = INADDR_ANY;
	memset(myAddr.sin_zero, '\0', sizeof(myAddr.sin_zero));
	
	if (bind(socketfd, (struct sockaddr*)&myAddr, sizeof(myAddr)) == -1) {
		nanoPubSub__ClientIO_printErrBind();
		return 1;
	}
	
	while (1) {
		/* Receive a message over the network connected to the socket */
		if (!nanoPubSub__Network_recvMessage(
				socketfd, (struct sockaddr*)&fromAddr, &msg)) {
			return 1;
		}
		
		/* Only print standard messages */
		if (msg.type == NANOPUBSUB__STANDARD_MESSAGE) {
			nanoPubSub__ClientIO_printMessage(&msg, true);
		}
	}
	
	return 0;
}
