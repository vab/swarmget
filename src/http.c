#include "http.h"

int write_line_to_socket(int c, char *the_line);
unsigned int read_line(int,unsigned char *,unsigned int);

int http_socket(struct download_stream *the_stream)
{
	int sockfd=0;
	int new_fd=0;
	struct hostent *he;
	struct sockaddr_in local_addr;
	struct sockaddr_in target_addr;
	socklen_t sin_size=0;
	char data[255];
	char buff[1024];
	char *data2;
	
	FILE *df;	
	char dfn[22];
	int z = 0;
	char y = 0;

	long total_read = 0;
	long content_length = 0;
	unsigned char read_buff[1024];


	/* DEBUG DATA */
	printf("Starting Thread: %d\n", the_stream->sid);
	printf("Host: %s\n\n", the_stream->hostname);
	/*  END DEBUG DATA */

	
	/* open a socket to fetch the content from */
	if((he = gethostbyname(the_stream->hostname)) == NULL)
	{
		fprintf(stderr,"Failed to get host by name.\n");
	
		return -1;
	}

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		fprintf(stderr,"Failed to create socket.\n");

		return -1;
	}
	
	target_addr.sin_family = AF_INET;
	target_addr.sin_port = htons(the_stream->port);
	target_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(target_addr.sin_zero,0,sizeof(target_addr.sin_zero));

	if(connect(sockfd,(struct sockaddr *)&target_addr,
		sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr,"Failed to open socket.\n");

		return -1;
	}

	snprintf(buff,1023,"GET %s HTTP/1.1\n", the_stream->loc);
	write_line_to_socket(sockfd,buff);
	snprintf(buff,1023,"Host: %s\n",the_stream->hostname);
	write_line_to_socket(sockfd,buff);
	snprintf(buff,60,"Range: bytes=%d-%d\n",the_stream->start,
			the_stream->stop);
	write_line_to_socket(sockfd,buff);
	write_line_to_socket(sockfd,"Accept: */*\n\n");

	snprintf(dfn,20,"file%d",the_stream->sid);
	the_stream->filename = (unsigned char *)malloc(strlen(dfn)+1);
	if(the_stream->filename == NULL)
	{
		fprintf(stderr,"failed to malloc the_stream->filename\n");

		return -1;
	}
	strncpy(the_stream->filename,dfn,strlen(dfn));
	
	if((df = fopen(dfn,"wb")) == NULL)
	{
		printf("Failed to open datafile.\n");

		return -1;	
	}
	
	memset(data,0,sizeof(data));

	do
	{
		z = read_line(sockfd,buff,64);
		
	} while ((z != 0) && (strncasecmp("Content-Length",buff,14) != 0));

	if(strncasecmp("Content-Length: ",buff,16) == 0)
	{
		data2 = strtok(buff," ");
		data2 = strtok('\0'," ");
		content_length = atoi(data2);
	}
	do
	{
		z = read_line(sockfd,buff,64);
	} while ((z != 0) && (buff[0] != '\n') && (buff[0] != '\r'));

	while(total_read < content_length)
	{
		read_buff[0] = '\0';
		z=read(sockfd,read_buff,1023);
		read_buff[z] = '\0';
		fwrite(read_buff,1,z,df);
		total_read += z;
	}	
	

	fclose(df);
	shutdown(sockfd,2);

	
	return 0;
}


int write_line_to_socket(int c, char *the_line)
{
	int status = 0;
        int result = 0;
	int count = 0;

        count = strlen(the_line);
	
        while(status != count)
        {
        	result = write(c, the_line + status, count - status);
         	if (result < 0) return result;
        	status += result;
        }

        return(status);
}


/*  Adapated from W. Richard Stevens: Unix Network Programming */
unsigned int read_line(int skt, unsigned char *buffer, unsigned int read_len)
{
        unsigned char c, *ptr;
        unsigned int n, rc;
        ptr = buffer;
	read_len = read_len -2;

        for(n=1; n < read_len; n++)
        {
                if((rc = read(skt, &c,1)) == 1)
                {
                        *ptr++ = c;
                        if(c == '\n')
                        {
                                break;
                        }
                }
                else if(rc == 0)
                {
                        if(n == 1)
                                return 0;
                        else break;
                }
                else
                        return -1;
        }
        *ptr = 0;

        return n;
}

