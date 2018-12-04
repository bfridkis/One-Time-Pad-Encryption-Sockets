					     One-Time-Pad-Encryption-Sockets
					
The following 5 utility programs comprising this repository perform the following:

**  1. otp_enc_d: Daemon-like process that will accept incoming connections
**				  for requests to encode text using a "one-time-pad" 
**				  encryption method. (See https://en.wikipedia.org/wiki/One-time_pad
**				  for additional details.) Only compatible with companion client
**				  "otp_enc". See otp_enc.c for details.
**
**  2. otp_enc:   Client program for otp_enc_d. Initiates requests to otp_enc_d to
**				  perform a one-time-pad encryption on the data sent from file
**				  specified by argv[1] provided key text specified in argv[2].
**				  For additional information concerning the  "one-time-pad" 
**				  encryption method, see https://en.wikipedia.org/wiki/One-time_pad
**				  for additional details. Only compatible with otp_enc_d server.
**				  See otp_enc_d.c for details.
**
**	3. otp_dec_d: Daemon-like process that will accept incoming connections
**	   			  for requests to encode text using a "one-time-pad" 
**	   			  encryption method. (See https://en.wikipedia.org/wiki/One-time_pad
**	   			  for additional details.) Only compatible with companion client
**	   			  "otp_enc". See otp_enc.c for details.
**
**  4. otp_dec:   Client program for otp_dec_d. Initiates requests to otp_dec_d to
**	   			  perform a one-time-pad decryption on the data sent from file
**	   			  specified by argv[1] provided key text specified in argv[2].
**	   			  For additional information concerning the  "one-time-pad" 
**	   			  decryption method, see https://en.wikipedia.org/wiki/One-time_pad
**	   			  for additional details. Only compatible with otp_dec_d server.
**	   			  See otp_dec_d.c for details.
**
**	5. keygen:	  Program to generate a key of random characters of the set
**				  {A-Z, " "}, with length specified by the input parameter 
**				  length (argument 2).
**
**  Programs all use special address "localhost" for socket connections. Multiple
**	connections can be handled simultaneously by each daemon-like process (otp_enc_d
**  & otp_dec_d). 
	
	
Getting Started:

1.  Copy all repository files to a directory of your choice.

2.  Type the command "compileall" (without quotes)

3.  Type "otp_enc_d [port_number] &" (without quotes and without brackets, where port_number is
		a port number of your choice, usually in range of 3,000 - 65,535) to launch the encryption daemon-like
		process in the background. 
		
4.  Type "otp_dec_d [port_number] &" (without quotes and without brackets, where port_number is
		a port number of your choice, usually in range of 3,000 - 65,535) to launch the decryption daemon-like
		process in the background.

5.  Generate a key file for [plaintext], where plaintext is a file with data to be encrypted, by typing
		"keygen [length of plaintext] > mykey" (without quotes and without brackets, where length of plaintext 
		is the character count of the plaintext file). 
		
		**PLAINTEXT MUST CONTAIN ONLY UPPER-CASE ENGLISH ALPHA CHARACTERS AND SPACES**
		
6.  To encrypt [plaintext], type "otp_enc [plaintext] mykey [port_number] > my_cipher_text" (without quotes and 
		without brackets, where plaintext is a file with data to be encrypted, and port_number is the same as 
		that chosen in step 3). (The encrypted text will save to the file my_cipher_text located in the current 
		directory.)
		
		**PLAINTEXT MUST CONTAIN ONLY UPPER-CASE ENGLISH ALPHA CHARACTERS AND SPACES**
		
7.  To decrypt my_cipher_text, type "otp_dec my_cipher_text mykey [port_number] > my_decrypted_text" (without quotes and 
		without brackets, where port_number is the same as that chosen in step 4). (The decrypted text will save 
		to the file my_decrypted_text located in the current directory.)
		
8.  When finished, type the command "ps" (wihtout quotes) to see current running processes, then enter the following commands:

		-"kill -TERM [otp_enc_d_PID]" (wihtout quotes and brackets, where otp_enc_d_PID is the process id of otp_enc_d
			listed via the "ps" command).
			
		-"kill -TERM [otp_dec_d_PID]" (wihtout quotes and brackets, where otp_dec_d_PID is the process id of otp_dec_d
			listed via the "ps" command).

9.  Type "make clean" (without quotes) to remove all object and executable files associated with each of the five programs 
		listed above.

