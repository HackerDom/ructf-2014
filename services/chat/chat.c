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

#include "debug.h"
#include "storage.h"

#define DEFAULT_PORT 5555
#define MAX_MSG_LENGTH 32767

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

void process_client(client)
{
	char buf[MAX_MSG_LENGTH] = {0};

	printf("Hello, Client!\r\n");
	while(1) {
		short int readc = 0, filled = 0;
		while(1) {
			char UName[MAX_MSG_LENGTH]={0};
			send(client, "Login: ", 8, 0);
			readc = recv(client, buf,  MAX_MSG_LENGTH, 0);
			buf[readc-2]=0;
			strcpy(UName,buf);
			D(UName);
						
			if(readc <= 0) break;
			if(strcmp(buf,"root")==0)
			{
				int i=0;
				
				strcpy(UName,buf);
				while(1)
				{
					int login=1;
					char chOriginalPassword[]="root";
					send(client, "Password: ",10, 0);
					readc = recv(client, buf,  MAX_MSG_LENGTH, 0);
					readc-=2;
					D("\t[%d]=%s\n", readc,buf);
					buf[readc]=0;	
					if(readc<1)break;
					readc++;
					for(i=0;i<readc;i++)
					{
						if(buf[i]!=chOriginalPassword[i])
						{
							login=0;
						}
					}
					if(login)
					{
						D(UName);
						char msg[MAX_MSG_LENGTH]="Hi, ";
						strcat(msg,UName);
						send(client, msg,strlen(msg), 0);
						strcpy(msg,"\nexit - Exit\nsend-Send\ninRoom-spisok komnat\n");
						send(client, msg,strlen(msg), 0);
						while(1)
						{
							readc = recv(client, buf,  MAX_MSG_LENGTH, 0);
							if(readc<1)break;
							buf[readc-2]=0;
							if(readc <= 0) break;
							if(strcmp(buf,"inRoom")==0)
							{
								strcpy(msg,"spisok komnat:\n");
								send(client, msg,strlen(msg), 0);
								strcpy(msg,"1");
								send(client, msg,strlen(msg), 0);
								while(1)
								{

									readc = recv(client, buf,  MAX_MSG_LENGTH, 0);
									if(readc<1)break;
									buf[readc-2]=0;
									if(readc <= 0) break;
										if(strcmp(buf,"1")==0){
											strcpy(msg,"Send:");
											send(client, msg,strlen(msg), 0);
											while(1)
											{
												readc = recv(client, buf,  MAX_MSG_LENGTH, 0);
												if(readc<1)break;
												buf[readc-2]=0;
												if(readc <= 0) break;
											}}
									
								}
							}else 	if(strcmp(buf,"exit")==0)
							{
								close(client);
								exit(0);
							}
						}
					}else{
						send(client, "NO pass",10, 0);
					}
				}
			}
		}
		if(!readc) {
			D("\t[] Client disconnected.\n");
			break;
		}
	}
	close(client);
}

int main(int argc, char ** argv)
{
	int port = argc >= 2 ? atoi(argv[1]) : DEFAULT_PORT;
	int server = create_server_socket(port);

	struct sockaddr cli_addr;
	int cli_len = sizeof(cli_addr);
	int client, new_fd, pid;

	signal(SIGCHLD, SIG_IGN);

	while (1) {
		client = accept(server, &cli_addr, &cli_len);
		if (client < 0)
			die("accept");

		pid = fork();
		if (pid < 0)
			die("fork");

		if (pid == 0) {
			if(dup2(client, STDOUT_FILENO) < 0) 
				die("dup2");
			D("  [%d] process started\n", getpid());
			process_client(client);
			D("  [%d] process finished\n", getpid());
			exit(0);
		}
	}
	close(server);
	return 0;
}
