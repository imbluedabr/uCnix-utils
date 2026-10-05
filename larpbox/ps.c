#include "main.h"
#include <sys/sysctl.h>
#include <stdio.h>
#include <string.h>

const char states[][8] = {
	"ZOMBIE",
	"BLOCKED",
	"READY"
};

int builtin_ps(int argc, char** argv) {

	if (argc > 1) {
		if (strcmp(argv[1], "-?") == 0) {
			show_help(8);
			return 0;
		}
	}

	struct pstat pbuff[4];
	
	puts("PID\tPPID\tPGRP\tSTATE\n");
	int n = sysctl(0, &pbuff, 4);
	for (int i = 0; i < n; i++) {
		printf("%d\t%d\t%d\t%s\n", pbuff[i].pid, pbuff[i].ppid, pbuff[i].pgrp, states[pbuff[i].state]);
	}
	return 0;
}




