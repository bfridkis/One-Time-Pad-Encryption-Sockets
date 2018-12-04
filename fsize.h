/*********************************************
** Program name: fsize.h
** Class name: CS344
** Author: Ben Fridkis
** Date: 11/21/2018
** Description: Header file for fsize.c
*********************************************/

#ifndef fsize_h
#define fsize_h

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

off_t fsize(const char *filename);

#endif /* fsize_h */