#include "main.h"
#include <sys/sysctl.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int builtin_free(int argc, char** argv) {
	bool option_h = 0;
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0) {
			option_h = 1;
		} else if (strcmp(argv[i], "-?") == 0) {
			show_help(9);
			return 0;
		}
	}
	
	struct mstat mbuff;
	sysctl(1, &mbuff, 0);
	if (option_h) { //-h
		printf("total: %d KiB, used: %d KiB, free: %d KiB\n", mbuff.bytes_total/1024, mbuff.bytes_used/1024, (mbuff.bytes_total - mbuff.bytes_used)/1024);
	} else {
		printf("total: %d, used: %d, free: %d\n", mbuff.bytes_total, mbuff.bytes_used, mbuff.bytes_total - mbuff.bytes_used);
	}
	return 0;
}


