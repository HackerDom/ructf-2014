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

#include "storage.h"

#define PORT 5555
#define MAX_MSG_LENGTH 32767
#define END_STRING "chau\n"
#define COMPLETE_STRING "fin-respuesta"

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL SO_NOSIGPIPE
#endif

#define perro(x) {fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, x, strerror(errno));exit(1);}

#define D(...) fprintf(new_stream, __VA_ARGS__)

int main() {
	int sock;
	struct sockaddr_in name;
	char buf[MAX_MSG_LENGTH] = {0};

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) perro("opening socket");

	int optval = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	name.sin_family = AF_INET;
	name.sin_addr.s_addr = INADDR_ANY;
	name.sin_port = htons(PORT);
	if(bind(sock, (void*) &name, sizeof(name))) perro("binding tcp socket");
	if(listen(sock, 1) == -1) perro("listen");

	struct sockaddr cli_addr;
	int cli_len = sizeof(cli_addr);
	int new_socket, new_fd, pid;
	FILE* new_stream;

	if(new_fd = dup(STDERR_FILENO) == -1) perro("dup");
	new_stream = fdopen(new_fd, "w");
	setbuf(new_stream, NULL); // sin buffering

	D("Initializing server...\n");
	while(new_socket = accept(sock, &cli_addr, &cli_len)) {
		D("Client connected.\nForking... ");
		if(pid = fork()) D("child pid = %d.\n", pid);
		else {
			pid = getpid();
			if(new_socket < 0) perro("accept");
			if(dup2(new_socket, STDOUT_FILENO) == -1) perro("dup2");
			if(dup2(new_socket, STDERR_FILENO) == -1) perro("dup2");
			while(1) {
				short int readc = 0, filled = 0;
				while(1) {
					char UName[MAX_MSG_LENGTH]={0};
					send(new_socket, "Login: ", 8, 0);
					readc = recv(new_socket, buf,  MAX_MSG_LENGTH, 0);
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
							send(new_socket, "Password: ",10, 0);
							readc = recv(new_socket, buf,  MAX_MSG_LENGTH, 0);
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
								send(new_socket, msg,strlen(msg), 0);
								strcpy(msg,"\nexit - Exit\nsend-Send\ninRoom-spisok komnat\n");
								send(new_socket, msg,strlen(msg), 0);
								while(1)
								{
									readc = recv(new_socket, buf,  MAX_MSG_LENGTH, 0);
									if(readc<1)break;
									buf[readc-2]=0;
									if(readc <= 0) break;
									if(strcmp(buf,"inRoom")==0)
									{
										strcpy(msg,"spisok komnat:\n");
										send(new_socket, msg,strlen(msg), 0);
										strcpy(msg,"1");
										send(new_socket, msg,strlen(msg), 0);
										while(1)
										{

											readc = recv(new_socket, buf,  MAX_MSG_LENGTH, 0);
											if(readc<1)break;
											buf[readc-2]=0;
											if(readc <= 0) break;
												if(strcmp(buf,"1")==0){
													strcpy(msg,"Send:");
													send(new_socket, msg,strlen(msg), 0);
													while(1)
													{
														readc = recv(new_socket, buf,  MAX_MSG_LENGTH, 0);
														if(readc<1)break;
														buf[readc-2]=0;
														if(readc <= 0) break;
													}}
											
										}
									}else 	if(strcmp(buf,"exit")==0)
									{
										close(new_socket);
										D("\t[%d] Dying.", pid);
										exit(0);
									}
								}
							}else{
								send(new_socket, "NO pass",10, 0);
							}
						}
					}
				}
				if(!readc) {
					D("\t[%d] Client disconnected.\n", pid);
					break;
				}
			}
			close(new_socket);
			D("\t[%d] Dying.", pid);
			exit(0);
		}
	}
	fclose(new_stream);
	close(sock);
	return 0;
}

int create_room()
{

}

int send_msg()
{

}