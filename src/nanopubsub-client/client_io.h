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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <netdb.h>
#include <time.h>

#include <message.h>

#include "defs.h"


typedef struct
{
	uint8_t programMode;
	
	char *host;
	
	unsigned short port;
	
	char *clientid;
	
	char *topic;
	
	char *body;

	bool version;

	bool help;
} nanoPubSub__ClientIO_options;


int nanoPubSub__ClientIO_getCLOptions(int argc, char **argv,
	nanoPubSub__ClientIO_options *opts);


/**
 * Prints the program version to the standard output (stdout).
 */
void nanoPubSub__ClientIO_printVersion(void);


/**
 * Prints information about how to use the program to the standard
 * output (stdout).
 */
void nanoPubSub__ClientIO_printUsage(void);


/**
 * Prints an error message to the standard output (stdout), indicating
 * that at least one of the parameters required for sending a standard
 * text message is missing.
 */
void nanoPubSub__ClientIO_printErrMsgOptions(void);


/**
 * Prints an error message to the standard output (stdout), indicating
 * that at least one of the parameters required for sending a
 * subscribe/unsubscribe message is missing.
 */
void nanoPubSub__ClientIO_printErrSubOptions(void);


/**
 * Prints an error message to the standard output (stdout), indicating
 * that the lookup of a host name failed.
 */
void nanoPubSub__ClientIO_printErrHostNameLookup(void);


/**
 * Prints an error message to the standard output (stdout), indicating
 * that an error occurred while trying to create a socket.
 */
void nanoPubSub__ClientIO_printErrSocket(void);


/**
 * Prints an error message to the standard output (stdout), indicating
 * that an error occurred while trying to bind to an address.
 */
void nanoPubSub__ClientIO_printErrBind(void);


/**
 * Prints an error message to the standard output (stdout), indicating
 * that a message could not be sent.
 */
void nanoPubSub__ClientIO_printErrSend(void);


/**
 * Prints a message to the standard output (stdout), informing
 * the user that a message was successfully sent.
 *
 * @param bytesSent The number of bytes sent
 */
void nanoPubSub__ClientIO_printSuccessSend(unsigned int bytesSent);


/**
 * Prints the given local time to the standard output (stdout).
 *
 * @param clock The time to print as a unix timestamp
 */
void nanoPubSub__ClientIO_printLocaltime(const time_t *clock);


/**
 * Prints a nanoPubSub message to the standard output (stdout).
 *
 * @param msg The message to print
 * @param timestamp If true, a timestamp is output before the message
 */
void nanoPubSub__ClientIO_printMessage(const nanoPubSub__Message *msg,
	bool timestamp);
