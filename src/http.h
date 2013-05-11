/* http.h - Main header file for http related functions.
 *
 * Copyright 2003 CryptNET, V. Alex Brennen
 *
 * License: GPL
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "datastructures.h"


int http_socket(struct download_stream *);
int write_line_to_socket(int, char *);
unsigned int read_line(int,unsigned char *,unsigned int);

