#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <libgen.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/limits.h>

#define OK "HTTP/1.0 200 OK\n\n"
#define CREATED "HTTP/1.0 201 CREATED\n\n"
#define NOT_FOUND "HTTP/1.0 404 NOT FOUND\n\n"
#define FORBIDDEN "HTTP/1.0 403 FORBIDDEN\n\n"
#define BAD_REQUEST "HTTP/1.0 400 BAD REQUEST\n\n"
#define SERVER_ERROR "HTTP/1.0 500 INTERNAL SERVER ERROR\n\n"
#define NOT_SUPPORTED "HTTP/1.0 505 HTTP VERSION NOT SUPPORTED\n\n"
#define NOT_IMPLEMENTED "HTTP/1.0 501 NOT IMPLEMENTED\n\n"

#define DEFAULT_ROOT "/home/elliott/Github/C-Server-Collection/single-HTTP/"
#define DEFAULT_LOG_ROOT "/home/elliott/Github/C-Server-Collection/single-HTTP/logs/"

#define DEFAULT_PORT "8888"
#define MSG_TEMPLATE "Connection from %s for file %s"
#define USAGE_MSG "Usage: %s [-h] [-V] [-v] [-s <configuration file>]\n"

#define BACKLOG 1
#define STR_MAX 2048
#define PORT_MIN 0
#define PORT_MAX 65536
#define MAX_ARGS 4
#define PACKET_MAX 1024
#define HTTP_VER_AMT 3
#define HTTP_REQ_AMT 8
#define REQLINE_TOKEN_AMT 3

#define NT_LEN 1
#define MSG_LEN 4096
#define PORT_LEN 5
#define FTIME_MLEN 25
#define GET_REQ_LEN 3
#define PHP_EXT_LEN 4
#define REQLINE_LEN 128
#define CONF_EXT_LEN 5
#define HTTP_VER_LEN 8
#define CODE_200_LEN 17
#define CODE_400_LEN 26
#define CODE_403_LEN 24
#define CODE_404_LEN 24
#define CODE_500_LEN 36
#define	CODE_501_LEN 30
#define CODE_505_LEN 41
#define MSG_TEMP_LEN 41
#define DEFAULT_PAGE_LEN 10
#define MDEFAULT_PAGE_LEN 2
#define FF_TIME_PATH_MLEN 19

char _port[PORT_LEN] = DEFAULT_PORT,
	 _doc_root[PATH_MAX] = DEFAULT_ROOT,
	 _log_root[PATH_MAX] = DEFAULT_LOG_ROOT;
const char *const _http_requests[HTTP_REQ_AMT] = {"GET", "HEAD", "POST", "PUT",
												 "DELETE", "CONNECT", "OPTIONS",
												 "TRACE"},
		   *const _http_ver[HTTP_VER_AMT] = {"HTTP/1.0", "HTTP/1.1", "HTTP/2.0"};
bool verbose_flag = false, sigint_flag = true;

bool is_valid_port(void) { // Done
	const int port_num = atoi(_port);

	return ((PORT_MIN < port_num) && (port_num < PORT_MAX));
}

bool is_valid_request(char **const reqline) { // Done
	for (int i = 0; i < HTTP_REQ_AMT; i++)
		if (strncmp(reqline[0], _http_requests[i], strnlen(_http_requests[i], STR_MAX)) == 0)
			return true;

	for (int i = 0; i < HTTP_VER_AMT; i++)
		if (strncmp(reqline[2], _http_ver[i], strnlen(_http_ver[i], STR_MAX)) == 0)
			return true;

	return false;
}

void determine_root(char **const reqlines) { // Done
	char *const path = reqlines[1];

	if (strncmp(path, "/\0", MDEFAULT_PAGE_LEN) == 0)
		strncpy(path, "index.html", DEFAULT_PAGE_LEN);
	else
		memmove(path, path + 1, strnlen(path, PATH_MAX));
}

void load_configuration(const char *const path) { // Needs logic improvement
	const char *extension = strrchr(path, '.');

	if (strncmp(extension, ".conf", CONF_EXT_LEN) != 0) {
		fprintf(stderr, "-s option takes a configuration file as an argument; example.conf\n");
		exit(EXIT_FAILURE);
	}

	char defn[8 + NT_LEN], value[PATH_MAX + NT_LEN];
	FILE *conf_f = fopen(path, "r");

	if (conf_f) {
		fscanf(conf_f, "%s %s", defn, value);
		strncpy(_port, value, PORT_LEN);
		fscanf(conf_f, "%s %s", defn, value);
		strncpy(_doc_root, value, PATH_MAX);
		fscanf(conf_f, "%s %s", defn, value);
		strncpy(_log_root, value, PATH_MAX);
		fclose(conf_f);
	} else {
		fprintf(stderr, "%s", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void compute_flags(const int argc, char **const argv, bool *v_flag) { // Done
	int c;

	while ((c = getopt(argc, argv, "hVvs:")) != -1) {
		switch (c) {
		case 'h':
			printf(USAGE_MSG
				   "-h\tHelp menu\n"
				   "-V\tVersion\n"
				   "-v\tVerbose\n"
				   "-s\tLoad a configuration file\n", basename(argv[0]));
			exit(EXIT_SUCCESS);
		case 'V':
			printf("Version 0.1\n");
			exit(EXIT_SUCCESS);
		case 'v':
			*v_flag = true;
			break;
		case 's':
			load_configuration(optarg);
			break;
		case '?':
			exit(EXIT_FAILURE);
		}
	}
}

void server_log(const char *const msg) { // Look into setuid & setgid bits
	const mode_t mode_d = 0770, mode_f = 0660;
	const time_t cur_time = time(NULL);
	char *log_dir = (char*) calloc(PATH_MAX + NT_LEN, sizeof(char)),
		 *f_time = (char*) malloc((FTIME_MLEN + NT_LEN) * sizeof(char)),
		 ff_time_path[FF_TIME_PATH_MLEN + NT_LEN];
	const struct tm *const t_data = localtime(&cur_time);

	if (!log_dir || !f_time) {
		server_log(strerror(errno));
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
	snprintf(log_dir, PATH_MAX, "%s%s", _log_root, ff_time_path);

	const int fd = open(log_dir, O_CREAT | O_WRONLY | O_APPEND, mode_f);
	if (fd == -1)
		perror(strerror(errno));
	else
		dprintf(fd, "[%s]: %s\n", f_time, msg);

	close(fd);
	free(f_time);
	f_time = NULL;

	free(log_dir);
	log_dir = NULL;
}

void process_php(const int client_fd, const char *const file_path) { // Done
	const pid_t c_pid = fork();

	if (c_pid == -1)
		server_log(strerror(errno));

	if (c_pid == 0) {
		dup2(client_fd, STDOUT_FILENO);
		execl("/usr/bin/php", "php", file_path, (char*) NULL);
	}
	close(client_fd);
}

void send_file(const int client_fd, const char *const path) { // Done
	const int fd = open(path, O_RDONLY);

	if (fd > -1) {
		char *f_contents = (char*) malloc((PACKET_MAX + NT_LEN) * sizeof(char));

		if (!f_contents) {
			server_log(strerror(errno));
			exit(EXIT_FAILURE);
		}

		size_t nbytes = read(fd, f_contents, PACKET_MAX);

		while (nbytes > 0) {
			send(client_fd, f_contents, nbytes, 0);
			nbytes = read(fd, f_contents, PACKET_MAX);
		}

		free(f_contents);
		f_contents = NULL;
	}

	close(fd);
	close(client_fd);
}

void respond(const int client_fd, char **const reqlines, const char *const path) { // Add codes 501 & 505
	if (!is_valid_request(reqlines)) {
		if (verbose_flag)
			printf("%s %s [400 Bad Request]\n", reqlines[0], reqlines[1]);
		send(client_fd, BAD_REQUEST, CODE_400_LEN, 0);
		send_file(client_fd, "http-code-responses/400.html");
		return;
	}

	if (strncmp(reqlines[2], "HTTP/2.0", HTTP_VER_LEN) == 0) {
		if (verbose_flag)
			printf("GET %s %s [505 Http Version Not Supported]\n", reqlines[1], reqlines[2]);
		send(client_fd, NOT_SUPPORTED, CODE_505_LEN, 0);
		send_file(client_fd, "http-code-responses/505.html");
		return;
	}

	if (strncmp(reqlines[0], "GET", 3) != 0) {
		if (verbose_flag)
			printf("%s %s [501 Not Implemented]\n", reqlines[0], reqlines[1]);
		send(client_fd, NOT_IMPLEMENTED, CODE_501_LEN, 0);
		send_file(client_fd, "http-code-responses/501.html");
		return;
	}

	const int fd = open(path, O_RDONLY);

	if (fd > -1) {
		close(fd);

		if (verbose_flag)
			printf("GET %s [200 OK]\n", reqlines[1]);
		send(client_fd, OK, CODE_200_LEN, 0);
		const char *extension = strrchr(path, '.');

		if (strncmp(extension, ".php", PHP_EXT_LEN) == 0)
			process_php(client_fd, path);
		else
			send_file(client_fd, path);
	}
	else if (errno == ENOENT) {
		if (verbose_flag)
			printf("GET %s [404 Not Found]\n", reqlines[1]);
		send(client_fd, NOT_FOUND, CODE_404_LEN, 0);
		send_file(client_fd, "http-code-responses/404.html");
	}
	else if (errno == EACCES) {
		if (verbose_flag)
			printf("GET %s [403 Access Denied]\n", reqlines[1]);
		send(client_fd, FORBIDDEN, CODE_403_LEN, 0);
		send_file(client_fd, "http-code-responses/403.html");
	}
	else {
		if (verbose_flag)
			printf("GET %s [500 Internal Server Error]\n", reqlines[1]);
		send(client_fd, SERVER_ERROR, CODE_500_LEN, 0);
		send_file(client_fd, "http-code-responses/500.html");
	}
	close(fd);
}

char **get_req_lines(char *msg) { // Done
	char **const reqline = (char**) malloc(REQLINE_TOKEN_AMT * sizeof(char*));

	if (!reqline) {
		server_log(strerror(errno));
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < REQLINE_TOKEN_AMT; i++) {
		reqline[i] = (char*) calloc(REQLINE_LEN + NT_LEN, sizeof(char));
		if (!reqline[i]) {
			server_log(strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	char *tok = strtok(msg, " \t\n");

	if (!tok)
		return NULL;

	strncpy(reqline[0], tok, (strnlen(tok, STR_MAX) + NT_LEN));
	tok = strtok(NULL, " \t");

	if (!tok)
		return NULL;

	strncpy(reqline[1], tok, (strnlen(tok, STR_MAX) + NT_LEN));
	tok = strtok(NULL, " \t\n");

	if (!tok)
		return NULL;

	strncpy(reqline[2], tok, (strnlen(tok, STR_MAX) + NT_LEN));
	return reqline;
}

void free_req_lines(char **reqline) { // Done
	if (!reqline)
		return;
	for (int i = 0; i < REQLINE_TOKEN_AMT; i++) {
		if (!reqline[i])
			continue;
		free(reqline[i]);
		reqline[i] = NULL;
	}
	free(reqline);
	reqline = NULL;
}

void init_addrinfo(struct addrinfo *const addressinfo) { // Done
	memset(addressinfo, 0, sizeof(*addressinfo));
	(*addressinfo).ai_family = AF_INET; // IPV4
	(*addressinfo).ai_socktype = SOCK_STREAM; // TCP
	(*addressinfo).ai_flags = AI_PASSIVE | AI_V4MAPPED; // Gen socket, IPV4
}

int get_socket(int *const socketfd, struct addrinfo *const serviceinfo) { // Done
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

	return -1;

}

void handle_sigint(const int arg) { // Done
	sigint_flag = false;
}

void init_signals(void) { // Done
	struct sigaction new_action_int;

	new_action_int.sa_handler = handle_sigint;

	sigemptyset(&new_action_int.sa_mask);
	new_action_int.sa_flags = 0;

	if (sigaction(SIGINT, &new_action_int, NULL) == -1) {
		perror(strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void process_request(const int fd, char *msg, const char *const ipv4_address) { // Done
	char cust_msg[MSG_TEMP_LEN + PATH_MAX],
		 **const reqlines = get_req_lines(msg);

	if (!reqlines) {
		snprintf(cust_msg, 29 + INET_ADDRSTRLEN, "Connection from %s; BAD REQUEST", ipv4_address);
		server_log(cust_msg);
		send(fd, BAD_REQUEST, CODE_400_LEN, 0);
		send_file(fd, "http-code-responses/400.html");
	} else {
		determine_root(reqlines);
		strncat(_doc_root, reqlines[1], PATH_MAX);
		snprintf(cust_msg, MSG_TEMP_LEN + PATH_MAX, MSG_TEMPLATE, ipv4_address, reqlines[1]);
		server_log(cust_msg);
		respond(fd, reqlines, _doc_root);
	}
	free_req_lines(reqlines);
}

int main(const int argc, char **const argv) {
	char ipv4_address[INET_ADDRSTRLEN];
	int masterfd, newfd;
	struct addrinfo addressinfo, *serviceinfo;
	struct sockaddr client_addr;
	socklen_t sin_size = sizeof(client_addr);

	init_signals();
	if (argc > MAX_ARGS) {
		fprintf(stderr, USAGE_MSG, basename(argv[0]));
		exit(EXIT_FAILURE);
	}
	compute_flags(argc, argv, &verbose_flag);
	if (!is_valid_port()) {
		fprintf(stderr, "Error: Invalid port %s\n", _port);
		exit(EXIT_FAILURE);
	}

	init_addrinfo(&addressinfo);

	if (getaddrinfo(NULL, _port, &addressinfo, &serviceinfo) != 0) {
		perror(gai_strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (get_socket(&masterfd, serviceinfo) == -1)
		exit(EXIT_FAILURE);

	if (listen(masterfd, BACKLOG) == -1) {
		perror(strerror(errno));
		exit(EXIT_FAILURE);
	}

	char *msg = (char*) malloc((MSG_LEN + NT_LEN) * sizeof(char));

	if (!msg) {
		server_log(strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (verbose_flag)
		printf("Initialization: SUCCESS; Listening on port %s, root is %s\n", _port, _doc_root);

	const int default_root_len = strnlen(_doc_root, PATH_MAX);

	while (sigint_flag) {
		_doc_root[default_root_len] = '\0';
		newfd = accept(masterfd, &client_addr, &sin_size);

		if (newfd == -1) {
			server_log(strerror(errno));
			continue;
		}

		inet_ntop(AF_INET, &(((struct sockaddr_in*)&client_addr)->sin_addr), ipv4_address, INET_ADDRSTRLEN);

		if (recv(newfd, msg, MSG_LEN, 0) > 0)
			process_request(newfd, msg, ipv4_address);
		else
			server_log(strerror(errno));
	}
	free(msg);
	msg = NULL;

	return EXIT_SUCCESS;
}

