#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

#include "http.h"
#include "datastructures.h"
#include "ntsfile.h"
#include "md5.h"
#include "sha.h"


void *start_download(void *);
int  assemble_tmpfiles(struct swarm_dl *);
int  verify_download(struct swarm_dl *);

