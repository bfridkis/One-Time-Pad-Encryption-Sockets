/*********************************************************************************
** Program name: otp_dec_d.c
** Class name: CS344
** Author: Ben Fridkis
** Date: 11/21/2018
** Description: Daemon-like process that will accept incoming connections
**				for requests to decode text using a "one-time-pad" 
**				decryption method. (See https://en.wikipedia.org/wiki/One-time_pad
**				for additional details.) Only compatible with companion client
**				"otp_dec". See otp_dec.c for details.
**********************************************************************************/

#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "registerSigChldHandler.h"
#include "regexGenerator.h"

void error(const char *msg) { perror(msg); exit(1); } //Error function used for reporting issues

int main(int argc, char* argv[]){

	//Function to register a SIGCHLD handler, such that any terminating
	//child is automatically "reaped". See registerSigChldHandler.h for more details.
	registerSigChldHandler();

	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, charsSent;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in serverAddress, clientAddress;

	if(argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } //Check usage & args
	//Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); //Clear out the address struct
	portNumber = atoi(argv[1]); //Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; //Create a network-capable socket
	serverAddress.sin_port = portNumber; //Store the port number
	//serverAddress.sin_addr.s_addr = INADDR_ANY; //Any address is allowed for connection to this process
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Only allow incoming connections from localhost

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if(listenSocketFD < 0) error("ERROR opening socket ");

	//Set socket options
	//Specifically, set socket up to reuse port number 
	//See https://beej.us/guide/bgnet/html/multi/clientserver.html#simpleserver
	//int yes = 1;
	//if(setsockopt(listenSocketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 

	//Enable the socket to begin listening
	//Connect socket to port
	if(bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
		error("ERROR on binding "); 
	listen(listenSocketFD, 5); //Flip the socket on - it can now receive up to 5 connections
	
	while(1){

		//Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); //Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); 
		if (establishedConnectionFD < 0) error("ERROR on accept ");
		
		if(fork() == 0){
			
			//In child, verify that incoming request is from otp_dec_d, which
			//sends a preliminary code of "dec" as an identifier. If the request
			//does not contain this code (i.e. is not from otp_dec, send an 
			//error message to stderr and terminate.
			char validate[4];
			memset(validate, '\0', 4);
			if((charsRead = recv(establishedConnectionFD, validate, 3, 0)) < 0){
				error("ERROR: Unable to read data from socket ");
			}
			if(strcmp(validate, "dec") != 0){
				if((charsSent = send(establishedConnectionFD, "dec NO ACK", 10, 0)) < 0){
					error("ERROR: Unable to send dec NO ACK ");
				}
				write(STDERR_FILENO, "ERROR: Must send decryption request from otp_dec\n", 49);
				exit(1);
			}
			if((charsSent = send(establishedConnectionFD, "dec ACK", 7, 0)) != 7){
				error("ERROR: Unable to send dec ACK ");
			}

			//Strings to hold buffers for decrypted text, key text, and the total character count
			//for incoming data.
			char etBuffer[513], kBuffer[513], tccs[33];

			//otp_dec next sends the total character count for the pending data.
			//Store this value in tccs, and send client an acknowledgment that
			//the count has been received (or print error and exit if unable to 
			//receive).
			memset(tccs, '\0', 33);
			if((charsRead = recv(establishedConnectionFD, tccs, 32, 0)) < 0){
				error("ERROR: Unable to determine incoming character count ");
			}
			else if((charsSent = send(establishedConnectionFD, "ch ct ACK", 9, 0)) < 0){
				error("ERROR: Unable to send ch ct ACK ");
			}
			int tcc = atoi(tccs);
		
			//Determine total number of recv operations needed, based on a max
			//char input of 512 for each recv and tcc determined above
			int numRecvs = tcc / 512;
			if(tcc % 512 > 0){numRecvs++;}

			//Recv encrypted data followed by key text data. Encrypt and send back
			//to otp_dec. If bad data is detected, print error message and exit.
			for(int i = 0; i < numRecvs; i++){
				
				//Receive up to 512 characters of encrypted text, store in etBuffer.
				memset(etBuffer, '\0', 513);
				if((charsRead = recv(establishedConnectionFD, etBuffer, 512, 0)) < 0){
					error("ERROR: Unable to read encrypted data ");
				}
				
				//Compile a regular expression that will match any character
				//that is not a space or uppercase alpha character.
				//(See regexGenerator.c for additional details.)
				regex_t* regex = regexGenerator("[^[:upper:] ]");
				
				//Check for invalid encrypted character using regular expression compiled above.
				//Print error and exit if an invalid character is detected.
				if(regexec(regex, etBuffer, 0, NULL, 0) == 0){
					fprintf(stderr, "ERROR: Invalid character(s) in encrypted file\n");
					if((charsSent = send(establishedConnectionFD, "bad ch", 6, 0)) < 0){
					error("ERROR: Unable to send bad ch for encrypted segment receipt ");
				}
					exit(1);
				}
				
				//Acknowledge receipt of encrypted data segment
				if((charsSent = send(establishedConnectionFD, "et recvd", 8, 0)) < 0){
					error("ERROR: Unable to send ack for encrypted segment receipt ");
				}

				//Receive the number of key characters equal to the number
				//of encrypted characters read above.
				memset(kBuffer, '\0', 513);
				if((charsRead = recv(establishedConnectionFD, kBuffer, charsRead, 0)) < 0){
					error("ERROR: Unable to read key data ");
				}
				
				//Check for invalid key character using regular expression compiled above.
				//Print error and exit if an invalid character is detected.
				if(regexec(regex, kBuffer, 0, NULL, 0) == 0){
					fprintf(stderr, "ERROR: Invalid character(s) in key file\n");
					if((charsSent = send(establishedConnectionFD, "bad ch", 8, 0)) < 0){
					error("ERROR: Unable to send bad ch for key data segment receipt ");
					}
					exit(1);
				}
				
				//Acknowledge receipt of key data segment.
				if((charsSent = send(establishedConnectionFD, "kd recvd", 8, 0)) < 0){
					error("ERROR: Unable to send ack for key data segment receipt ");
				}
				
				/******************************************************
				 *** Alternate means of checking for bad input data ***
				 ******************************************************
				//Check for invalid character(s) in encrypted or key data 
				//from client, and exit if present.
				for(int j = 0; j < strlen(ptBuffer); j++){
					if((ptBuffer[j] < 65 || ptBuffer[j] > 90) && ptBuffer[j] != 32){
						write(STDERR_FILENO, "ERROR: Bad encrypted data\n", 26); 
						exit(1);
					}
				}
				for(int j = 0; j < strlen(kBuffer); j++){
					if((kBuffer[j] < 65 || kBuffer[j] > 90) && kBuffer[j] != 32){
						write(STDERR_FILENO, "ERROR: Bad key data\n", 20); 
						exit(1);
					}
				}
				**********************************************************/
				
				memset(validate, '\0', 4);
				if((charsRead = recv(establishedConnectionFD, validate, 3, 0)) != 3){
					write(STDOUT_FILENO, "ERROR: No receipt message from client\n", 38); 
					exit(1);
				}

				//Encrypt the buffer. (Note the renaming of etBuffer to decBuffer during this
				//process isn't strictly necessary, but is used to provide a more accurate
				//nomenclature, as by the time of sending, the buffer is no longer encrypted,
				//since it has been decrypted.)
				char *decBuffer = etBuffer;
				for(int j = 0; j < strlen(etBuffer); j++){
					
					//If the encrypted character or key character is a space, convert it 
					//to the value that immediately follows "Z" in the ASCII table, so the 
					//decryption formula below will still be valid for it.
					if(etBuffer[j] == 32){decBuffer[j] = 91;}
					if(kBuffer[j] == 32){kBuffer[j] = 91;}

					if((decBuffer[j] = (decBuffer[j] - 65) - (kBuffer[j] - 65)) < 0){
						decBuffer[j] += 27;
					}

					//If value represents 27th character (i.e. space), convert back to ASCII 
					//space character (32). Otherwise, add 65 to revert back to ASCII character 
					//in range {A-Z}
					if(decBuffer[j] == 26){
						decBuffer[j] = 32;
					}
					else{
						decBuffer[j] += 65;
					}
				}
				
				//Send back to otp_dec
				if((charsSent = send(establishedConnectionFD, decBuffer, strlen(decBuffer), 0)) < 0){
					error("ERROR: Unable to send decrypted block ");
				}
			}
			close(establishedConnectionFD);
			exit(0);
		}
	}
	close(listenSocketFD);
	return 0;
}

/* References
 * https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming
 * https://beej.us/guide/bgnet/html/multi/sockaddr_inman.html
 * https://beej.us/guide/bgnet/html/multi/getnameinfoman.html
 * https://stackoverflow.com/questions/7021725/how-to-convert-a-string-to-integer-in-c
 * https://stackoverflow.com/questions/6121094/how-do-i-run-a-program-with-commandline-args-using-gdb-within-a-bash-script
 * https://linux.die.net/man/3/inet_addr
 * http://man7.org/linux/man-pages/man2/sigaction.2.html
 * https://unix.stackexchange.com/questions/56988/how-can-i-find-which-process-sends-data-to-a-specific-port
*/
