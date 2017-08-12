#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define DEFAULT_PORT "8888"
#define BACKLOG 10
#define PACKET_MAX 1024
#define ASC_TIME_MAX 24
#define MSG_LEN 4096
#define NT_LEN 1
#define ROOT_DIR_LEN 53
#define GET_REQ_LEN 3
#define HTTP_VER_LEN 8
#define DEFAULT_PAGE_LEN 10
#define CODE_400_LEN 25
#define CODE_404_LEN 23
#define CODE_403_LEN 27
#define PORT_MIN 0
#define PORT_MAX 65536

/* Is it better to have HTTP response messages as globals or defined macros?*/
// Better to have responses as one line or multiline?
const char *const OK = "HTTP/1.1 200 OK",
		   *const NOT_FOUND = "HTTP/1.1 404 NOT FOUND",
		   *const FORBIDDEN = "HTTP/1.1 403 FORBIDDEN",
		   *const BAD_REQUEST = "HTTP/1.1 400 BAD REQUEST",
		   *const CREATED = "HTTP/1.1 201 CREATED",
		   *const TIMEOUT = "HTTP/1.1 408 REQUEST TIME-OUT",
		   *const SERVER_ERROR = "HTTP/1.1 500 INTERNAL SERVER ERROR",
		   *const log_root = "/home/elliott/github/C-Server-Collection/single-HTTP/";

// char *132doc_root = "/home/elliott/Github/C-Server-Collection/single-HTTP/"; // Was const after pointer

int sigint_flag = 1;
short verbose_flag = 1;

bool is_valid_listen(int socketfd) {
	return listen(socketfd, BACKLOG) == 0;
}

bool is_valid_address(const int status) {
	return status == 0;
}

bool is_valid_numberof_arguments(const int argc) {
	return argc == 1;
}

bool is_valid_port(const char *const port) {
	const int port_num = atoi(port);

	return ((port_num > PORT_MIN) || (port_num < PORT_MAX)); // Is this bool expression correct?
	// Maybe use && boolean...?
}

bool is_valid_request(char **const reqline) {
	if (strncmp(reqline[0], "GET", GET_REQ_LEN) != 0)
		return false;
	if ((strncmp(reqline[2], "HTTP/1.0", HTTP_VER_LEN) != 0) && (
		strncmp(reqline[2], "HTTP/1.1", HTTP_VER_LEN) != 0))
		return false;
	return true;
}

void determine_root(char **reqline) {
	if (strncmp(reqline[1], "/\0", 2) == 0) {
		printf("asked for /\n");
		strncpy(reqline[1], "index.html", DEFAULT_PAGE_LEN);
	} else {
		char *tok = strtok(reqline[1], "/");

		printf("Get the last elem?\n");
		if (!tok) {
			printf("add index.html to end?\n");
			strncpy(reqline[1], "index.html", DEFAULT_PAGE_LEN);
		} else {
			printf("add someting to end?\n");
			memmove(reqline[1], tok, strlen(reqline[1]));
		}
	}
	printf("Done determining root\n");
}

void compute_flags(int argc, char **const argv, const char **const port, short *verbose_flag) {
	int c;

	while ((c = getopt(argc, argv, "vp:")) != -1) {
		switch (c) {
		case 'p':
			*port = optarg;
			break;
		case 'v':
			*verbose_flag = 1;
			break;
		case '?':
			if (optopt == 'p')
				exit(EXIT_FAILURE); // Redundant?
				// terminate("");
		default:
			exit(EXIT_FAILURE); // Redundant?
			// terminate("Unknonwn flag.");
		}
	}
}

// Access bad?
void respond(char **const reqline, int client_fd) {
	if (access(reqline[1], F_OK | R_OK) == 0) {
		if (verbose_flag)
			printf("GET %s [200 OK]\n", reqline[1]);
		send(client_fd, "HTTP/1.1 200 OK\n\n", 17, 0);
		FILE *fp = fopen(reqline[1], "r");
		char *f_contents = malloc(PACKET_MAX + NT_LEN);

		// check_malloc(f_contents);
		size_t nbytes;

		while ((nbytes = fread(f_contents, sizeof(char), PACKET_MAX, fp)) > 0)
			send(client_fd, f_contents, nbytes, 0);

		free(f_contents);
		fclose(fp);
		close(client_fd);
		return;
	}

	if (access(reqline[1], F_OK) == -1) {
		if (verbose_flag)
			printf("GET %s [404 Not Found]\n", reqline[1]);
		send(client_fd, "HTTP/1.1 404 Not Found\n", CODE_404_LEN, 0);
		close(client_fd);
		return;
	}

	if (verbose_flag)
		printf("GET %s [403 Access Denied]\n", reqline[1]);
	send(client_fd, "HTTP/1.1 403 Access Denied\n", CODE_403_LEN, 0);
	close(client_fd);
}

void determine_response(char *msg, int client_fd, char *working_directory) {
	char **const reqline = malloc(3 * sizeof(char *));

	if (!reqline)
		exit(EXIT_FAILURE);
		// terminate("");

	for (int i = 0; i < 3; i++) {
		reqline[i] = calloc(128 + NT_LEN, sizeof(char));
		// check_malloc(reqline[i]);
	}

	char *tok = strtok(msg, " \t\n");

	strncpy(reqline[0], tok, (strlen(tok) + NT_LEN));
	tok = strtok(NULL, " \t");

	strncpy(reqline[1], tok, (strlen(tok) + NT_LEN));
	tok = strtok(NULL, " \t\n");

	strncpy(reqline[2], tok, (strlen(tok) + NT_LEN));

	printf("Done copying\nThis is r[0]: %s\nThis is r[1]: %s\nThis is r[2]: %s\n", reqline[0], reqline[1], reqline[2]);

	if (!is_valid_request(reqline)) {
		if (verbose_flag)
			printf("%s %s [400 Bad Request]\n", reqline[0],
			       reqline[1]);
		send(client_fd, "HTTP/1.1 400 Bad Request\n", CODE_400_LEN, 0);
		close(client_fd);
	} else {
		printf("Starting determine root\n");
		determine_root(reqline);
		printf("Concatenating determined root\n");
		printf("This is wd: %s\n This is r[1]: %s\n", working_directory, reqline[1]);
		strncat(working_directory, reqline[1], strlen(reqline[1]));
		printf("successful determine response\n");
		respond(reqline, client_fd);
	}

	for (int i = 0; i < 3; i++)
		free(reqline[i]);
	free(reqline);
	printf("Reqline done free\n");
}

// Feature need, rolling log files
void server_log(const char *const msg) {
	time_t cur_time = time(NULL);
	char *log_dir = calloc(PATH_MAX + NT_LEN, sizeof(char)),
		 *log_archive = calloc(ASC_TIME_MAX + strlen(msg) + 4 + NT_LEN, sizeof(char));

	strncpy(log_dir, log_root, PATH_MAX);
	strncat(log_dir, "server.log", 10);
	FILE *fp = fopen(log_dir, "a");

	// use fprintf and reduce strncat calls?
	strncpy(log_archive, asctime(localtime(&cur_time)), ASC_TIME_MAX);
	strncat(log_archive, " ", 1);
	strncat(log_archive, msg, strlen(msg));
	strncat(log_archive, "\r\n", 2);
	fputs(log_archive, fp); // Use fprintf?
	fclose(fp);
	free(log_dir);
	free(log_archive);
}

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in *)sa)->sin_addr);

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void init_addrinfo(struct addrinfo *addressinfo) {
	memset(addressinfo, 0, sizeof(*addressinfo));
	(*addressinfo).ai_family = AF_INET;
	(*addressinfo).ai_socktype = SOCK_STREAM; // TCP
	(*addressinfo).ai_flags = AI_PASSIVE | AI_V4MAPPED; // Correct flags?
}

void get_socket(int *socketfd, struct addrinfo *serviceinfo) {
	const short yes = 1;
	struct addrinfo *p;

	for (p = serviceinfo; p; p = p->ai_next) {
		*socketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		if (*socketfd == -1) {
			perror("server: socket"); // Log it/Ignore it?
			continue;
		}

		if (setsockopt(*socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt"); // Log it/Ignore it?
//			terminate("");
		}

		if (bind(*socketfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(*socketfd);
			perror("server: bind"); // Log it/Ignore it?
			continue;
		}

		break;
	}

	freeaddrinfo(serviceinfo);

	if (!p)  {
		fprintf(stderr, "server: failed to bind"); // Log it/Ignore it?
//		terminate("");
	}
}

void handle_sigint(int arg) {
	arg = 0;
	sigint_flag = arg;
}

void init_signals(void) {
	struct sigaction new_action_int;

	new_action_int.sa_handler = handle_sigint; // Look in man page

	sigemptyset(&new_action_int.sa_mask);
	new_action_int.sa_flags = 0;

	if (sigaction(SIGINT, &new_action_int, NULL) == -1)
		// terminate(strerror(errno));
		exit(EXIT_FAILURE);
}

int main(int argc, char **const argv) {
	const char *port = DEFAULT_PORT;
	char *initwd,
		 *ipv4_address = "",
		 *const doc_root = calloc(PATH_MAX + NT_LEN, sizeof(char));
	int status, socketfd, new_fd; // File descriptor proper names? Masterfd, newfd?
	struct sockaddr_storage recv_addr;
	struct addrinfo addressinfo, *serviceinfo;
	socklen_t sin_size = sizeof(recv_addr);

	init_signals();
	if (!is_valid_numberof_arguments(argc))
		exit(EXIT_FAILURE);
	compute_flags(argc, argv, &port, &verbose_flag);
	if (!is_valid_port(port))
		exit(EXIT_FAILURE);

	init_addrinfo(&addressinfo);
	status = getaddrinfo(NULL, port, &addressinfo, &serviceinfo);

	if (!is_valid_address(status))
		exit(EXIT_FAILURE);

	get_socket(&socketfd, serviceinfo);

	if (!is_valid_listen(socketfd))
		exit(EXIT_FAILURE);

	initwd = getcwd(NULL, PATH_MAX);
	if (!initwd)
		exit(EXIT_FAILURE);

	if (verbose_flag)
		printf("Initialization: SUCCESS; Listening on port %s, root is %s\n", port, initwd);

	char *msg = malloc((MSG_LEN + NT_LEN) * sizeof(char));

	while (sigint_flag) {
		strncpy(doc_root, "/home/elliott/Github/C-Server-Collection/single-HTTP/", ROOT_DIR_LEN);
		// char *workingDirectory = calloc(PATH_MAX + NT_LEN, sizeof(char)); // Can this be put on stack?

//		check_malloc(workingDirectory);
		// char *msg = malloc((MSG_LEN + NT_LEN) * sizeof(char));

//		check_malloc(msg);

		// strncpy(workingDirectory, initwd, (strlen(initwd) + 1));
		// strncat(workingDirectory, "/", 1);
		// sin_size = sizeof(recv_addr);
		new_fd = accept(socketfd, (struct sockaddr *)&recv_addr, &sin_size);

		if (new_fd == -1) {
			perror("accept"); // Log it/ignore?
			continue;
		}

		inet_ntop(recv_addr.ss_family, get_in_addr((struct sockaddr*)&recv_addr), ipv4_address, sizeof(*ipv4_address));

		if (recv(new_fd, msg, MSG_LEN, 0) < 0) { // Look into man page
			perror("recv"); // Log it/ignore?
			continue;
		} else
			printf("RECIEVE PACKET: SUCCESS\n");
		determine_response(msg, new_fd, doc_root);
	}
	free(initwd);
	free(msg);
	free(doc_root);
	printf("Closed\n");

	return 0;
}

