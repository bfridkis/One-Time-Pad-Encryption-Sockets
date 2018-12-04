/*************************************************
** Program name: regexGenerator.h
** Class name: CS344
** Author: Ben Fridkis
** Date: 11/26/2018
** Description: Header file for regexGenerator.c
*************************************************/

#ifndef regexGenerator_h
#define regexGenerator_h

#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

regex_t* regexGenerator(char* regexSpecifier);

#endif /* regexGenerator_h */
