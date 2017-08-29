// Bug with random get???
// Php memory leak?
// HTTP/1.0 vs HTTP/1.1
// Separate ip address from determine_response

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define ROOT_DIR "/home/elliott/Github/C-Server-Collection/single-HTTP/"
#define DEFAULT_PORT "8888"
#define OK "HTTP/1.1 200 OK\n\n"
#define NOT_FOUND "HTTP/1.1 404 NOT FOUND\n\n"
#define FORBIDDEN "HTTP/1.1 403 FORBIDDEN\n\n"
#define BAD_REQUEST "HTTP/1.1 400 BAD REQUEST\n\n"
#define CREATED "HTTP/1.1 201 CREATED\n\n"
#define TIMEOUT "HTTP/1.1 408 REQUEST TIME-OUT\n\n"
#define SERVER_ERROR "HTTP/1.1 500 INTERNAL SERVER ERROR\n\n"
#define LOG_ROOT "/home/elliott/Github/C-Server-Collection/single-HTTP/logs/"

#define BACKLOG 1
#define MAX_ARGS 4
#define PACKET_MAX 1024
#define ASC_TIME_MAX 24
#define MSG_LEN 4096
#define NT_LEN 1
#define ROOT_DIR_LEN 53
#define GET_REQ_LEN 3
#define HTTP_VER_LEN 8
#define PHP_EXT_LEN 4
#define MDEFAULT_PAGE_LEN 2
#define DEFAULT_PAGE_LEN 10
#define LOG_FILE_LEN 7
#define LOG_ROOT_LEN 58
#define FTIME_MLEN 25
#define FF_TIME_PATH_MLEN 19
#define CODE_200_LEN 17
#define CODE_400_LEN 26
#define CODE_404_LEN 24
#define CODE_403_LEN 24
#define CODE_500_LEN 36
#define REQLINE_LEN 128
#define REQLINE_TOKEN_AMT 3
#define PORT_MIN 0
#define PORT_MAX 65536

bool verbose_flag = false, sigint_flag = true;

bool is_valid_port(const char *const port) {
	const int port_num = atoi(port);

	return ((port_num > PORT_MIN) && (port_num < PORT_MAX));
}

bool is_valid_request(char **const reqline) {
	if (strncmp(reqline[0], "GET", GET_REQ_LEN) != 0)
		return false;
	if ((strncmp(reqline[2], "HTTP/1.0", HTTP_VER_LEN) != 0) && (
		strncmp(reqline[2], "HTTP/1.1", HTTP_VER_LEN) != 0))
		return false;
	return true;
}

void determine_root(char *path) {
	if (strncmp(path, "/\0", MDEFAULT_PAGE_LEN) == 0)
		strncpy(path, "index.html", DEFAULT_PAGE_LEN);
	else
		memmove(path, path + 1, strlen(path));
}

void compute_flags(const int argc, char **const argv, const char **const port, bool *v_flag) {
	int c;

	while ((c = getopt(argc, argv, "hvp:")) != -1) {
		switch (c) {
		case 'h':
			printf("Usage: ./single-HTTP [-h] [-v] [-p <unsigned int>]\n"
				"-h\tHelp menu\n"
				"-v\tVerbose\n"
				"-p\t<Unsigned Int>\tPort number. 1-65535; 1-1024 require root privileges.\n");
			exit(EXIT_SUCCESS);
		case 'p':
			*port = optarg;
			break;
		case 'v':
			*v_flag = true;
			break;
		case '?':
			exit(EXIT_FAILURE);
		}
	}
}

// Program owner and group owner
void server_log(const char *const msg) {
	const mode_t mode_d = 0770, mode_f = 0660;
	const time_t cur_time = time(NULL);
	char *const log_dir = calloc(PATH_MAX + NT_LEN, sizeof(char)),
		 *const f_time = malloc((FTIME_MLEN + NT_LEN) * sizeof(char)),
		 ff_time_path[FF_TIME_PATH_MLEN + NT_LEN];
	const struct tm *const t_data = localtime(&cur_time);

	if (!log_dir || !f_time) {
		fprintf(stderr, "Error: Memory allocation\n");
		exit(EXIT_FAILURE);
	}

	strftime(f_time, FTIME_MLEN, "%a %b %d %T %Y", t_data);
	strftime(ff_time_path, 10, "logs/%Y", t_data);
	mkdir(ff_time_path, mode_d);

	strftime(ff_time_path, 14, "logs/%Y/%b", t_data);
	mkdir(ff_time_path, mode_d);

	strftime(ff_time_path, 17, "logs/%Y/%b/%U", t_data);
	mkdir(ff_time_path, mode_d);

	strftime(ff_time_path, 20, "%Y/%b/%U/%a.log", t_data);
	snprintf(log_dir, PATH_MAX, "%s%s", LOG_ROOT, ff_time_path);

	const int fd = open(log_dir, O_CREAT | O_WRONLY | O_APPEND, mode_f);
	if (fd == -1)
		perror(strerror(errno));
	else
		dprintf(fd, "[%s]: %s\n", f_time, msg);

	close(fd);
	free(f_time);
	free(log_dir);
}

void process_php(const int client_fd, const char *const script_path) {
	const pid_t c_pid = fork();

	if (c_pid == -1) {
		server_log(strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (c_pid == 0) {
		dup2(client_fd, STDOUT_FILENO);
		execl("/usr/bin/php", "php", script_path, (char *) NULL);
	}
}

void send_file(const int client_fd, const char *const path) {
	const int fd = open(path, O_RDONLY);

	if (fd > -1) {
		char *const f_contents = malloc((PACKET_MAX + NT_LEN) * sizeof(char));

		if (!f_contents) {
			fprintf(stderr, "Error: Memeory allocation\n");
			exit(EXIT_FAILURE);
		}

		size_t nbytes = read(fd, f_contents, sizeof(char));

		while (nbytes > 0) {
			send(client_fd, f_contents, nbytes, 0);
			nbytes = read(fd, f_contents, sizeof(char));
		}

		free(f_contents);
	}

	close(fd);
}

void respond(const int client_fd, char *const path) {
	const int fd = open(path, O_RDONLY);

	if (fd > -1) {
		close(fd);

		if (verbose_flag)
			printf("GET %s [200 OK]\n", path);
		send(client_fd, OK, CODE_200_LEN, 0);
		const char *extension = strrchr(path, '.');

		if (strncmp(extension, ".php", PHP_EXT_LEN) == 0)
			process_php(client_fd, path);
		else
			send_file(client_fd, path);
	}
	else if (errno == ENOENT) {
		if (verbose_flag)
			printf("GET %s [404 Not Found]\n", path);
		send(client_fd, NOT_FOUND, CODE_404_LEN, 0);
		send_file(client_fd, "http-code-responses/404.html");
	}
	else if (errno == EACCES) {
		if (verbose_flag)
			printf("GET %s [403 Access Denied]\n", path);
		send(client_fd, FORBIDDEN, CODE_403_LEN, 0);
		send_file(client_fd, "http-code-responses/403.html");
	}
	else {
		if (verbose_flag)
			printf("GET %s [500 Internal Server Error]\n", path);
		send(client_fd, SERVER_ERROR, CODE_500_LEN, 0);
		send_file(client_fd, "http-code-responses/500.html");
	}
	close(fd);
	close(client_fd);
}

void determine_response(const int client_fd, char *msg, char *working_directory, const char *const ipv4_address) {
	char **const reqline = malloc(REQLINE_TOKEN_AMT * sizeof(char*));

	if (!reqline) {
		fprintf(stderr, "Error: Memory allocation\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < REQLINE_TOKEN_AMT; i++) {
		reqline[i] = calloc(REQLINE_LEN + NT_LEN, sizeof(char));
		if (!reqline[i]) {
			fprintf(stderr, "Error: Memory allocation\n");
			exit(EXIT_FAILURE);
		}
	}

	char *tok = strtok(msg, " \t\n");
	if (!tok)
		goto BAD;
	printf("This is tok 1 in determine response: %s\n", tok);

	strncpy(reqline[0], tok, (strlen(tok) + NT_LEN));
	tok = strtok(NULL, " \t");
	if (!tok)
		goto BAD;
	printf("This is tok 2 in determine response: %s\n", tok);

	strncpy(reqline[1], tok, (strlen(tok) + NT_LEN));
	tok = strtok(NULL, " \t\n");
	if (!tok)
		goto BAD;
	printf("This is tok 3 in determine response: %s\n", tok);

	strncpy(reqline[2], tok, (strlen(tok) + NT_LEN));

	printf("Done copying\nThis is r[0]: %s\nThis is r[1]: %s\nThis is r[2]: %s\n", reqline[0], reqline[1], reqline[2]);

	char bob[200];
	snprintf(bob, 200, "Connection from %s for file %s", ipv4_address, reqline[1]); // TMP
	server_log(bob);

	if (!is_valid_request(reqline)) {
		BAD: if (verbose_flag)
			     printf("%s %s [400 Bad Request]\n", reqline[0], reqline[1]);
		send(client_fd, BAD_REQUEST, CODE_400_LEN, 0);
		send_file(client_fd, "http-code-responses/400.html");
		close(client_fd);
	} else {
		determine_root(reqline[1]);
		strncat(working_directory, reqline[1], strlen(reqline[1]));
		respond(client_fd, reqline[1]);
	}

	for (int i = 0; i < REQLINE_TOKEN_AMT; i++)
		free(reqline[i]);
	free(reqline);
}

void *get_in_addr(const struct sockaddr *const sa) {
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in *)sa)->sin_addr);

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void init_addrinfo(struct addrinfo *const addressinfo) {
	memset(addressinfo, 0, sizeof(*addressinfo));
	(*addressinfo).ai_family = AF_INET; // IPV4
	(*addressinfo).ai_socktype = SOCK_STREAM; // TCP
	(*addressinfo).ai_flags = AI_PASSIVE | AI_V4MAPPED; // Gen socket, IPV4
}

int get_socket(int *const socketfd, struct addrinfo *const serviceinfo) {
	const short yes = 1;
	const struct addrinfo *p;

	for (p = serviceinfo; p; p = p->ai_next) {
		*socketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		if (*socketfd == -1) {
			perror(strerror(errno));
			continue;
		}

		if (setsockopt(*socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror(strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (bind(*socketfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror(strerror(errno));
			close(*socketfd);
			continue;
		}

		break;
	}

	freeaddrinfo(serviceinfo);

	if (p)
		return 0;
	else
		return -1;

}

void handle_sigint(const int arg) {
	sigint_flag = false;
}

void init_signals(void) {
	struct sigaction new_action_int;

	new_action_int.sa_handler = handle_sigint;

	sigemptyset(&new_action_int.sa_mask);
	new_action_int.sa_flags = 0;

	if (sigaction(SIGINT, &new_action_int, NULL) == -1) {
		perror(strerror(errno));
		exit(EXIT_FAILURE);
	}
}

int main(const int argc, char **const argv) {
	const char *port = DEFAULT_PORT;
	char ipv4_address[INET_ADDRSTRLEN], *initwd,
		*const doc_root = calloc(PATH_MAX + NT_LEN, sizeof(char));
	int masterfd, newfd;
	struct sockaddr_storage client_addr;
	struct addrinfo addressinfo, *serviceinfo;
	socklen_t sin_size = sizeof(client_addr);

	if (!doc_root) {
		fprintf(stderr, "Error: Memory allocation\n");
		exit(EXIT_FAILURE);
	}

	init_signals();
	if (argc > MAX_ARGS) {
		fprintf(stderr, "Usage: ./single-HTTP [-h] [-v] [-p <unsigned int>]\n");
		exit(EXIT_FAILURE);
	}
	compute_flags(argc, argv, &port, &verbose_flag);
	if (!is_valid_port(port)) {
		fprintf(stderr, "Error: Invalid port number\n");
		exit(EXIT_FAILURE);
	}

	init_addrinfo(&addressinfo);

	if (getaddrinfo(NULL, port, &addressinfo, &serviceinfo) != 0) {
		perror(gai_strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (get_socket(&masterfd, serviceinfo) == -1) {
		free(doc_root);
		exit(EXIT_FAILURE);
	}

	if (listen(masterfd, BACKLOG) == -1) {
		perror(strerror(errno));
		exit(EXIT_FAILURE);
	}

	initwd = getcwd(NULL, PATH_MAX);
	if (!initwd) {
		perror(strerror(errno));
		exit(EXIT_FAILURE);
	}

	char *const msg = malloc((MSG_LEN + NT_LEN) * sizeof(char));

	if (!msg) {
		fprintf(stderr, "Error: Memory allocation\n");
		exit(EXIT_FAILURE);
	}
	strncpy(doc_root, ROOT_DIR, ROOT_DIR_LEN);

	if (verbose_flag)
		printf("Initialization: SUCCESS; Listening on port %s, root is %s\n", port, initwd);

	while (sigint_flag) {
		doc_root[ROOT_DIR_LEN] = '\0';

		newfd = accept(masterfd, (struct sockaddr *)&client_addr, &sin_size);

		if (newfd == -1) {
			server_log(strerror(errno));
			continue;
		}

		inet_ntop(AF_INET, get_in_addr((struct sockaddr*)&client_addr), ipv4_address, INET_ADDRSTRLEN);

		if (recv(newfd, msg, MSG_LEN, 0) < 0) {
			server_log(strerror(errno));
			continue;
		}
		determine_response(newfd, msg, doc_root, ipv4_address);
	}
	free(initwd);
	free(msg);
	free(doc_root);
	return 0;
}

