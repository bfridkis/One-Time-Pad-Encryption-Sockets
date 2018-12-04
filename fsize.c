/****************************************************************************************************
** Program name: fsize.c
** Class name: CS344
** Author: Ben Fridkis
** Date: 11/21/2018
** Description: Utility wrapper function that uses stat 
**				to return a file size in bytes.
**				CODE TAKEN DIRECTLY FROM:
**				https://stackoverflow.com/questions/8236/how-do-you-determine-the-size-of-a-file-in-c
******************************************************************************************************/

#include "fsize.h"

off_t fsize(const char *filename) {
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    fprintf(stderr, "\nCannot determine size of %s: %s\n",
            filename, strerror(errno));
			
	//perror("File Size:");		

    return -1;
}