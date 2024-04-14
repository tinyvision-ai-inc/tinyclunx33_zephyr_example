/*
 * Prints the throughput in bytes per second of the input file by reading it
 * continuously with time is measured.
 *
 * cc -o throughput throughput.c
 */

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

struct timespec ts_prev;
struct timespec ts_now;

static void usage(char const *argv0)
{
	fprintf(stderr, "usage: %s <file_to_read> <block_size>\n", argv0);
	exit(1);
}

#define USEC_PER_SEC	1000 * 1000
#define NSEC_PER_USEC	1000
#define CK(expr)	check(expr, #expr)

static inline int check(int ret, char *str)
{
	if (ret < 0) {
		perror(str);
		exit(1);
	}
	return ret;
}

static void measure(char *scratch_buf, size_t scratch_len, int fd)
{
	unsigned long usec_per_slice = 0;
	unsigned long bytes_per_slice = 0;
	unsigned long bytes_per_sec = 0;

	memcpy(&ts_prev, &ts_now, sizeof(ts_prev));
	CK(clock_gettime(CLOCK_MONOTONIC_COARSE, &ts_now));

	while (bytes_per_slice < scratch_len) {
		bytes_per_slice += CK(read(fd, scratch_buf, scratch_len));
	}

	usec_per_slice = (ts_now.tv_sec - ts_prev.tv_sec) * USEC_PER_SEC +
		(ts_now.tv_nsec - ts_prev.tv_nsec) / NSEC_PER_USEC;
	bytes_per_sec = bytes_per_slice * USEC_PER_SEC / usec_per_slice;

	printf("%ld\n", bytes_per_sec);
	fflush(stdout);
}

int main(int argc, char **argv)
{
	struct termios tio = {0};
	char *scratch_buf;
	size_t scratch_len;
	int fd;

	if (argc != 3) {
		usage(argv[0]);
	}

	/* Open the serial port */
	fd = CK(open(argv[1], O_RDONLY));

	/* Scratch buffer size */
	scratch_len = strtoul(argv[2], NULL, 10);
	if (scratch_len == 0) {
		usage(argv[0]);
	}

	/* Prepare the IOCTL options */
	cfmakeraw(&tio);
	cfsetospeed(&tio, B115200);
	cfsetispeed(&tio, B115200);
	CK(tcsetattr(fd, TCSANOW, &tio));

	scratch_buf = malloc(scratch_len);
	if (scratch_buf == NULL) {
		perror("allocating scratch buffer");
		exit(1);
	}

	while (1) {
		measure(scratch_buf, scratch_len, fd);
	}
	return 0;
}
