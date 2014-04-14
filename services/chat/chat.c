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

#define MAX_ARGV          5
#define LEN_ARG         128
#define ARG_SEPARATORS " \t"

#define ROOMS_MAX     65536
#define DEFAULT_PORT   5555
#define BUF_SIZE      32767

#define die(message)  { perror(message); exit(1); }
#define die2(message) { fprintf(stderr, "%s\n",message); exit(1); }

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

    if (bind(sock, (void *) &name, sizeof(name)) < 0)
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

void print_help()
{
    WriteLn("\\register <name> <pass> - register new user");
    WriteLn("\\login <name> <pass>    - login");
    WriteLn("\\create <room> [pass]   - create new room (with optional password)");
    WriteLn("\\list                   - list all chat rooms");
    WriteLn("\\join <room>            - join chat room by name");
    WriteLn("\\leave                  - leave current chat room");
    WriteLn("\\quit                   - quit program");
}

void parse_argv(char buf[], char **argv, int *argc)
{
    (*argc) = 0;
    char *c = strtok(buf, ARG_SEPARATORS);
    while (c != NULL && (*argc) < MAX_ARGV)
    {
        argv[(*argc)++] = c;
        c = strtok(NULL, ARG_SEPARATORS);
    }
}

void process_client()
{
    char *argv[MAX_ARGV];
    char buf[BUF_SIZE];
    int argc = 0;

    print_greeting();

    while (1)
    {
        Write("> ");
        if (!fgets(buf, BUF_SIZE, stdin))
            break;
        strtok(buf, "\r\n");            // Skip everything after "\r" or "\n"

        if (buf[0] != '\\')             // If not command, then client wants to say something
        {
            say(buf);
            room_history();
            WriteLn("");
            continue;
        }

        parse_argv(buf, argv, &argc);    // It it sommand, so let's parse it
        char *cmd = argv[0];             // At least 1 token exists

        if (!strcmp(cmd, "\\quit"))
        {
            break;
        }
        else if (!strcmp(cmd, "\\help"))
        {
            print_help();
        }
        else if (!strcmp(cmd, "\\list") && argc == 1)
        {
            list();
        }
        else if (!strcmp(cmd, "\\register") && argc == 3)
        {
            user_create(argv[1], argv[2]);            // (user,pass)
        }
        else if (!strcmp(cmd, "\\login") && argc == 3)
        {
            user_login(argv[1], argv[2]);             // (user,pass)
        }
        else if (!strcmp(cmd, "\\create") && argc >= 2 && argc <= 3)
        {
            room_create(argv[1], argc == 3 ? argv[2] : NULL);
        }
        else if (!strcmp(cmd, "\\join") && argc >= 2 && argc <= 3)
        {
            room_join(argv[1], argc == 3 ? argv[2] : NULL);
        }
        else
        {
            WriteLn("Unknown command or wrong number of arguments (type '\\help' for commands list)");
        }

        WriteLn("");
    }
}

void test_mongo_connection()
{
    int connect = db_connect();
    if (connect < 0) die2("db connection failed");
    db_disconnect();
}

int main(int argc, char **argv)
{
    signal(SIGCHLD, SIG_IGN);

    test_mongo_connection();

    int port = argc >= 2 ? atoi(argv[1]) : DEFAULT_PORT;
    int server = create_server_socket(port);

    while (1)
    {
        struct sockaddr cli_addr;
        int cli_len = sizeof(cli_addr);
        int client = accept(server, &cli_addr, &cli_len);
        if (client < 0)
            die("accept");

        int pid = fork();
        if (pid < 0)
            die("fork");

        if (pid == 0)
        {
            if (dup2(client, STDIN_FILENO) < 0) die("dup2 stdin");
            if (dup2(client, STDOUT_FILENO) < 0) die("dup2 stdout");

            D("  [%d] process started\n", getpid());

            if (db_connect() == 0)
            {
                process_client();
                db_disconnect();
            }
            else
            {
                WriteLn("DB connection problem, sorry");
                D("  [%d] cannot connect to db\n", getpid());
            }

            D("  [%d] process finished\n", getpid());

            shutdown(client, 2);
            exit(0);
        }
    }
    close(server);
    return 0;
}
