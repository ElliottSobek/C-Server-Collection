#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>

#include "globals.h"
#include "types.h"
#include "colors.h"

#define FTIME_MLEN 25
#define FF_TIME_PATH_MLEN 19

void server_log(const String restrict msg) {
	const mode_t mode_d = 0770, mode_f = 0660;
	const time_t cur_time = time(NULL);
	String log_dir = (String) calloc(PATH_MAX + NT_LEN, sizeof(char)),
		f_time = (String) malloc((FTIME_MLEN + NT_LEN) * sizeof(char));
	char ff_time_path[FF_TIME_PATH_MLEN + NT_LEN];
	const struct tm *const t_data = localtime(&cur_time);

	if (!log_dir || !f_time) {
		fprintf(stderr, RED "Memory Error: %s\n" RESET, strerror(errno));
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

	if ((fd == -1) && (_verbose_flag))
		printf(YELLOW "Logging File Error: %s\n" RESET, strerror(errno));
	else
		dprintf(fd, "[%s]: %s\n", f_time, msg);

	if ((close(fd) == -1) && (_verbose_flag))
		printf(YELLOW "Logging File Descriptor Error: %s\n" RESET, strerror(errno));

	free(f_time);
	f_time = NULL;

	free(log_dir);
	log_dir = NULL;
}
