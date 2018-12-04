/**************************************************************************
** Program name: keygen.c
** Class name: CS344
** Author: Ben Fridkis
** Date: 11/21/2018
** Description: Program to generate a key of random characters of the set
**				{A-Z, " "}, with length specified by the input parameter 
**				length.			
**************************************************************************/

#include <stdio.h>      
#include <stdlib.h>     
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	
	//Load file stream with random number key
	srand(time(NULL));	//Seed the random number generator
	for(int i = 0; i < atoi(argv[1]); i++){
		char random = (rand() % 27) + 65;
		if(random == 91){random = 32;}	//ASCII 32 = [space]
		fputc(random, stdout);
	}

	fputc('\n', stdout);

	return 0;
}

/* References
 * http://www.cplusplus.com/reference/cstdlib/srand/
 * https://www.gnu.org/software/libc/manual/html_node/Opening-and-Closing-Files.html
 * https://stackoverflow.com/questions/6121094/how-do-i-run-a-program-with-commandline-args-using-gdb-within-a-bash-script
 * https://stackoverflow.com/questions/13600204/checking-if-argvi-exists-c
*/
