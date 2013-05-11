/*
 *  ntsfile.h - main header file for code to process
 *              Non-Transient Swarm Files.
 *
 *  Copyright 2003 CryptNET, V. Alex Brennen
 *
 *  License: GPL
 *
 */

#include "datastructures.h"


struct download_stream *process_nts_file(char *, struct swarm_dl *);
int process_url(struct download_stream *);

