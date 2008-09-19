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

#include "client_io.h"


int nanoPubSub__ClientIO_getCLOptions(int argc, char **argv,
		nanoPubSub__ClientIO_options *opts)
{
	/* Make sure opts is not a Null pointer */
	assert(opts != NULL);

	struct option long_options[] =
	{
		{"listen",   no_argument,       NULL, 'l'},
		{"sub",      no_argument,       NULL, 's'},
		{"unsub",    no_argument,       NULL, 'u'},
		{"msg",      no_argument,       NULL, 'm'},
		{"host",     required_argument, NULL, 'h'},
		{"port",     required_argument, NULL, 'p'},
		{"topic",    required_argument, NULL, 't'},
		{"clientid", required_argument, NULL, 'i'},
		{"body",     required_argument, NULL, 'b'},
		{"version",  no_argument,       NULL, 'v'},
		{"help",     no_argument,       NULL, '?'},
		{0, 0, 0, 0}
	};

	int c;
	size_t size;
	
	do {
		c = getopt_long(argc, argv, "lsumh:p:t:i:b:?", long_options, NULL);

		switch (c)
		{
			case 'l':
				opts->programMode = NANOPUBSUB__CLIENT_MODE_LISTEN;
				break;

			case 'm':
				opts->programMode = NANOPUBSUB__CLIENT_MODE_MSG;
				break;

			case 's':
				opts->programMode = NANOPUBSUB__CLIENT_MODE_SUB;
				break;

			case 'u':
				opts->programMode = NANOPUBSUB__CLIENT_MODE_UNSUB;
				break;
			
			case 'h':
				size = strlen(optarg);
				if (opts->host != NULL) { free(opts->host); }
				opts->host = (char*)malloc(size);
				strcpy(opts->host, optarg);
				break;

			case 'p':
				opts->port = strtol(optarg, 0, 10);
				break;

			case 'i':
				size = strlen(optarg);
				if (opts->clientid != NULL) { free(opts->clientid); }
				opts->clientid = (char*)malloc(size);
				strcpy(opts->clientid, optarg);
				break;

			case 't':
				size = strlen(optarg);
				if (opts->topic != NULL) { free(opts->topic); }
				opts->topic = (char*)malloc(size);
				strcpy(opts->topic, optarg);
				break;

			case 'b':
				size = strlen(optarg);
				if (opts->body != NULL) { free(opts->body); }
				opts->body = (char*)malloc(size);
				strcpy(opts->body, optarg);
				break;

			case 'v':
				opts->version = true;
				break;

			case '?':
				opts->help = true;
				break;

			default:
				break;
		}
	} while (c != -1);

	return 1;
}


/**
 * Prints the program version to the standard output (stdout).
 */
void nanoPubSub__ClientIO_printVersion(void)
{
	printf("nanoPubSub client, Version %s\n", NANOPUBSUB__CLIENT_VERSION);
}


/**
 * Prints information about how to use the program to the standard
 * output (stdout).
 */
void nanoPubSub__ClientIO_printUsage(void)
{
	printf("Usage: nanopubsub-client [options]\n\n");
	
	printf("Options:\n");
	printf("  --listen, -l    Listen for incoming messages\n");
	printf("  --sub, -s       Send a subscribe message to the server\n");
	printf("  --unsub, -u     Send an unsubscribe message to the server\n");
	printf("  --msg, -m       Send a standard (text) message to the server\n");
	printf("  --host, -h      The host name of the server to send a message"
	                          " to\n");
	printf("  --port, -p      The port number of the server or the port number\n"
	       "                  to listen on for incoming messages\n");
	printf("  --topic, -t     The message topic\n");
	printf("  --clientid, -i  The client id for this client\n");
	printf("  --body, -b      The body (text part) of the message to send\n");
	printf("  --version, -v   Display version information\n");
	printf("  --help, -?      Display this message\n");
}


/**
 * Prints an error message to the standard output (stdout), indicating
 * that at least one of the parameters required for sending a standard
 * text message is missing.
 */
void nanoPubSub__ClientIO_printErrMsgOptions(void)
{
	printf("--host, --clientid, --topic and --body must be supplied when"
	       " sending a message!\n");
}


/**
 * Prints an error message to the standard output (stdout), indicating
 * that at least one of the parameters required for sending a
 * subscribe/unsubscribe message is missing.
 */
void nanoPubSub__ClientIO_printErrSubOptions(void)
{
	printf("--clientid and --topic must be supplied when sending a"
	       " subscribe/unsubscribe message!\n");
}


/**
 * Prints an error message to the standard output (stdout), indicating
 * that the lookup of a host name failed.
 */
void nanoPubSub__ClientIO_printErrHostNameLookup(void)
{
	printf("Host name lookup failed: %s\n", hstrerror(h_errno));
}


/**
 * Prints an error message to the standard output (stdout), indicating
 * that an error occurred while trying to create a socket.
 */
void nanoPubSub__ClientIO_printErrSocket(void)
{
	printf("Could not create socket!\n");
}


/**
 * Prints an error message to the standard output (stdout), indicating
 * that an error occurred while trying to bind to an address.
 */
void nanoPubSub__ClientIO_printErrBind(void)
{
	printf("Could not bind to address!\n");
}


/**
 * Prints an error message to the standard output (stdout), indicating
 * that a message could not be sent.
 */
void nanoPubSub__ClientIO_printErrSend(void)
{
	printf("The message could *NOT* be sent!\n");
}


/**
 * Prints a message to the standard output (stdout), informing
 * the user that a message was successfully sent.
 *
 * @param bytesSent The number of bytes sent
 */
void nanoPubSub__ClientIO_printSuccessSend(unsigned int bytesSent)
{
	printf("The message was successfully sent (%u bytes).\n", bytesSent);
}


/**
 * Prints the given local time to the standard output (stdout).
 *
 * @param clock The time to print as a unix timestamp
 */
inline void nanoPubSub__ClientIO_printLocaltime(const time_t *clock)
{
	struct tm timeinfo;
	char timestring[26];
	
	localtime_r(clock, &timeinfo);
	asctime_r(&timeinfo, (char *restrict)&timestring);
	
	/* Strip the newline character from the string */
	timestring[24] = '\0';
	
	printf("%s", timestring);
}


/**
 * Prints a nanoPubSub message to the standard output (stdout).
 *
 * @param msg The message to print
 * @param timestamp If true, a timestamp is output before the message
 */
void nanoPubSub__ClientIO_printMessage(const nanoPubSub__Message *msg,
		bool timestamp)
{
	char *buffer;
	size_t length = nanoPubSub__Message_length(msg);
	
	if ((buffer = (char*)malloc(length+1)) == NULL) {
		printf("Out of memory!\n");
		return;
	}

	if (timestamp) {
		time_t t = time(NULL);
		printf("[");
		nanoPubSub__ClientIO_printLocaltime( &t );
		printf("] ");
	}
	
	nanoPubSub__Message_writeString(msg, buffer, length+1);
	printf("%s\n", buffer);
	
	free(buffer);
}
