#include "server.h"

void mb_start_server(int port) {
	int listener, newsockfd;
	int yes = 1, pid;
	struct sockaddr_in servAddr, clientAddr;
	socklen_t clientAddrSize = sizeof(clientAddr);
	struct timeval timeout;

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0)
		error("Error opening listener socket");

	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		error("setsockopt failed on SO_REUSEADDR");

	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.sin_port = htons(port);

	if (bind(listener, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
		error("Error on binding");

	listen(listener, 5);

	while (1)
	{
		newsockfd = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrSize);
		if (newsockfd < 0)
			error("Error on accept");

		pid = fork();
		if (pid < 0)
			error("Error on fork");
		if (pid == 0)
		{
			close(listener);

			if (setsockopt (newsockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
						sizeof(timeout)) < 0)l
				error("setsockopt failed on SO_RCVTIMEO");

			if (setsockopt (newsockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
						sizeof(timeout)) < 0)
				error("setsockopt failed on SO_SNDTIMEO");
			
			mb_process_client(newsockfd);
			exit(0);
		}
		else
		{
			close(newsockfd);
		}
	}
}

void mb_process_client(int sockfd) {
	enum MBRequestType request_type;
	enum MBResponseType response_type;
	char *data = NULL;
	int data_length;

	if (recv(sockfd, &request_type, sizeof(MBRequestType)) != sizeof(MBRequestType))
		error("Failed to retrieve request type");

	if (request_type == MB_REQUEST_PUT) {

	}
	else if (request_type == MB_REQUEST_GET) {

	}
	else {
		response_type = MB_RESPONSE_UNKNOWN_REQUEST_TYPE_ERROR;
	}

	send(sockfd, (void *)&response_type, sizeof(MBResponseType), 0);
	if (data != NULL)
		send(sockfd, data, data_length, 0);
}