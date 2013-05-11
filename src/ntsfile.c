/* ntsfile.c - Main source file for routines to parse Non-Transient Swarm
 *             Files. 
 *
 * Copyright 2003 CryptNET, V. Alex Brennen
 *
 * License:  GPL
 *
 */

#include "ntsfile.h"


struct download_stream *process_nts_file(char *file_loc, struct swarm_dl *the_dl)
{
	FILE *nts;
	char first_line[256];
	char url_line[256];

	char file_name[1000];
	char hash[100];
	long size;

	int nodes = 0;
	struct download_stream *new_stream;
	struct download_stream *walk_stream;

	long size_each = 0;
	int rslt = 0;
	int i = 0;
	long pos = 0;
	int hash_len = 0;
	

	first_line[0] = '\0';
	url_line[0] = '\0';

	if( (nts = fopen(file_loc,"r")) == NULL)
	{
		printf("failed to open nts file.\n");

		return NULL;
	}

	fscanf(nts,"%s %s %d",&file_name,&hash,&size);
	hash_len = strlen(hash);
	if(hash_len == 32)
	{
		/* md5 */
		the_dl->hash_algo = D_MD5;
	}
	else if(hash_len == 40)
	{
		/* sha1 */
		the_dl->hash_algo = D_SHA;
	}
	else
	{
		fprintf(stderr,"Invalid Hash Value.\n");
		
		return NULL;
	}
	
	the_dl->filename = (unsigned char *)malloc(strlen(file_name)+1);
	strncpy(the_dl->filename,file_name,strlen(file_name)+1);
	the_dl->hash = (unsigned char *)malloc(strlen(hash)+1);
	strncpy(the_dl->hash,hash,strlen(hash)+1);
	the_dl->size = size;

	while(fscanf(nts,"%s",&url_line) != EOF)
	{
		nodes++;
		new_stream = (struct download_stream *)malloc(sizeof(struct download_stream));
		if(new_stream == NULL)
		{
			fprintf(stderr,"Malloc Call Failed.\n");
			
			return NULL;
		}
		rslt = init_download_stream(new_stream);
		if(rslt == -1)
		{
			fprintf(stderr,"Failed To init dl stream\n");
			
			return NULL;
		}
		new_stream->sid = nodes;
		new_stream->url = (unsigned char *)malloc(strlen(url_line)+1);
		if(new_stream->url == NULL)
		{
			fprintf(stderr,"Failed To Malloc URL\n");

			return NULL;
		}
		strncpy(new_stream->url,url_line,strlen(url_line));
		
		rslt = process_url(new_stream);
		if(rslt != 0)
		{
			fprintf(stderr,"URL parsing failed.\n");


			return NULL;
		}
		
		if(the_dl->streams == NULL)
		{
			the_dl->streams = new_stream;
		}
		else
		{
			add_download_stream(the_dl->streams,new_stream);
		}
	}

	fclose(nts);

	size_each = size/nodes;

	walk_stream = get_first_download_stream(the_dl->streams);
	for(i=0;i<nodes;i++)
	{
		walk_stream->start = pos;
		pos = pos + size_each;
		walk_stream->stop = pos-1;
		if(i==(nodes-1))
		{
			walk_stream->stop = size;
		}
		else
		{
			if(walk_stream != NULL) walk_stream = walk_stream->next;
		}

	}

	printf("Swarm Size:  %d nodes parsed.\n",nodes);


	return the_dl->streams;
}


int process_url(struct download_stream *the_stream)
{
	char *ptr;
	char *ptr2;

	the_stream->port = 80;
	the_stream->loc = NULL;
	the_stream->hostname = NULL;
	
	if(the_stream->url == NULL)
	{
		fprintf(stderr,"NULL url. Corruption in ntsfile detected.\n");

		return -1;
	}
	if(memcmp(the_stream->url,"http://",7) != 0)
	{
		fprintf(stderr,"Corrpution in ntsfile detected. No http prefix.\n");
		
		return -1;
	}
	else
	{
		the_stream->protocol = D_HTTP;
	}

	ptr = &the_stream->url[7];

	ptr2 = strchr(ptr,'/');
			
	the_stream->loc = (char *)malloc(strlen(ptr2)+1);
	if(the_stream->loc == NULL)
	{
		fprintf(stderr,"Failed to malloc for loc\n");

		return -1;
	}
	strncpy(the_stream->loc,ptr2,strlen(ptr2));

	ptr2[0] = '\0';

	the_stream->hostname = (char *)malloc(strlen(ptr)+1);
	if(the_stream->hostname == NULL)
	{
		fprintf(stderr,"Failed to malloc for hn\n");
		
		return -1;
	}
	strncpy(the_stream->hostname,ptr,strlen(ptr));


	return 0;
}

