#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>

#include "io.h"
#include "debug.h"
#include "storage.h"

#define DEFAULT_PORT   5555
#define BUF_SIZE      32767

#define die(message) { perror(message); exit(1); }

int create_server_socket (int port)
{
	int sock;
	struct sockaddr_in name;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) 
		die("create socket");

	int optval = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	name.sin_family = AF_INET;
	name.sin_addr.s_addr = INADDR_ANY;
	name.sin_port = htons(port);

	if(bind(sock, (void*) &name, sizeof(name)))
		die("bind");

	if(listen(sock, 1) == -1)
		die("listen");

	D("Listening to port: %d\n", port);
	return sock;
}

void print_greeting() 
{
	WriteLn("   ::: Welcome to RuCTF 2014 passenger chat :::");
	WriteLn("");
	WriteLn("Commands: \\help, \\register, \\login, \\list, \\join <room>, \\leave, \\quit");
	WriteLn("");
}

void print_help()
{
	WriteLn("\\register <name>  - register new user");
	WriteLn("\\login <name>     - login");
	WriteLn("\\list             - list all chat rooms");
	WriteLn("\\join <room>      - join chat room by name");
	WriteLn("\\leave            - leave current chat room");
	WriteLn("\\quit             - quit program");
}

void process_client()
{
	char buf[BUF_SIZE] = {0};

	print_greeting();

	while (1)
	{
		Write("> ");
		if (!fgets(buf, BUF_SIZE, stdin))
			break;
		strtok(buf, "\r");
		strtok(buf, "\n");

		if (0 == strcmp(buf, "\\quit")) {
			break;
		}
		else if (0 == strcmp(buf, "\\help")) {
			print_help();
		}
		else {
			WriteLn("Unknown command (type '\\help' for help)");
		}

		WriteLn("");
	}
}

int main(int argc, char ** argv)
{
	int port = argc >= 2 ? atoi(argv[1]) : DEFAULT_PORT;
	int server = create_server_socket(port);

	signal(SIGCHLD, SIG_IGN);

	while (1) {
		struct sockaddr cli_addr;
		int cli_len = sizeof(cli_addr);
		int client = accept(server, &cli_addr, &cli_len);
		if (client < 0)
			die("accept");

		int pid = fork();
		if (pid < 0)
			die("fork");

		if (pid == 0) {
			if (dup2(client, STDIN_FILENO) < 0) die("dup2 stdin");
			if (dup2(client, STDOUT_FILENO) < 0) die("dup2 stdout");

			D("  [%d] process started\n", getpid());
			process_client();
			D("  [%d] process finished\n", getpid());

			shutdown(client, 2);
			exit(0);
		}
	}
	close(server);
	return 0;
}
