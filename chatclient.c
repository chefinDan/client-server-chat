/******************************************************************************
** Author: Daniel Green, greendan@oregonstate.edu
** Description: starts a client program that sends plaintext data to otp_enc_d
******************************************************************************/
#define _POSIX_C_SOURCE 1
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <math.h>
#define MAX 1024
#define MAX_STATUS 8
#define HANDLE_LEN 10
#define h_addr h_addr_list[0] /* for backward compatibility */

// struct message{
// 	struct stat statbuf;
// 	int chrCnt;
// 	int fd;
// 	char *buffer;
// };


void error(const char *msg) { perror(msg);} // Error function used for reporting issues


// int getMessage(char *file, struct message* msg){
// 	int tmpchar = 0;
// 	int charCnt = 0;
//
// 	FILE *fp = fopen(file, "r");
// 	if(fp == NULL) { error("OTP_ENC: ERROR, cannot open file.\nUsage: otp_enc <plaintext> <key> <port>"); exit(1); }
//
// 	info->fd = fileno(fp);
// 	fstat(fileno(fp), &info->statbuf);
// 	printf("")
// 	while((tmpchar = fgetc(fp))){
// 		if((tmpchar >= 65 && tmpchar <= 90) || tmpchar == 32){
// 			charCnt++;
// 		}
// 		else if(tmpchar == '\n'){
// 			info->chrCnt = charCnt;
// 			break;
// 		}
// 		else{
// 			return -1;
// 		}
// 	}
	// info->buffer = malloc(sizeof(char) * info->chrCnt);
	// memset(info->buffer, '\0', info->chrCnt);
//
// 	rewind(fp);
// 	fgets(info->buffer, info->chrCnt+1, fp);
// 	fclose(fp);
// 	return charCnt;
// }

// function for recving a small number indicating the total size of the coming
// data file.
int recvDataSize(int sockfd){
	char buffer[MAX_STATUS];
	int charsRead, charsSent, n;
	memset(buffer, '\0', MAX_STATUS);

	n = recv(sockfd, buffer, MAX_STATUS, 0);
	if(n < 0){ error("SERVER: cannot recieve data size"); exit(1); }
	send(sockfd, "200", MAX_STATUS, 0);
	return (int)strtol(buffer, NULL, 10);
}


int sendDataSize(int sockfd, char* data){
	int n, len;
	char sizeBuf[MAX_STATUS];
	memset(sizeBuf, '\0', MAX_STATUS);

	sprintf(sizeBuf, "%ld", strlen(data));

	n = send(sockfd, sizeBuf, MAX_STATUS, 0);
	if (n < 0) { error("CLIENT: ERROR in sendDataSize(), writing to socket"); return 1; }

	memset(sizeBuf, '\0', MAX_STATUS);
	n = recv(sockfd, sizeBuf, MAX_STATUS, 0);
	if (n < 0) { error("CLIENT: ERROR in sendDataSize(), reading from socket"); return 1; }

	return 0;
}

int sendData(int sockfd, char* data){
	int charsWritten, charsRead, charsRemain, n;
	char status[MAX_STATUS];
	char* buffer = NULL;

	// sendDataSize(sockfd, data);

	charsRemain = strlen(data);
	charsWritten = 0;
	buffer = malloc(sizeof(char) * (size_t)strlen(data));
	memset(buffer, '\0', strlen(data));

	while(charsWritten < strlen(data)){
		n = send(sockfd, buffer + charsWritten, charsRemain, 0); // Write to the server
		if (n < 0) { error("CLIENT: ERROR writing to socket"); return 1; }
		charsWritten+=n;
		charsRemain-=n;
	}

	// Get return message from server
	charsRead = recv(sockfd, status, MAX_STATUS, 0); // Read data from the socket
	if (charsRead < 0) { error("CLIENT: ERROR reading from socket"); return 1; }
	return 0;
}

int recvData(int sockfd, char* data){
	int charsRead, charsSent, charsExpected, charsRemain, n;
	char* buffer = NULL;

	charsRead = 0;
	charsExpected = recvDataSize(sockfd);
	if(charsExpected == 0){ return 0; }
	charsRemain = charsExpected;

	buffer = malloc(sizeof(char) * (size_t)strlen(data));
	memset(buffer, '\0', strlen(data));

	while(charsRead < charsExpected){
		n = recv(sockfd, buffer + charsRead, charsRemain, 0);
		if (charsRead < 0) { error("CLIENT: ERROR reading from socket"); }
		charsRead+=n;
		charsRemain-=n;
	}
	buffer[charsExpected] = '\0';

	// Send a Success message back to the server
	n = send(sockfd, "200", MAX_STATUS, 0); // Send success back
	if (n < 0) { error("ERROR writing to socket"); }

	return 1;
}


int main(int argc, char *argv[])
{
	int socketFD, port, mCnt, keyCnt, nread;
	size_t maxHandleLen = HANDLE_LEN;
	char handle[10];
	char* message = NULL;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

	if(argc < 3){ error("CLIENT: ERROR, usage: chatclient <server_hostname> <server_port>"); exit(1); }
	// printf("here");
	// mCnt = fileCheck(argv[1], &srcinfo); // open the srctext file and store info in struct srcinfo
	// keyCnt = fileCheck(argv[2], &keyinfo); // open keytext file and store info in struct keyinfo

	// if(mCnt < 0){ error("OTP_ENC: ERROR, plaintext file contains invalid character(s)"); exit(1); }
	// if(keyCnt < 0) {error("OTP_ENC: ERROR, key file contains invalid character(s)"); exit(1); }
	// if(keyCnt < mCnt) { error("OTP_ENC: ERROR, key file contains fewer characters than plaintext file"); exit(1); }
	// encinfo.chrCnt = mCnt;
	// encinfo.buffer = (char*)malloc(mCnt);

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	port = atoi(argv[2]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(port); // Store the port number
	serverHostInfo = gethostbyname(argv[1]); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) {error("CLIENT: ERROR opening socket"); exit(1); }

	// Connect to server

	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){ // Connect socket to address
		error("CLIENT: ERROR connecting"); exit(1);
	}

	// Get users handle
	printf("Enter a handle that you would like to be identified by: ");
	fgets(handle, maxHandleLen, stdin);
	handle[strlen(handle) -1] = '\0';
	printf("Hello %s, you may begin chatting with Hal\n", handle);
	printf("%s> ", handle);

	// while ((nread = getline(handle, &maxHandleLen, stdin)) != -1) {
  // 	printf("Retrieved line of length %zu:\n", (size_t)nread);
  // }

	// message = "Hello from the client";
	//
	// if( sendData(socketFD, &srcinfo) == -1 ){ error("CLIENT: ERROR, did not send all data"); exit(1); }
	// if( sendData(socketFD, &keyinfo) == -1 ){ error("CLIENT: ERROR, did not send all data"); exit(1); }
	// if( recvData(socketFD, &encinfo) == -1 ){ error("CLIENT: ERROR, did not recv all data"); exit(1); }
	//
	// close(socketFD);
	//
	// printf("%s\n", encinfo.buffer);
	// free(srcinfo.buffer);
	// free(keyinfo.buffer);

	 // Close the socket
	return 0;
}
