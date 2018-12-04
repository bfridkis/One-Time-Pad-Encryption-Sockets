/*******************************************************************************
** Program name: regexGenerator.c
** Class name: CS344
** Author: Ben Fridkis
** Date: 11/26/2018
** Description: Program to generate a compiled regular expression based on the
**				the regular expression pattern provided in regexSpecifier.
********************************************************************************/

#include "regexGenerator.h"

regex_t* regexGenerator(char* regexSpecifier){
	
	//Declare regex_t and return value variables
	static regex_t regex;
	int retV;

	//Compile the regular expression
	if((retV = regcomp(&regex, regexSpecifier, REG_NOSUB | REG_EXTENDED)) != 0){
		char msgBuffer[100];
		regerror(retV, &regex, msgBuffer, sizeof(msgBuffer));
		fprintf(stderr, "Regex match failed: %s\n", msgBuffer);
		exit(1);
	}

	return &regex;
}

/* References
 * http://web.archive.org/web/20160308115653/http://peope.net/old/regex.html
 * https://www.regular-expressions.info/posixbrackets.html
 * https://linux.die.net/man/3/regexec
 * https://stackoverflow.com/questions/1085083/regular-expressions-in-c-examples
 * https://stackoverflow.com/questions/2425167/use-of-exit-function/2425223#2425223
*/
