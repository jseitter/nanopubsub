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

#include <netdb.h>

#include <message.h>
#include <network.h>

#include "defs.h"
#include "client_io.h"

/** Program options */
static nanoPubSub__ClientIO_options options;


/**
 * Sends a message over the network as specified in the static variable
 * options.
 * @return 0 on success, -1 otherwise
 */
inline static int sendMessage(void);


/**
 * Listens for incoming messages and prints them to the standard output
 * (stdout).
 */
inline static int receiveMessages(void);