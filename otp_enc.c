/*********************************************************************************
** Program name: otp_enc.c
** Class name: CS344
** Author: Ben Fridkis
** Date: 11/21/2018
** Description: Client program for otp_enc_d. Initiates requests to otp_enc_d to
**				perform a one-time-pad encryption on the data sent from file
**				specified by argv[1] provided key text specified in argv[2].
**				For additional information concerning the  "one-time-pad" 
**				encryption method, see https://en.wikipedia.org/wiki/One-time_pad
**				for additional details. Only compatible with otp_enc_d server.
**				See otp_enc_d.c for details.
**********************************************************************************/

#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "registerSigChldHandler.h"
#include "regexGenerator.h"
#include "validateFiles.h"
#include "fsize.h"

//Error function used for reporting non-connection related issues
void error(const char *msg) { perror(msg); exit(1); } 

//Error function used for reporting connection issues
void cError(int port) { fprintf(stderr, "OTC_ENC: Connection error on port: %d\n", port); exit(2); }

int main(int argc, char* argv[]){

	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	//struct hostent* serverHostInfo;
	char ptBuffer[513], kBuffer[513];

	//Check usage and args
	if (argc < 4) { 
		fprintf(stderr,"USAGE: %s port [plaintext filename] "
						"[key filename] port\n", argv[0]);
		exit(1); 
	}

	//Compare key file size to plaintext file size. If key is smaller
	//print error and exit.
	if(fsize(argv[1]) > fsize(argv[2])){
		fprintf(stderr, "OTP_ENC: Key file size < plaintext file size\n");
		exit(1);
	}

	//Open both files (plaintext and key).
	//Print error message and exit if either cannot be opened.
	int pt;
	if((pt = open(argv[1], O_RDONLY)) == -1){
		error("OTP_ENC: Could not open plaintext file ");
	}
	int k;
	if((k = open(argv[2], O_RDONLY)) == -1){
		error("OTP_ENC: Could not open key file ");
	}

	//Check files for invalid characters
	//(See validateFiles.c for more details)
	validateFiles(pt, k, 1);

	//Set up the address struct for this process
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); //Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; //Create a network-capable socket
	serverAddress.sin_port = portNumber; //Store the port number
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); //Server address is localhost
	//serverHostInfo = gethostbyname("localhost"); //Convert the machine name into a special form of address
	//memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); //Copy in the address

	//Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) cError(portNumber);

	//Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		cError(portNumber);
	
	//Send client-type identifier
	if((charsWritten = send(socketFD, "enc", 3, 0)) < 0){
		error("OTP_ENC: Error writing to socket ");
	}
	
	//Receive acknowledgement from server, error if "enc NO ACK" is sent
	char ackResponse[11];
	memset(ackResponse, '\0', 11);
	if((charsRead = recv(socketFD, ackResponse, 10, 0)) < 0){
		error("OTP_ENC: Unable to read ACK response from server ");
	}
	if(strcmp(ackResponse, "enc ACK") != 0){
		fprintf(stderr, "OTP_ENC: Unable to connect to server - bad server/client combo\n");
		exit(2);
	}
	
	//Convert the plaintext file size to a string and send to server.
	int fileLengthStringSize = snprintf(NULL, 0, "%d", (int)fsize(argv[1]));
	char* fileSizeString = malloc(fileLengthStringSize + 1);
	snprintf(fileSizeString, fileLengthStringSize + 1, "%d", (int)fsize(argv[1]));
	if((charsWritten = send(socketFD, fileSizeString, strlen(fileSizeString), 0)) < 0){
		error("OTP_ENC: Unable to send file size ");
	}

	//Receive server's acknowledgement that the file size message has been
	//received.
	memset(ackResponse, '\0', 11);
	if((charsRead = recv(socketFD, ackResponse, 9, 0)) != 9){
		error("OTP_ENC: Server unable to acknowledge plain text character count ");
	}

	//Rewind file pointer offsets (for the plaintext and key files) in preparation
	//for data send operations.
	if(lseek(pt, 0, SEEK_SET) == -1){
		error("OTP_ENC: Unable to rewind plain text file for send operations ");
	}
	if(lseek(k, 0, SEEK_SET) == -1){
		error("OTP_ENC: Unable to rewind key file for send operations ");
	}
	
	//This final while loop handles the main data processing, sending segements
	//of 512 bytes for first the plain text file and then the key file, then 
	//receiving and printing to stdout the encrypted result from the sever.
	int segmentSize;
	char response[513];
	while(memset(ptBuffer, '\0', sizeof(ptBuffer)) &&
			(segmentSize = read(pt, ptBuffer, sizeof(ptBuffer) - 1)) != 0){
		if(segmentSize == -1){
			error("OTP_ENC: Unable to read plain text file for send operation ");
		}
		//Do not send trailing newline if present (only applies for final segment).
		if(ptBuffer[strlen(ptBuffer) - 1] == '\n'){
			segmentSize--;
		}

		//Send plain text, up to sizeof(ptBuffer) - 1 bytes
		if((charsWritten = send(socketFD, ptBuffer, segmentSize, 0)) < 0){
			error("OTP_ENC: Unable to send plain text data segment ");
		}
		
		//Receive ack from server that plain text bytes have been received
		memset(ackResponse, '\0', 11);
		if((charsRead = recv(socketFD, ackResponse, 8, 0)) < 0){
			error("OTP_ENC: No ACK from server after sending plain text data segment ");
		}
		if(strcmp(ackResponse, "bad ch") == 0){
			fprintf(stderr, "OTP_ENC: Bad character sent in plaintext segment\n");
			exit(1);
		}
		
		if((read(k, kBuffer, segmentSize)) != segmentSize){ 
			error("OTP_ENC: Unable to read key file for segment corresponding to plain text ");
		}
		//Send key data, up to size of (ptBuffer) - 1 bytes, corresponding to plain text 
		//data previously sent
		if((charsWritten = send(socketFD, kBuffer, segmentSize, 0)) < 0){
			error("OTP_ENC: Unable to send key data segment ");
		}
		//Receive ack from server that key data bytes have been received
		memset(ackResponse, '\0', 11);
		if((charsRead = recv(socketFD, ackResponse, 8, 0)) < 0){
			error("OTP_ENC: No ACK from server after sending key data segment ");
		}
		if(strcmp(ackResponse, "bad ch") == 0){
			fprintf(stderr, "OTP_ENC: Bad character sent in key segment\n");
			exit(1);
		}
		
		//Send ready message to server, then receive result segment
		if((charsWritten = send(socketFD, "rdy", 3, 0)) < 0){
			error("OTP_ENC: Unable to send receipt ready message to server ");
		}
		memset(response, '\0', sizeof(response));
		if((charsRead = recv(socketFD, response, sizeof(response) - 1, 0)) < 0){
			error("OTP_ENC: Unable to receive encrypted data segment from server ");
		}

		//Write result segment to stdout
		write(STDOUT_FILENO, response, charsRead); 
	}
	
	//Close the socket and print a trailing newline to stdout
	close(socketFD);
	write(STDOUT_FILENO, "\n", 1);

	return 0;
}	


/* References
 * https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming
 * https://beej.us/guide/bgnet/html/multi/sockaddr_inman.html
 * https://beej.us/guide/bgnet/html/multi/getnameinfoman.html
 * https://stackoverflow.com/questions/7021725/how-to-convert-a-string-to-integer-in-c
 * https://stackoverflow.com/questions/6121094/how-do-i-run-a-program-with-commandline-args-using-gdb-within-a-bash-script
 * https://www.tutorialspoint.com/c_standard_library/c_function_strstr.html
*/
