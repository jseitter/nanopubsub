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

#include "message.h"

/*
 * A simple preprocessor macro shortcut to make the code within the
 * second switch statement in the function nanoPubSub__Message_length
 * look cleaner.
 */
#define __SAFEADD(a,b) nanoPubSub__Message_safeAdd(a, b)


/**
 * Calculates the length of a given nanoPubSub message (in bytes)
 * and returns it.
 *
 * @param msg The message to calculate the length for
 * @return The length of the given message, or 0 if an error occurred
 */
size_t nanoPubSub__Message_length(const nanoPubSub__Message *msg)
{
	size_t length = 0;

	/* Make sure the message is not a Null pointer */
	if (msg == NULL) {
		return 0;
	}

	/* Add number of "special" characters to the length */
	switch (msg->type)
	{
		case NANOPUBSUB__SUBSCRIBE_MESSAGE:
			length += 7; /* 4x '#' + "sub" */
			break;
		
		case  NANOPUBSUB__UNSUBSCRIBE_MESSAGE:
			length += 9; /* 4x '#' + "unsub" */
			break;
		
		case NANOPUBSUB__STANDARD_MESSAGE:
			length += 8; /* 5x '#' + "msg" */
			break;
			
		default:
			/* The message obviously has an invalid format */
			return 0;
			break;
	}

	/* Add number of characters used for the message content to the length */
	switch (msg->type)
	{
		case NANOPUBSUB__STANDARD_MESSAGE:
			if (msg->body != NULL) {
				if ( !__SAFEADD(&length, strlen(msg->body)) ) {
					return 0;
				}
			} else {
				/* The body is a Null string. This is an error. */
				return 0;
			}
		
		case NANOPUBSUB__SUBSCRIBE_MESSAGE:
		case NANOPUBSUB__UNSUBSCRIBE_MESSAGE:
			if (msg->clientId != NULL && msg->topic != NULL) {
				if (!__SAFEADD(&length, strlen(msg->clientId))
					|| !__SAFEADD(&length, strlen(msg->topic))) {
						return 0;
				}
			} else {
				/* Either the client id or the topic are Null strings.
				   This is an error. */
				return 0;
			}
			
		default:
			break;
	}

	return length;
}


/**
 * Generates a Null-terminated string representation of the given
 * message and writes it into a buffer.
 *
 * @param msg The message to generate a string for
 * @param buffer The buffer to write the message's string representation
 *               into
 * @param maxLength The maximum number of bytes to write into the buffer
 *                  (including the trailing Null character).
 */
void nanoPubSub__Message_writeString(const nanoPubSub__Message *msg,
	char *buffer, const size_t maxLength)
{
	/* If the message is a Null pointer, we don't have to do that much... */
	if (msg == NULL) {
		if (maxLength > 0) { buffer[0] = '\0'; }
		return;
	}

	/* Make sure the message string has a length */
	assert(nanoPubSub__Message_length(msg) > 0);

	/* Make sure the message string will fit into the buffer */
	assert(nanoPubSub__Message_length(msg) <= maxLength);

	switch (msg->type)
	{
		case NANOPUBSUB__STANDARD_MESSAGE:
			snprintf(buffer, maxLength, "#msg#%s#%s#%s#",
				msg->clientId, msg->topic, msg->body);
			break;

		case NANOPUBSUB__SUBSCRIBE_MESSAGE:
			snprintf(buffer, maxLength, "#sub#%s#%s#",
				msg->clientId, msg->topic);
			break;

		case NANOPUBSUB__UNSUBSCRIBE_MESSAGE:
			snprintf(buffer, maxLength, "#unsub#%s#%s#",
				msg->clientId, msg->topic);
			break;
	}
}


/**
 * Parses a given string for a nanoPubSub message.
 *
 * @param string The Null-terminated string to parse
 * @param size The length of the string (in bytes) to parse
 * @param msg Pointer to the message to write the results into
 *
 * @return 1 on success, 0 on error
 */
int nanoPubSub__Message_parseString(const char* string,
		const unsigned int size, nanoPubSub__Message *msg)
{
	int retval = 1;	/* the function's return value */
	int done   = 0;	/* indicates whether or not we've finished parsing */

	uint8_t state = 0;	/* current state number */

	unsigned int pos = 0;		/* current string position */
	unsigned int strStart = 0;	/* position of the start of the substring */
	unsigned int strLength = 0;	/* length of the substring */
	
	/* current character */
	char c, cl;

	/* Make sure the message is not longer than the max. allowed length */
	if (size > NANOPUBSUB__MAX_MESSAGE_LENGTH) {
		return -1;
	}

	/* Iterate over the characters in the string */
	for (pos = 0; pos < size && retval == 1 && done == 0; pos++) {
		c  = string[pos];
		
		if (state < 10) {
			cl = tolower(c);
		}
		
		switch (state)
		{
			/* STATES 0 - 10: DETERMINE MESSAGE TYPE */
			
			/* initial state: no message detected, yet */
			case 0:
				if (c == '#')
					state = 1;
				else if ( !isspace(c) )
					/* any character except '#' and whitespace is an error */
					retval = 0;
				break;

			/* state #1: beginning of a message detected */
			case 1:
				if (cl == 'm')      state = 2;
				else if (cl == 's') state = 4;
				else if (cl == 'u') state = 6;
				else retval = 0;
				break;
				
			/* state #2: "#m" detected */
			case 2:
				if (cl == 's') state = 3;
				else retval = 0;
				break;

			/* state #3: "#ms" detected */
			case 3:
				if (cl == 'g') {
					state = 10;
					msg->type = NANOPUBSUB__STANDARD_MESSAGE;
				}
				else retval = 0;
				break;

			/* state #4: "#s" detected */
			case 4:
				if (cl == 'u') state = 5;
				else retval = 0;
				break;

			/* state #5: "#su" detected */
			case 5:
				if (cl == 'b') {
					state = 10;
					msg->type = NANOPUBSUB__SUBSCRIBE_MESSAGE;
				} else retval = 0;
				break;

			/* state #6: "#u" detected */
			case 6:
				if (cl == 'n') state = 7;
				else retval = 0;
				break;

			/* state #7: "#un" detected */
			case 7:
				if (cl == 's') state = 8;
				else retval = 0;
				break;

			/* state #8: "#uns" detected */
			case 8:
				if (cl == 'u') state = 9;
				else retval = 0;
				break;

			/* state #9: "#unsu" detected */
			case 9:
				if (cl == 'b') {
					state = 10;
					msg->type = NANOPUBSUB__UNSUBSCRIBE_MESSAGE;
				} else retval = 0;
				break;

			/* state #10: <type> detected */
			case 10:
				if (c == '#') state = 11;
				else retval = 0;
				break;


			/* STATES 11 - 12: READ CLIENT ID */

			/* state #11: <type># detected */
			case 11:
				strStart = pos; /* memorize the string's start position */
				if (c == '#') retval = 0;
				state = 12;
				break;
				
			/* state #11: <type>#<clientid> detected */
			case 12:
				if (c == '#') {
					strLength = pos - strStart;
					if ((msg->clientId = (char*)malloc(strLength + 1))) {
						strncpy(msg->clientId, string+strStart, strLength);
						state = 13;
					} else retval = 0;
				}
				break;


			/* STATES 13 - 14: READ TOPIC */

			/* state #13: <type>#<clientid># detected */
			case 13:
				strStart = pos; /* memorize the string's start position */
				if (c == '#') retval = 0;
				state = 14;
				break;

			/* state #14: <type>#<clientid>#<topic> detected */
			case 14:
				if (c == '#') {
					strLength = pos - strStart;
					if ((msg->topic = (char*)malloc(strLength + 1))) {
						strncpy(msg->topic, string+strStart, strLength);
						
						/* only standard messages continue after here */
						if (msg->type == NANOPUBSUB__STANDARD_MESSAGE)
							state = 15;
						else
							done = 1;
					} else retval = 0;
				}
				break;


			/* STATES 15 - 16: READ MESSAGE BODY */

			/* state #15: <type>#<clientid>#<topic># detected */
			case 15:
				strStart = pos; /* memorize the string's start position */
				if (c == '#') retval = 0;
				state = 16;
				break;

			/* state #16: <type>#<clientid>#<topic>#<body> detected */
			case 16:
				if (c == '#') {
					strLength = pos - strStart;
					if ((msg->body = (char*)malloc(strLength + 1))) {
						strncpy(msg->body, string+strStart, strLength);

						/* we're finished */
						done = 1;
					} else retval = 0;
				}
				break;
				
			default:
				break;
		}
	}
	
	return retval;
}
