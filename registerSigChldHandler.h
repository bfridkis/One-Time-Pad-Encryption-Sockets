/***************************************************
** Name: registerSigChldHandler.h
** Class name: CS344
** Author: Ben Fridkis
** Date: 11/21/2018
** Description: Header to registerSigChldHandler.c.
****************************************************/

#ifndef registerSigChldHandler_h
#define registerSigChldHandler_h

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

void sigchld_handler(int s);
void registerSigChldHandler();

#endif /* registerSigChldHandler_h */
