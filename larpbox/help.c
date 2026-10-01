#include "main.h"
#include <stdio.h>

static const char* const help_strings[] = {
	"exit\nExit the shell.\n",
	"ls [OPTION]... [FILE]\nList directory contents.\nOptions:\n\t-a\tdo not ignore entries starting with .\n\t-l\tuse long listing format\n",
	"cd [dir]\nChange the shell working directory.\n",
	"pwd\nPrint current working directory to stdout.\n",
	"cat [FILE]...\nConcatenate files to stdout.\n",
	"uname [OPTION]...\nPrint system info to stdout.\nOptions:\n\t-a\tshow all information\n\t-s\tkernel name\n\t-n\tnetwork node hostname\n\t-r\tkernel release\n\t-v\tkernel version\n\t-m\tmachine\n",
	"kill [-s signal] [PID]...\nSend signal to a process.\n",
	"help [COMMAND]\nShow command info.\n",
	"sh\nStandard system shell.\n"
};

void show_help(const char* command) {
	int n = get_builtin_index(command);
	if (n >= BUILTIN_COUNT || n < 0) {
		printf("help: no such command\n");
		return;
	}
	printf("Usage: %s\n", help_strings[n]);
}

int builtin_help(int argc, char **argv) {
	if (argc == 2) {
		show_help(argv[1]);
	} else {
		for (int i = 0; i < BUILTIN_COUNT; i++) {
			show_help(builtin_names[i]);
		}
	}
	return 0;
}



