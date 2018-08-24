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

#include "globals.h"
#include "lib/logging/log.h"
#include "lib/types/types.h"
#include "lib/colors/colors.h"
#include "lib/sqlite3/sqlite3.h"
#include "lib/hashtable/hashtable.h"
#include "lib/s_linked_list/s_linked_list.h"

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
#define DEFAULT_DB_ROOT "/home/elliott/Github/C-Server-Collection/single-HTTP/database/db.sqlite3"

#define DEFAULT_HT_S 10
#define DEFAULT_PORT "8888"
#define CONNECTION_TEMPLATE "Connection from %s for file %s"
#define USAGE_MSG "Usage: %s [-h] [-V] [-v] [-d[filename]] [-l <filepath>] [-s <configuration file>] [-u <unsigned int>] [-g <unsigned int>]\n"

#define BACKLOG 1
#define STR_MAX 2048
#define PORT_MIN 0
#define PORT_MAX 65536
#define MAX_ARGS 4
#define PACKET_MAX 1024
#define HTTP_VER_AMT 3
#define HTTP_REQ_AMT 8
#define REQLINE_TOKEN_AMT 3

#define MSG_LEN 4096
#define PORT_LEN 5
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
#define HTTP_METHOD_LEN 7
#define DEFAULT_PAGE_LEN 22
#define MDEFAULT_PAGE_LEN 2
#define CONNECTION_TEMPLATE_LEN 28
#define IMPLEMENTED_HTTP_METHODS_LEN 2

S_Ll _paths;
char _port[PORT_LEN] = DEFAULT_PORT,
	 _doc_root[PATH_MAX] = DEFAULT_ROOT;

bool sigint_flag = true;

bool is_valid_port(void) { // Done
	const int port_num = atoi(_port);

	return ((PORT_MIN < port_num) && (port_num < PORT_MAX));
}

bool is_valid_request(String *const reqline) { // Done
	const String restrict http_methods[HTTP_REQ_AMT] = {
		"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"
	};

	for (int i = 0; i < HTTP_REQ_AMT; i++) // Double check
		if (strncmp(reqline[0], http_methods[i], strnlen(http_methods[i], STR_MAX)) == 0)
			return true;

	const String restrict http_ver[HTTP_VER_AMT] = {"HTTP/1.0", "HTTP/1.1", "HTTP/2.0"};

	for (int i = 0; i < HTTP_VER_AMT; i++)
		if (strncmp(reqline[2], http_ver[i], strnlen(http_ver[i], STR_MAX)) == 0)
			return true;

	return false;
}

String clean_config_line(String string) { // Done

	while (*string < 'a' || *string > 'z')
		string++;

	String offset = strpbrk(string, " #\n");

	if (!offset)
		return string;
	*offset = '\0';

	return string;
}

void load_configuration(const String path) { // Done
	const String extension = strrchr(path, '.');

	if (!extension) {
		if (verbose_flag)
			puts(YELLOW "File Warning: -s option was not supplied a file" RESET);
		return;
	}

	if (strncmp(extension, ".conf", CONF_EXT_LEN) != 0) {
		if (verbose_flag)
			puts(YELLOW "File Warning: -s option takes a configuration file as an argument\n"
			       "Using default parameter values" RESET);
		return;
	}

	char buffer[KBYTE_S] = "";
	String line = "", defn = "", value = "";
	FILE *conf_f = fopen(path, "r");

	if ((!conf_f) && (verbose_flag))
		printf(YELLOW "File Error: %s\nUsing default parameter values\n" RESET, strerror(errno));
	else {
		HashTable hashtable = ht_create(DEFAULT_HT_S);

		while (fgets(buffer, KBYTE_S, conf_f)) {
			if (buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == '\t')
				continue;
			line = clean_config_line(buffer);
			defn = strtok(line, "=");
			value = strtok(NULL, "=");

			ht_insert(&hashtable, defn, value);
		}
		strncpy(_port, ht_get_value(hashtable, "port"), PORT_LEN);
		strncpy(_doc_root, ht_get_value(hashtable, "document_root"), PATH_MAX);
		strncpy(_log_root, ht_get_value(hashtable, "log_root"), PATH_MAX);
		strncpy(_db_path, ht_get_value(hashtable, "database_path"), PATH_MAX);
		ht_destroy(hashtable);
	}
	if ((fclose(conf_f) != 0) && (verbose_flag))
		printf(YELLOW "Configuration File Descriptor Error: %s\n" RESET, strerror(errno));
}

void compute_flags(const int argc, String *const argv, bool *v_flag) { // Done
	int c;
	uid_t euid; // Move inside assign block?
	gid_t egid;

	while ((c = getopt(argc, argv, "d::hl:Vvs:g:u:")) != -1) { // : at the start?
		switch (c) {
		case 'h':
			printf(USAGE_MSG
			       "-d\tDump the entire database or a specified table to a file\n"
			       "-l\tLoad a database fixture\n"
				   "-h\tHelp menu\n"
				   "-V\tVersion\n"
				   "-v\tVerbose\n"
				   "-s\tLoad a configuration file\n"
				   "-u\tSet the effective user id for the process\n"
				   "-g\tSet the effective group if for the process\n", basename(argv[0]));
			exit(EXIT_SUCCESS);
		case 'd':
			sqlite_dumpdata(argv);
			exit(EXIT_SUCCESS);
		case 'l':
			sqlite_load_exec(optarg);
			exit(EXIT_SUCCESS);
		case 'V':
			puts("Version 0.6");
			exit(EXIT_SUCCESS);
		case 'v':
			*v_flag = true;

			break;
		case 's':
			load_configuration(optarg);
			break;
		case 'u':
			euid = atoi(optarg);

			if (seteuid(euid) == -1)
				printf(YELLOW "EUID Error: %s\n" RESET, strerror(errno));
			break;
		case 'g':
			egid = atoi(optarg);

			if (setegid(egid) == -1)
				printf(YELLOW "EGID Error: %s\n" RESET, strerror(errno));
			break;
		default:
			fprintf(stderr, RED "Getopt Option Error: Unrecognized option: -%c\n" RESET, optopt);
			exit(EXIT_FAILURE);
		}
	}
}

void process_php(const int client_fd, const String file_path) { // Done
	const pid_t c_pid = fork();

	if (c_pid == -1) {
		const String err_msg = strerror(errno);

		if (verbose_flag)
			printf(YELLOW "Process Forking Error: %s\n" RESET, err_msg);
		server_log(err_msg);
	}

	if (c_pid == 0) {
		dup2(client_fd, STDOUT_FILENO);
		execl("/usr/bin/php", "php", file_path, (String) NULL);
	}
	if ((close(client_fd) == -1) && (verbose_flag))
		printf(YELLOW "File Descriptor Error 3: %s\n" RESET, strerror(errno));
}

void send_file(const int client_fd, const String path) { // Done
	const int fd = open(path, O_RDONLY);

	if (fd == -1) {
		const String err_msg = strerror(errno);

		if (verbose_flag)
			printf(YELLOW "Serve File Error: %s\n" RESET, err_msg);
		server_log(err_msg);
	} else {
		String f_contents = (String) malloc((PACKET_MAX + NT_LEN) * sizeof(char));

		if (!f_contents) {
			fprintf(stderr, RED "Memory Error: %s\n" RESET, strerror(errno));
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

	if ((close(fd) == -1) && (verbose_flag))
		printf(YELLOW "Copy File Descriptor Error: %s\n" RESET, strerror(errno));

	if ((close(client_fd) == -1) && (verbose_flag))
		printf(YELLOW "Serve File Descriptor Error: %s\n" RESET, strerror(errno));
}

void respond(const int client_fd, String *const reqlines, const String path) { // Done
	if (!is_valid_request(reqlines)) {
		if (verbose_flag)
			printf("%s %s [400 Bad Request]\n", reqlines[0], reqlines[1]);
		send(client_fd, BAD_REQUEST, CODE_400_LEN, 0);
		send_file(client_fd, "partials/code-responses/400.html");
		return;
	}

	if (strncmp(reqlines[2], "HTTP/2.0", HTTP_VER_LEN) == 0) {
		if (verbose_flag)
			printf("GET %s %s [505 Http Version Not Supported]\n", reqlines[1], reqlines[2]);
		send(client_fd, NOT_SUPPORTED, CODE_505_LEN, 0);
		send_file(client_fd, "partials/code-responses/505.html");
		return;
	}

	const String restrict implemented_http_methods[IMPLEMENTED_HTTP_METHODS_LEN] = {"GET", "POST"};
	bool in = false;

	for (unsigned int i = 0; i < IMPLEMENTED_HTTP_METHODS_LEN; i++)
		if (strncmp(reqlines[0], implemented_http_methods[i], HTTP_METHOD_LEN) == 0) {
			in = true;
			break;
		}

	if (!in) {
		if (verbose_flag)
			printf("%s %s [501 Not Implemented]\n", reqlines[0], reqlines[1]);
		send(client_fd, NOT_IMPLEMENTED, CODE_501_LEN, 0);
		send_file(client_fd, "partials/code-responses/501.html");
		return;
	}

	const int fd = open(path, O_RDONLY);

	if (fd != -1) {
		if ((close(fd) == -1) && (verbose_flag))
			printf(YELLOW "File Descriptor Error 1: %s\n" RESET, strerror(errno));

		if (verbose_flag)
			printf(GREEN "GET %s [200 OK]\n" RESET, reqlines[1]);
		send(client_fd, OK, CODE_200_LEN, 0);
		const String extension = strrchr(path, '.');

		if (strncmp(extension, ".php", PHP_EXT_LEN) == 0)
			process_php(client_fd, path);
		else
			send_file(client_fd, path);
	}
	else if (errno == ENOENT) {
		if (verbose_flag)
			printf("GET %s [404 Not Found]\n", reqlines[1]);
		send(client_fd, NOT_FOUND, CODE_404_LEN, 0);
		send_file(client_fd, "partials/code-responses/404.html");
	}
	else if (errno == EACCES) {
		if (verbose_flag)
			printf(YELLOW "GET %s [403 Access Denied]\n" RESET, reqlines[1]);
		send(client_fd, FORBIDDEN, CODE_403_LEN, 0);
		send_file(client_fd, "partials/code-responses/403.html");
	}
	else {
		if (verbose_flag)
			printf(RED "GET %s [500 Internal Server Error]\n" RESET, reqlines[1]);
		send(client_fd, SERVER_ERROR, CODE_500_LEN, 0);
		send_file(client_fd, "partials/code-responses/500.html");
	}
}

String *get_req_lines(String msg) { // Done
	String *const reqline = (String*) malloc(REQLINE_TOKEN_AMT * sizeof(String));

	if (!reqline) {
		fprintf(stderr, RED "Memory Error: %s\n" RESET, strerror(errno));
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < REQLINE_TOKEN_AMT; i++) {
		reqline[i] = (String) calloc(REQLINE_LEN + NT_LEN, sizeof(char));

		if (!reqline[i]) {
			fprintf(stderr, RED "Memory Error: %s\n" RESET, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	String tok = strtok(msg, " \t\n");

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

void free_req_lines(String *reqline) { // Done
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

void init_url_paths() {
	s_ll_insert(_paths, "/", "static/html/index.html");
	s_ll_insert(_paths, "/index", "static/html/index.html");
	s_ll_insert(_paths, "/login", "views/login.php");
	s_ll_insert(_paths, "/contact", "static/html/contact.html");
	s_ll_insert(_paths, "/forbidden", "static/html/forbidden.html");
}

void init_addrinfo(struct addrinfo *const addressinfo) { // Done
	memset(addressinfo, 0, sizeof(*addressinfo));
	(*addressinfo).ai_family = AF_INET6; // IPV4 & IPV6
	(*addressinfo).ai_socktype = SOCK_STREAM; // TCP
	(*addressinfo).ai_flags = AI_PASSIVE; // Gen socket
}

int get_socket(int *const socketfd, struct addrinfo *const serviceinfo) { // Done
	const short yes = 1;
	const struct addrinfo *p;

	for (p = serviceinfo; p; p = p->ai_next) {
		*socketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		if (*socketfd == -1) {
			if (verbose_flag)
				printf(YELLOW "Socket Init Error: %s\n" RESET, strerror(errno));
			continue;
		}

		if (setsockopt(*socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			fprintf(stderr, RED "Setsocket Error: %s\n" RESET, strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (bind(*socketfd, p->ai_addr, p->ai_addrlen) == -1) {
			if (verbose_flag)
				printf(YELLOW "Bind Error: %s\n" RESET, strerror(errno));
			if ((close(*socketfd) == -1) && (verbose_flag))
				printf(YELLOW "Bind File Descriptor Error: %s\n" RESET, strerror(errno));
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
		fprintf(stderr, RED "Sigal Error: %s\n" RESET, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

bool is_image(const String restrict extension) {
	const String img_ext[] = {".png", ".jpg", ".jpeg", ".tiff", ".gif", ".bmp", ".svg", ".ico"};
	const size_t img_ext_len = sizeof(img_ext) / sizeof(String);

	for (unsigned int i = 0; i < img_ext_len; i++)
		if (strncmp(extension, img_ext[i], CONF_EXT_LEN) == 0)
			return true;
	return false;
}

bool is_audio(const String restrict extension) {
	const String img_ext[] = {".wav", ".flac", ".opus", ".mp3", ".aac", ".ogg", ".pcm", ".aiff", ".wma", ".alac"};
	const size_t img_ext_len = sizeof(img_ext) / sizeof(String);

	for (unsigned int i = 0; i < img_ext_len; i++)
		if (strncmp(extension, img_ext[i], CONF_EXT_LEN) == 0)
			return true;
	return false;
}

bool is_video(const String restrict extension) {
	const String img_ext[] = {".mp4", ".mov", ".avi", ".flv", ".wmv"};
	const size_t img_ext_len = sizeof(img_ext) / sizeof(String);

	for (unsigned int i = 0; i < img_ext_len; i++)
		if (strncmp(extension, img_ext[i], CONF_EXT_LEN) == 0)
			return true;
	return false;
}

bool is_binary(const String restrict extension) {
	const String img_ext[] = {".exe", ".img", ".bin"};
	const size_t img_ext_len = sizeof(img_ext) / sizeof(String);

	for (unsigned int i = 0; i < img_ext_len; i++)
		if (strncmp(extension, img_ext[i], CONF_EXT_LEN) == 0)
			return true;
	return false;
}

void process_request(const int fd, String msg, const String ipv6_address) { // Done
	char con_msg[CONNECTION_TEMPLATE_LEN + PATH_MAX],
		 **const reqlines = get_req_lines(msg);

	if (!reqlines) {
		snprintf(con_msg, CONNECTION_TEMPLATE_LEN + INET6_ADDRSTRLEN, "Connection from %s; BAD REQUEST", ipv6_address);

		if (verbose_flag)
			printf(YELLOW "%s\n" RESET, con_msg);
		server_log(con_msg);
		send(fd, BAD_REQUEST, CODE_400_LEN, 0);
		send_file(fd, "partials/code-responses/400.html");
	} else {
		S_Ll_Node data;
		const String restrict extension = strrchr(reqlines[1], '.');

		if (extension) {
			if (strncmp(extension, ".css", CONF_EXT_LEN) == 0)
				strncat(_doc_root, "static/css/", PATH_MAX);
			else if (strncmp(extension, ".js", CONF_EXT_LEN) == 0)
				strncat(_doc_root, "static/javascript/", PATH_MAX);
			else if (is_image(extension))
				strncat(_doc_root, "static/images/", PATH_MAX);
			else if (is_video(extension))
				strncat(_doc_root, "static/video/", PATH_MAX);
			else if (is_binary(extension))
				strncat(_doc_root, "static/binary/", PATH_MAX);
			else if (is_audio(extension))
				strncat(_doc_root, "static/audio/", PATH_MAX);
			strncat(_doc_root, reqlines[1], PATH_MAX);
		}
		else if ((data = s_ll_find(_paths, reqlines[1])))
			strncat(_doc_root, data->path, PATH_MAX);
		snprintf(con_msg, CONNECTION_TEMPLATE_LEN + PATH_MAX, CONNECTION_TEMPLATE, ipv6_address, reqlines[1]);

		if (verbose_flag)
			printf("%s\n", con_msg);
		server_log(con_msg);
		respond(fd, reqlines, _doc_root);
	}
	free_req_lines(reqlines);
}

int main(const int argc, String *const argv) {
	char ipv6_address[INET6_ADDRSTRLEN];
	int masterfd, newfd;
	struct addrinfo addressinfo, *serviceinfo;
	struct sockaddr_in6 client_addr;
	socklen_t sin_size = sizeof(client_addr); // Does this change with every connection?
	const mode_t mode_d = 0770;

	verbose_flag = true;
	_paths = s_ll_create();
	strncpy(_log_root, DEFAULT_LOG_ROOT, PATH_MAX);
	strncpy(_db_path, DEFAULT_DB_ROOT, PATH_MAX);

	init_signals();
	if (argc > MAX_ARGS) {
		printf(USAGE_MSG, basename(argv[0]));
		exit(EXIT_FAILURE);
	}

	compute_flags(argc, argv, &verbose_flag);
	if (!is_valid_port()) {
		fprintf(stderr, RED "Port Error: Invalid port %s\n" RESET, _port);
		exit(EXIT_FAILURE);
	}

	if ((mkdir(_log_root, mode_d) == -1) && (verbose_flag))
		if (errno != EEXIST) {
			printf("Log Directory Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

	init_addrinfo(&addressinfo);

	if (getaddrinfo(NULL, _port, &addressinfo, &serviceinfo) != 0) {
		fprintf(stderr, RED "Get Address Info Error: %s\n" RESET, gai_strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (get_socket(&masterfd, serviceinfo) == -1)
		exit(EXIT_FAILURE);

	if (listen(masterfd, BACKLOG) == -1) {
		fprintf(stderr, RED "Listen Error: %s\n" RESET, strerror(errno));
		exit(EXIT_FAILURE);
	}

	init_url_paths();

	String msg = (String) malloc((MSG_LEN + NT_LEN) * sizeof(char));

	if (!msg) {
		fprintf(stderr, RED "Memory Error: %s\n" RESET, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (verbose_flag)
		printf(GREEN "Initialization: SUCCESS;\n"
		       "Listening on port: %s\n"
		       "root is: %s\n"
		       "log root is: %s\n" RESET, _port, _doc_root, _log_root);

	const int default_root_len = strnlen(_doc_root, PATH_MAX);

	while (sigint_flag) {
		_doc_root[default_root_len] = '\0';
		newfd = accept(masterfd, &client_addr, &sin_size);

		if (newfd == -1) {
			const String err_msg = strerror(errno);

			if (verbose_flag)
				printf(YELLOW "Accept Error: %s\n" RESET, err_msg);
			server_log(err_msg);
			continue;
		}

		inet_ntop(AF_INET6, &(((struct sockaddr_in6*)&client_addr)->sin6_addr), ipv6_address, INET6_ADDRSTRLEN);

		if (recv(newfd, msg, MSG_LEN, 0) > 0)
			process_request(newfd, msg, ipv6_address);
		else {
			const String err_msg = strerror(errno);

			if (verbose_flag)
				printf(YELLOW "Inboud Data Read Error: %s\n" RESET, err_msg);
			server_log(err_msg);
		}
	}
	s_ll_destroy(_paths);

	if ((close(masterfd) == -1) && (verbose_flag))
		printf(YELLOW "Master File Descriptor Error: %s\n" RESET, strerror(errno));
	free(msg);
	msg = NULL;

	return EXIT_SUCCESS;
}

