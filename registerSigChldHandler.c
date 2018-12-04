/*******************************************************************************
** Program name: registerSigChldHander.c
** Class name: CS344
** Author: Ben Fridkis
** Date: 11/21/2018
** Description: Functions to establish a handler for SIGCHLD signals that will
**				automatically reap "defunct" (zombie) processes when child
**				processes terminate. This is taken directly from:
** 
**	  https://beej.us/guide/bgnet/html/multi/clientserver.html#simpleserver
**
*******************************************************************************/

#include "registerSigChldHandler.h"

void sigchld_handler(int s)
{
	   //waitpid() might overwrite errno, so we save and restore it:
       int saved_errno = errno;
	
	   while(waitpid(-1, NULL, WNOHANG) > 0);
		
	   errno = saved_errno;
}

void registerSigChldHandler(){
	//Register handler for SIGCHLD signal, which is raised when child
	//process terminates (SA_NOCLDSTOP suppresses signal when child 
	//process stops or continues.)
	//See references 23 and 24 for complete details.
	struct sigaction sa;
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
	     perror("sigaction");
	     exit(1);
	}
}

/* References
 * http://man7.org/linux/man-pages/man2/sigaction.2.html
 * http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
 * https://beej.us/guide/bgnet/html/multi/clientserver.html#simpleserver
 * https://support.sas.com/documentation/onlinedoc/sasc/doc/lr2/waitpid.html
 * http://pubs.opengroup.org/onlinepubs/009604499/functions/sigaction.html
*/
