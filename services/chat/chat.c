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

#include "db.h"
#include "io.h"
#include "debug.h"
#include "storage.h"

#define MAX_ARGV          5
#define LEN_ARG         128
#define ROOMS_MAX     65536
#define DEFAULT_PORT   5555
#define BUF_SIZE      32767

#define die(message) { perror(message); exit(1); }


int create_server_socket (int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) 
		die("create socket");

	int optval = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	struct sockaddr_in name;
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = INADDR_ANY;
	name.sin_port = htons(port);

	if (bind(sock, (void*) &name, sizeof(name)) < 0)
		die("bind");

	if (listen(sock, 1) < 0)
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

/* Command: \register name pass */

void cmd_register(char *buf)
{
char *cmd,*user,*pass;
 char space[10]=" ";
 cmd= strtok (buf,space);
 user = strtok (NULL,space);
 pass = strtok (NULL,space);
user_create(user,pass);
}

void cmd_login(char *buf)
{
	char *cmd,*user,*pass;
	char space[10]=" ";
	cmd= strtok (buf,space);
	user = strtok (NULL,space);
	pass = strtok (NULL,space);
	user_login(user,pass);
}

void cmd_login_room(char *buf)
{
        char *cmd,*room,*pass;
        char space[10]=" ";
        cmd= strtok (buf,space);
        room = strtok (NULL,space);
        pass = strtok (NULL,space);
        room_join(room, pass);
        list_room();
}

void cmd_creat_room(char *buf)
{
	 char *cmd,*name,*pass;
        char space[10]=" ";
        cmd= strtok (buf,space);
        name = strtok (NULL,space);
        pass = strtok (NULL,space);
	room_create(name, currentUser, pass);
}

void cmd_say(char *buf)
{
        char *cmd,*msg;
        char space[10]=" ";
        cmd= strtok (buf,space);
        msg = strtok (NULL,space);
        say(msg);
}


void cmd_list()
{
	list();
}

void cmd_help()
{
	WriteLn("\\register <name> <pass> - register new user");
	WriteLn("\\login <name> <pass>    - login");
	WriteLn("\\create <room> [pass]   - create new room (with optional password)");
	WriteLn("\\list                   - list all chat rooms");
	WriteLn("\\join <room>            - join chat room by name");
	WriteLn("\\leave                  - leave current chat room");
	WriteLn("\\quit                   - quit program");
}

char* allocate_argv(int count, int length)
{
	// TODO
	return NULL;
}

void free_argv(int count)
{
	// TODO
}

void process_client()
{
	int argc;
	char * argv[MAX_ARGV];
	char buf[BUF_SIZE] = {0};

	print_greeting();

	while (1)
	{
		Write("> ");
		if (!fgets(buf, BUF_SIZE, stdin))
			break;
		strtok(buf, "\r\n");

		if (buf == strstr(buf, "\\quit")) {
			break;
		} else if (buf == strstr(buf, "\\register")) {
			cmd_register(buf);
		}
		else if (buf == strstr(buf, "\\login")) {
			cmd_login(buf);
		}
		else if (buf == strstr(buf, "\\help")) {
			cmd_help();
		}
		else if (buf == strstr(buf, "\\list")) {
			cmd_list();
		}
		else if (buf == strstr(buf, "\\create")) {
                        cmd_creat_room(buf);
                } 
		else if (buf == strstr(buf, "\\join")) {
                        cmd_login_room(buf);
                }
		else if (buf == strstr(buf, "\\say")) {
                        cmd_say(buf);
			list_room();

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
	connect_db();
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
