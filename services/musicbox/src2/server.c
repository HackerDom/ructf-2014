#include "server.h"
#include "networking.h"
#include "metadata.h"
#include "common.h"

void mb_send_response(int sockfd, char response_type) {
	send(sockfd, (void *)&response_type, sizeof(response_type), 0);
}

int mb_set_ttl(uint8_t *data, int data_size, int ttl) {
	struct Tag ttl_tag;
	char ttl_text[16];
	sprintf(ttl_text, "%d", ttl);
	mb_tag_init("TTL", ttl_text, &ttl_tag);
	return mb_set_metadata(data, data_size, data, &ttl_tag, 1);
}

void mb_process_put(int sockfd, struct Store *store) {
	uint8_t buffer[MAX_FILE];
	uint8_t *ptr = buffer;
	uint16_t ttl;
	uuid_t id;

	int bytes_read;

	if (recv(sockfd, &ttl, sizeof(ttl), 0) != sizeof(ttl))
		error("Failed to receive TTL value");
	do {
		bytes_read = mb_receive_all_bytes(sockfd, ptr, buffer + sizeof(buffer) - ptr);
		if (bytes_read < 0) {
			mb_send_response(sockfd, MB_RESPONSE_ERROR);
			return;
		}
		ptr += bytes_read;
	} while (bytes_read != 0);
	if (mb_set_ttl(buffer, ptr - buffer, ttl) < 0) {
		mb_send_response(sockfd, MB_RESPONSE_ERROR);
		return;
	}
	mb_store_put(store, id, buffer, ptr - buffer);
	mb_send_response(sockfd, MB_RESPONSE_SUCCESS);
	send(sockfd, (void *)&id, sizeof(id), 0);
}

void mb_process_get(int sockfd, struct Store *store) {
	uint8_t buffer[MAX_FILE];
	uint8_t *ptr = buffer;
	int data_length, bytes_sent;
	uuid_t id;

	if (recv(sockfd, &id, sizeof(id), 0) != sizeof(id))
		error("Failed to receive file ID");

	data_length = mb_store_get(store, id, buffer, sizeof(buffer));
	if (data_length < 0) {
		mb_send_response(sockfd, MB_RESPONSE_ERROR);
		return;
	}
	mb_send_response(sockfd, MB_RESPONSE_SUCCESS);
	
	while (buffer + data_length - ptr > 0) {
		bytes_sent = mb_send_all_bytes(sockfd, ptr, buffer + data_length - ptr);
		ptr += bytes_sent;
	}
	mb_send_all_bytes(sockfd, NULL, 0);
}

void mb_process_client(int sockfd, struct Store *store) {
	char request_type;

	if (recv(sockfd, &request_type, sizeof(request_type), 0) != sizeof(request_type))
		error("Failed to receive request type");

	switch (request_type)
	{
	case MB_REQUEST_PUT:
		mb_process_put(sockfd, store);
		break;
	case MB_REQUEST_GET:
		mb_process_get(sockfd, store);
		break;
	default:
		mb_send_response(sockfd, MB_RESPONSE_ERROR);
	}
}

void mb_start_server(int port, struct Store *store) {
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

	listen(listener, SOMAXCONN);

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
						sizeof(timeout)) < 0)
				error("setsockopt failed on SO_RCVTIMEO");

			if (setsockopt (newsockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
						sizeof(timeout)) < 0)
				error("setsockopt failed on SO_SNDTIMEO");

			mb_process_client(newsockfd, store);
			exit(0);
		}
		else
		{
			close(newsockfd);
		}
	}
}