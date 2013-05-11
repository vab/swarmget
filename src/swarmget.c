#include "swarmget.h"


int main(void)
{
	int i = 0;
	pthread_t *new_thread;
	int rslt = 0;

	struct swarm_dl *the_dl;	
	struct download_stream *walk_stream;

	the_dl = NULL;
	walk_stream = NULL;

	the_dl = (struct swarm_dl *)malloc(sizeof(struct swarm_dl));
	if(the_dl == NULL)
	{
		fprintf(stderr,"Malloc call failed.\n");

		return -1;
	}
	init_swarm_dl(the_dl);

	printf("Processing NTS file...\n");
	
	the_dl->streams = process_nts_file("nts.txt",the_dl);
	if(the_dl->streams == NULL)
	{
		fprintf(stderr, "streams is null failed to parse ntsfile.\n");

		return -1;
	}

	/* Set up tmp file loc */
	if(mkdir(the_dl->hash, S_IRUSR|S_IWUSR|S_IXUSR) != 0)
	{
		fprintf(stderr,"Failed to make dir.\n");

		return -1;
	}

	if(chdir(the_dl->hash) != 0)
	{
		fprintf(stderr,"Failed to change dir.\n");

		return -1;
	}
		
	printf("Starting Download Swarm...\n");

	walk_stream = get_first_download_stream(the_dl->streams);
	while(walk_stream != NULL)
	{
		walk_stream->thread = (pthread_t *)malloc(sizeof(pthread_t));

		pthread_create(walk_stream->thread,NULL,start_download,(void *)walk_stream);

		walk_stream = walk_stream->next;
	}
	
	walk_stream = get_first_download_stream(the_dl->streams);
	while(walk_stream != NULL)
	{
		/* replace that null and make sure we got right # of bytes */
		pthread_join(*(walk_stream->thread),NULL);

		walk_stream = walk_stream->next;
	}

	printf("Transmission Completed.\n");
	printf("Assembling Download....\n");
	/* The DL'd have completed.  */
	if(assemble_tmpfiles(the_dl) != 0)
	{
		fprintf(stderr,"File assembly failed.\n");
		
		return -1;
	}
	
	printf("Verifying Download...\n");
	if(verify_download(the_dl) != 0)
	{
		fprintf(stderr,"Failed to verify the Download.\n");
		fprintf(stderr,"Possible corruption during assembly");
		fprintf(stderr,"or transmission.\n");
		
		return -1;
	}

	
	printf("Download complete.\n");
	/* free streams */
	free_swarm(the_dl);

	
	return 0;
}


void *start_download(void *data)
{
	int rslt = 0;
	

	rslt = http_socket(data);
	((struct download_stream *)data)->status = rslt;
	if(rslt == 0)
	{
		printf("Stream %d Complete.\n",((struct download_stream *)data)->sid);
		fflush(0);
	}
	else if(rslt == -1)
	{
		fprintf(stderr,"http_socket call failed.\n");
		
		return;
	}	


	return;
}


int assemble_tmpfiles(struct swarm_dl *the_dl)
{
	struct download_stream *walk_stream;
	FILE *download;
	FILE *tmp_file;
	unsigned char buff[4096];

	if(the_dl == NULL)
	{
		fprintf(stderr,"Error: Unable to assemble tmp files.\n");
		fprintf(stderr,"the_dl was NULL\n");

		return -1;
	}
	if(the_dl->streams == NULL)
	{
		fprintf(stderr,"Error: Unable to assemble tmp files.\n");
	      	fprintf(stderr,"the_dl->streams was NULL\n");

		return -1;
	}
	if(the_dl->filename == NULL)
	{
		fprintf(stderr,"Erorr: Unable to assemble tmp files.\n");
		fprintf(stderr,"the_dl->filename was NULL\n");
		
		return -1;
	}

	

	if( (download = fopen(the_dl->filename,"w")) == NULL)
	{
		fprintf(stderr,"Error: Unable to assemble tmp file.\n");
	        fprintf(stderr,"Could not open the out file.\n");

		
		return -1;
	}
	
	/* Walk the streams, adding contect to dl file */	
	walk_stream = get_first_download_stream(the_dl->streams);
	while(walk_stream != NULL)
	{
		if( (tmp_file = fopen(walk_stream->filename, "r")) == NULL)	
		{
			fprintf(stderr,"Error: Could not open one of the");
			fprintf(stderr,"stream files.\n");
			fprintf(stderr,"Unable to assemble download.\n");

			
			return -1;
		}
		while(!feof(tmp_file))
		{
			int z = 0;
			
			z = fread(buff,1,4095,tmp_file);
			fwrite(buff,1,z,download);
		}
		
		fclose(tmp_file);	

		/* Unlink the tmp file, we're done with it */
		unlink(walk_stream->filename);
		
		walk_stream = walk_stream->next;
	}

	/* Done.  Close up and return. */
	fclose(download);
	

	return 0;
}


int verify_download(struct swarm_dl *the_dl)
{
	FILE *dl;
	unsigned char hash_bytes[20];
	unsigned char hash[52];

	if( (dl = fopen(the_dl->filename,"r")) == NULL)
	{
		fprintf(stderr,"Failed to open assembled dl.\n");

		
		return -1;
	}	

	if(the_dl->hash_algo == D_SHA)
	{	
		sha_stream(dl,&hash_bytes);

		snprintf(hash,51,"%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x",hash_bytes[0],hash_bytes[1],hash_bytes[2],hash_bytes[3],
		hash_bytes[4],hash_bytes[5],hash_bytes[6],
		hash_bytes[7],hash_bytes[8],hash_bytes[9],
		hash_bytes[10],hash_bytes[11],hash_bytes[12],
		hash_bytes[13],hash_bytes[14],
		hash_bytes[15],hash_bytes[16],hash_bytes[17],
		hash_bytes[18],hash_bytes[19],hash_bytes[20]);

	}
	else if(the_dl->hash_algo == D_MD5)
	{
		md5_stream(dl,&hash_bytes);	

		snprintf(hash,51,"%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x",hash_bytes[0],hash_bytes[1],hash_bytes[2],hash_bytes[3],
		hash_bytes[4],hash_bytes[5],hash_bytes[6],
		hash_bytes[7],hash_bytes[8],hash_bytes[9],
		hash_bytes[10],hash_bytes[11],hash_bytes[12],
		hash_bytes[13],hash_bytes[14],
		hash_bytes[15],hash_bytes[16]);

	
	}
	else
	{
		fprintf(stderr,"Unknown Hash Algo.\n");
		fclose(dl);

		return -1;
	}
		
	fclose(dl);


	if(memcmp(hash,the_dl->hash,strlen(the_dl->hash)) != 0)
	{
		fprintf(stderr,"Error: The download hash does not match.\n");
		
		return -1;
	}
	
	printf("Hash values match.\n");
	
	return 0;
}

