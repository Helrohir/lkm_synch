/*
 * mysyscallUsr1.c
 *
 *  Created on: Oct 16, 2012
 *      Author: helrohir
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "barrier.h"

#define GETBARRIER "-get"
#define SLEEPON "-sleepon"
#define WAKEUP "-wakeup"
#define RELEASE "-release"

int main(int argc, char **argv) {

	if (argc < 2) {
		printf("ERROR: Too few arguments (%d of at least 2)\n", argc);
		return 0;
	}
	int ret = 0;
	if (strcmp(argv[1], GETBARRIER) == 0) {
		if (argc < 4) {
			printf(
					"ERROR: Too few arguments (%d of at least 3: command key flags)\n",
					argc - 1);
			return 0;
		}

		int key = atoi(argv[2]);
		int flags = atoi(argv[3]);
		ret = get_barrier(key, flags);

	} else if (strcmp(argv[1], SLEEPON) == 0) {
		if (argc < 4) {
			printf(
					"ERROR: Too few arguments (%d of at least 3: command bd tag)\n",
					argc - 1);
			return 0;
		}

		int bd = atoi(argv[2]);
		int tags = atoi(argv[3]);
		ret = sleep_on_barrier(bd, tags);

	} else if (strcmp(argv[1], WAKEUP) == 0) {
		if (argc < 4) {
			printf(
					"ERROR: Too few arguments (%d of at least 3: command bd tag)\n",
					argc - 1);
			return 0;
		}

		int bd = atoi(argv[2]);
		int tags = atoi(argv[3]);
		ret = awake_barrier(bd, tags);

	} else if (strcmp(argv[1], RELEASE) == 0) {
		if (argc < 3) {
			printf("ERROR: Too few arguments (%d of at least 3: command bd )\n",
					argc - 1);
			return 0;
		}

		int bd = atoi(argv[2]);
		ret = release_barrier(bd);

	} else {
		printf("ERROR: Command not recognized\n");
	}

	printf("RETURNED VALUE: %d\n", ret);
	return ret;
}
