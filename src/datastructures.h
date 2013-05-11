/*   datastructures.h  - Main header file for datastructures
 *
 *   Copyright 2003 CryptNET Guerrilla Technology Development
 *   Copyright 2003 V. Alex Brennen <vab@cryptnet.net>
 *
 */


#ifndef DATASTRUCTURES
#define DATASTRUCTURES
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct swarm_dl
{
	unsigned char *filename;
	unsigned char *hash;
	unsigned int  hash_algo;
	unsigned long size;
	
	struct download_stream *streams;
};

struct download_stream
{
	unsigned int sid;
	unsigned int block_id;
	long start;
	long stop;
	unsigned int status;
	unsigned char *url;
	unsigned char *loc;
	unsigned char *hostname;
	unsigned int protocol;
	unsigned int port;
	unsigned char *filename;
	int sta;
	
	pthread_t	*thread;


	struct	download_stream *next;
	struct  download_stream *prev;
};

struct block
{
	unsigned int block_id;
	unsigned int status;
	long start;
	long stop;

	struct block *next;
	struct block *prev;
};


int init_swarm_dl(struct swarm_dl *);
int init_download_stream(struct download_stream *);

void add_download_stream(struct download_stream *, struct download_stream *);
struct download_stream *get_first_download_stream(struct download_stream *);
struct download_stream *get_last_download_stream(struct download_stream *);

void free_swarm(struct swarm_dl *);
void free_download_streams(struct download_stream *);

/* Protocol Constants */
#define D_HTTP		1
#define D_HTTPS		2

/* Hash Algo Constants */
#define D_SHA		1
#define D_MD5		2

#endif

