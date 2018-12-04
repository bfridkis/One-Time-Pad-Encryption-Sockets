CFLAGS = -g -Wall -Wfatal-errors -std=gnu99
#CFLAGS = -g -w -std=gnu99
VPATH = cutest

keygen : keygen.o 
	gcc $(CFLAGS) -o $@ $^

keygen.o : keygen.c
	gcc $(CFLAGS) -c -o $@ $<
			
otp_enc_d : otp_enc_d.o registerSigChldHandler.o regexGenerator.o
	gcc $(CFLAGS) -o $@ $^
			
otp_enc_d.o : otp_enc_d.c
	gcc $(CFLAGS) -c -o $@ $<

otp_dec_d : otp_dec_d.o registerSigChldHandler.o regexGenerator.o
	gcc $(CFLAGS) -o $@ $^
			
otp_dec_d.o : otp_dec_d.c
	gcc $(CFLAGS) -c -o $@ $<

registerSigChldHandler.o : registerSigChldHandler.c
	gcc $(CFLAGS) -c -o $@ $<

otp_enc : otp_enc.o regexGenerator.o fize.o validateFiles.o
	gcc $(CFLAGS) -o $@ $^

otp_enc.o : otp_enc.c
	gcc $(CFLAGS) -c -o $@ $<

otp_dec : otp_dec.o regexGenerator.o fize.o validateFiles.o
	gcc $(CFLAGS) -o $@ $^

otp_dec.o : otp_dec.c
	gcc $(CFLAGS) -c -o $@ $<

regexGenerator.o : regexGenerator.c
	gcc $(CFLAGS) -c -o $@ $<

fize.o : fsize.c
	gcc $(CFLAGS) -c -o $@ $<

validateFiles.o : validateFiles.c
	gcc $(CFLAGS) -c -o $@ $<

all : keygen otp_enc_d otp_enc otp_dec_d otp_dec
												
clean :
	-rm *.o
	-rm keygen
	-rm otp_enc_d
	-rm otp_enc
	-rm otp_dec_d
	-rm otp_dec

