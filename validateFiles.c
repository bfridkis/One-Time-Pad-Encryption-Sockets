/*********************************************************************************
** Program name: validateFiles.c
** Class name: CS344
** Author: Ben Fridkis
** Date: 11/21/2018
** Description: Function to validate input files (plain text and key). Function
**				returns true (1) if the plaintext and key files contain only
**				capital alpha and space characters (besides trailing newline
**				character in the last character position of each file). Otherwise,
**				prints an error message to stderr and exits.
**				Requires open file descriptors (for plaintext and key) as 
**				arguments 1 and 2, respectively.
**********************************************************************************/

#include "validateFiles.h"

static void error(const char *msg) { perror(msg); exit(1); } 

int validateFiles(int pt, int k, int mode){

	char ptBuffer[513], kBuffer[513], charsRead;
	
	//Compile a regular expression that will match any character
	//that is not a space, uppercase alpha character, or newline.
	//(See regexGenerator.c for additional details.)
	//Check both the plain text and key files for invalid characters.
	//If any exist, print error message and exit.
	regex_t* regex = regexGenerator("[^[:upper:]\n ]");
	int ptNewlineEncountered = 0, kNewlineEncountered = 0;
	while(memset(ptBuffer, '\0', sizeof(ptBuffer)) && 
			(charsRead = read(pt, ptBuffer, sizeof(ptBuffer) - 1)) != 0){
		if(charsRead == -1){
			error("OTP_ENC: Unable to read plain text file"); 
		}

		//Check for any newline that doesn't fall at the end of the file (for plaintext).
		//If one is encountered, print error message and exit.
		char* newlineLocation;
		if((newlineLocation = strstr(ptBuffer, "\n")) != NULL){ 
			if(ptNewlineEncountered > 0 || newlineLocation != &(ptBuffer[strlen(ptBuffer) - 1])){
				fprintf(stderr, "OTP_ENC: Invalid newline character(s) in plaintext file\n");
				exit(1);
			}
			else{
				ptBuffer[511] = '\0';
				ptNewlineEncountered++;
			}
		}
		//Check for invalid character using regular expression compiled above
		if(regexec(regex, ptBuffer, 0, NULL, 0) == 0){
			fprintf(stderr, "OTP_ENC: Invalid character(s) in %s file\n",
						(mode == 1) ? "plaintext" : "encrypted");
			exit(1);
		}

		memset(kBuffer, '\0', sizeof(kBuffer));
		if(read(k, kBuffer, charsRead) == -1){
			error("OTP_ENC: Unable to read key file");
		}

		//Check for any newline that doesn't fall at the end of the file (for key).
		//If one is encountered, print error message and exit.
		if((newlineLocation = strstr(kBuffer, "\n")) != NULL){
			if(kNewlineEncountered > 0 || newlineLocation != &kBuffer[strlen(kBuffer) - 1]){
				fprintf(stderr, "OTP_ENC: Invalid newline character(s) in key file\n");
				exit(1);
			}
			else{
				kBuffer[511] = '\0';
				kNewlineEncountered++;
			}
		}
		if(regexec(regex, kBuffer, 0, NULL, 0) == 0){
			fprintf(stderr, "OTP_ENC: Invalid character(s) in key file\n");
			exit(1);
		}
	}
	//free(regex);

	return 0;
}

/* References
 * https://en.wikibooks.org/wiki/Regular_Expressions/POSIX_Basic_Regular_Expressions
 * https://linux.die.net/man/3/regexec
 * http://web.archive.org/web/20160308115653/http://peope.net/old/regex.html
 * https://stackoverflow.com/questions/1085083/regular-expressions-in-c-examples
*/
