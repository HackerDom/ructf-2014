#include "networking.h"
#include "logging.h"

int mb_send_all_bytes(int sockfd, uint8_t *buffer, int desired_length) {
	short length = min(desired_length, MAX_CHUNK);
	size_t bytes_sent, total_bytes_sent;

	send(sockfd, (void *)&length, sizeof(length), 0);

	total_bytes_sent = 0;
	while (total_bytes_sent < length) {
		bytes_sent = send(sockfd, buffer + total_bytes_sent, length - total_bytes_sent, 0);
		if (bytes_sent <= 0)
			return -1;
		total_bytes_sent += bytes_sent;
	}

	return total_bytes_sent;
}

int mb_receive_all_bytes(int sockfd, uint8_t *file_buffer, int capacity) {
	short length;
	size_t bytes_read, total_bytes_read;
	uint8_t buffer[MAX_CHUNK];

	if (recv(sockfd, &length, sizeof(length), 0) != sizeof(length))
		error("Failed to receive chunk length");
	if (length > capacity)
		return -1;

	total_bytes_read = 0;
	while (total_bytes_read < length) {
		bytes_read = recv(sockfd, buffer + total_bytes_read, length - total_bytes_read, 0);
		if (bytes_read <= 0)
			return -1;
		total_bytes_read += bytes_read;
	}

	memcpy(file_buffer, buffer, total_bytes_read);

	return total_bytes_read;
}