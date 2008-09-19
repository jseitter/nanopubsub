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
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>

#ifndef __LIBNANOPUBSUB__MESSAGE_H
#define __LIBNANOPUBSUB__MESSAGE_H


/** The maximum message length for nanoPubSub messages */
#define NANOPUBSUB__MAX_MESSAGE_LENGTH	1024


/** A standard (text) message */
#define NANOPUBSUB__STANDARD_MESSAGE    0

/** A subscribe message */
#define NANOPUBSUB__SUBSCRIBE_MESSAGE   1

/** An unsubscribe message */
#define NANOPUBSUB__UNSUBSCRIBE_MESSAGE 2


/**
 * This structure encapsulates a nanoPubSub message. A message can be
 * either a standard (text) message, a subscribe message or an
 * unsubscribe message.
 */
typedef struct
{
	/**
	 * The type of the message. This must be NANOPUBSUB__STANDARD_MESSAGE,
	 * NANOPUBSUB__SUBSCRIBE_MESSAGE or NANOPUBSUB_UNSUBSCRIBE_MESSAGE.
	 */
	uint8_t type;

	/** A Null-terminated string that identifies the message's sender */
	char *clientId;
	
	/** The message's topic (Null-terminated string) */
	char *topic;

	/** The message's body (Null-terminated string) */
	char *body;	
} nanoPubSub__Message;


/**
 * Safely adds a number of type size_t to another number of type size_t.
 *
 * This function is used to avoid integer overflows. It first checks if the
 * result of the operation would be greater than the maximum value allowed
 * for the type size_t and only executes the operation if it is possible.
 *
 * @param numberA A pointer to the first number. The other number is added to
 *                the value pointed to by this pointer.
 * @param numberB The second number.
 * @return 1 on success, 0 if the numbers couldn't be added
 */
static inline int nanoPubSub__Message_safeAdd(size_t *numberA,
		const size_t numberB)
{
#ifndef NDEBUG
	/* Make the assertion at the end of the function work */
	const size_t numberAOld = *numberA;
#endif

	/* (size_t)-1 is the max. value a variable of the type size_t can have.
	   This is needed because the max. value of the type size_t is not
	   defined in <limits.h>. */
	if (*numberA > 0 && numberB > 0 && *numberA > ((size_t)-1) - numberB) {
		/* numberA + numberB would be greater than (size_t)-1.
		   This is an error */
		return 0;
	}
	
	*numberA += numberB;

	/* Make sure everything worked out. */
	assert(*numberA > numberAOld);
	assert(*numberA > numberB);

	return 1;
}


/**
 * Calculates the length of a given nanoPubSub message (in bytes)
 * and returns it.
 *
 * @param msg The message to calculate the length for
 * @return The length of the given message, or 0 if an error occurred
 */
size_t nanoPubSub__Message_length(const nanoPubSub__Message *msg);


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
	char *buffer, const size_t maxLength);


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
	const unsigned int size, nanoPubSub__Message *msg);


#endif /* __LIBNANOPUBSUB__MESSAGE_H */
