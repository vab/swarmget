#include "datastructures.h"


int init_swarm_dl(struct swarm_dl *the_swarm)
{
	the_swarm->filename = NULL;
	the_swarm->hash = NULL;
	the_swarm->hash_algo = D_SHA;
	the_swarm->size = 0;

	the_swarm->streams = NULL;

	
	return 0;
}


int init_download_stream(struct download_stream *the_stream)
{
	the_stream->sid = 0;
	the_stream->block_id = 0;
	the_stream->start = 0;
	the_stream->stop = 0;
	the_stream->url = NULL;
	the_stream->hostname = NULL;
	the_stream->loc = NULL;
	the_stream->protocol = D_HTTP;
	the_stream->port = 80;
	the_stream->filename = NULL;
	the_stream->thread = NULL;
	the_stream->status = -1;

	the_stream->next = NULL;
	the_stream->prev = NULL;

	return 0;
}


void add_download_stream(struct download_stream *the_streams, struct download_stream *new_stream)
{
	struct download_stream *last_stream;

        last_stream = (struct download_stream *)get_last_download_stream(the_streams);
        if(last_stream == NULL)
        {
		the_streams = new_stream;
                new_stream->prev = NULL;
                new_stream->next = NULL;
        }
        else
        {
		last_stream->next = new_stream;
                new_stream->prev = last_stream;
                new_stream->next = NULL;
                the_streams = new_stream;
        }
}


struct download_stream *get_first_download_stream(struct download_stream *streams)
{
	if(streams != NULL)
        {
		while(streams->prev != NULL)
                {
			streams = streams->prev;
                }
        }
	
	return streams;
}


struct download_stream *get_last_download_stream(struct download_stream *streams)
{
        if(streams != NULL)
        {
                while(streams->next != NULL)
                {
                        streams = streams->next;
                }
        }

        return streams;
}


void free_swarm(struct swarm_dl *the_swarm)
{
	if(the_swarm != NULL)
	{
		free_download_streams(the_swarm->streams);

		if(the_swarm->filename != NULL) free(the_swarm->filename);
		if(the_swarm->hash != NULL) free(the_swarm->hash);
		if(the_swarm->streams != NULL) free(the_swarm->streams);	
		
		free(the_swarm);
	}

	return;
}


void free_download_streams(struct download_stream *streams)
{
	struct download_stream *walk_stream;
        struct download_stream *next_stream;

	if(streams != NULL)
        {
        	walk_stream = (struct download_stream *)get_first_download_stream(streams);
		while(walk_stream != NULL)
                {
                	next_stream = walk_stream->next;
			if(walk_stream->url != NULL) free(walk_stream->url);
			if(walk_stream->loc != NULL) free(walk_stream->loc);
			if(walk_stream->hostname != NULL) free(walk_stream->hostname);
			if(walk_stream->filename != NULL) free(walk_stream->filename);
			if(walk_stream->thread != NULL) free(walk_stream->thread);
                        free(walk_stream);
			
                        walk_stream = next_stream;
                }
        }
}

