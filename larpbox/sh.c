#include "main.h"

#include <unistd.h>
#include <sys/utsname.h>
#include <sys/spawn.h>
#include <sys/dir.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static char path_buff[32];

int shell_exit;

int builtin_exit(int argc, char** argv) {
    shell_exit = 1;
    return 0;
}

int builtin_pwd(int argc, char** argv) {
	if (argc == 2) {
		if (strcmp(argv[1], "-?") == 0) {
			show_help("pwd");
			return 0;
		}
	}
    printf("%s\n", getcwd(path_buff, sizeof(path_buff)));
    return 0;
}

int builtin_cd(int argc, char** argv) {
    if (argc != 2) {
        puts("cd: invalid argument(s)\n");
        return -1;
    }
	if (strcmp(argv[1], "-?") == 0) {
		show_help("cd");
		return 0;
	}
    int status = chdir(argv[1]);

    if (status == -ENOENT) {
        puts("cd: no such file or directory\n");
    } else if (status == -ENOTDIR) {
        puts("cd: not a directory\n");
    }
    return status;
}

char* strip_path(char* str) {
    char* word = str;
    while(*str) {
        if (*str == '/') {
            memset(word, 0, 1 + str - word);
            word = str + 1;
        }
        str++;
    }
    return word;
}

int spawn_command(char** argv) {
    fd_set fds;
    FD_ZERO(fds);
    FD_SET(STDIN_FILENO, fds);
    FD_SET(STDOUT_FILENO, fds);
    FD_SET(STDERR_FILENO, fds);
	for (int i = 0; argv[i] != NULL; i++) {
		printf("arg%d: %s\n", i, argv[i]);
	}
    pid_t pid = spawn(argv[0], &fds, (const char**) argv);
	printf("path: %s, pid: %d\n", argv[0], pid);
    if (pid < 0) return pid;
    setpgid(pid, pid);
    kill(pid, SIGCONT);
    return pid;
}

void execute_command(char* buffer)
{
    static char* arg_vec[8];
    
    int argc = 0;
    char* current_word = buffer;
    while(*buffer) { //handle endline character
        if (*buffer == '\n') {
            *buffer = '\0';
        }
        if (*buffer == ' ') {
            *buffer = '\0';
            arg_vec[argc++] = current_word;
            current_word = buffer + 1;
        }
        if (argc > 7) break;
        buffer++;
    }
    if (current_word != buffer && *current_word) arg_vec[argc++] = current_word;
    arg_vec[argc] = NULL;
    
    if (argc == 0) return;

    int builtin = get_builtin_index(arg_vec[0]);
    if (builtin > -1 && builtin < SHELL_BUILTIN_COUNT) {
        execute_builtin(builtin, argc, arg_vec);
        return;
    }
	
    pid_t pid = spawn_command(arg_vec);
    if (pid < 0) {
        memset(path_buff, 0, sizeof(path_buff));
        snprintf(path_buff, sizeof(path_buff), "/bin/%s", arg_vec[0]);
		arg_vec[0] = path_buff;
        pid = spawn_command(arg_vec);
    }
    
    if (pid < 0) {
        printf("sh: %s: command not found\n", arg_vec[0]);
    } else {
        int wstatus;
        waitpid(pid, &wstatus, 0);

        printf("exit: %d\n", wstatus);
    }
}


char line_buff[32];

int builtin_sh(int argc, char** argv)
{
    shell_exit = 0;
    while (shell_exit == 0) {
        puts("# ");
        memset(line_buff, 0, sizeof(line_buff));
        if (read(STDIN_FILENO, line_buff, sizeof(line_buff)) == 0) break; //CTRL+D
        execute_command(line_buff);
    }
    puts("exit\n");
    return 0;
}

