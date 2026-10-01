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
#include <stdbool.h>

typedef struct {
	char line[32];
	enum {
		SESS_ONESHOT,
		SESS_RESTART
	} type;  //0: oneshot, 1: restart
	char tty[8];
	pid_t pid;
} session_t;

session_t sessions[8];
int session_count;
static char text_buff[64];


//parse line in the initrc
//tty:type:command
void parse_line(char* line) {
    char *arg_vec[4];
    int arg_cnt = 0;
    char *p = line;
    char *start = p;

    while (*p) {
        if (*p == ':') {
            *p = '\0';
            arg_vec[arg_cnt++] = start;
            start = p + 1;
        }
        p++;
    }

    if (*start) {
		arg_vec[arg_cnt++] = start;
	}

	arg_vec[arg_cnt] = NULL;

	if (arg_cnt < 3) return;
	session_t* sess = &sessions[session_count++];

	strlcpy(sess->tty, arg_vec[0], 7);

	if (strcmp(arg_vec[1], "oneshot") == 0) {
		sess->type = SESS_ONESHOT;
	} else if (strcmp(arg_vec[1], "restart") == 0) {
		sess->type = SESS_RESTART;
	}
	
	strlcpy(sess->line, arg_vec[2], 31);

	printf("sess: tty=%s, type=%s, command=%s\n", sess->tty, arg_vec[1], sess->line);
}


//start a session
void start_session(session_t* sess) {
    char *arg_vec[4];
    int arg_cnt = 0;
    char *p = sess->line;
    char *start = p;

    while (*p) {
        if (*p == ' ') {
            *p = '\0';
            arg_vec[arg_cnt++] = start;
            start = p + 1;
        }
        p++;
    }

    if (*start) {
        arg_vec[arg_cnt++] = start;
	}
	arg_vec[arg_cnt] = NULL;
	
	//tty the process is attached to;
	snprintf(text_buff, 32, "/dev/%s", sess->tty);
	int stdin = open(text_buff, O_RDWR);
	if (stdin < 0) {
		printf("init: failed to open tty\n");
		return;
	}
	int stdout = fcntl(stdin, F_DUPFD, 0);
	int stderr = fcntl(stdin, F_DUPFD, 0);
	
	//executable to run
    fd_set fds;
    FD_ZERO(fds);
    FD_SET(stdin, fds);
    FD_SET(stderr, fds);
    FD_SET(stderr, fds);
    pid_t pid = spawn(arg_vec[0], &fds, (const char**)arg_vec);
	printf("init: sess=%d\n", pid);
	sess->pid = pid;

	if (pid > 0) {
		struct utsname ubuff;
		uname(&ubuff);
		snprintf(text_buff, 64, "%s %s %s %s %s (%s)\n\n", ubuff.sysname, ubuff.nodename, ubuff.release, ubuff.version, ubuff.machine, sess->tty);
		write(stdout, text_buff, strlen(text_buff));

		kill(pid, SIGCONT);
	}
	close(stdin);
	close(stdout);
	close(stderr);

	if (sess->type == SESS_ONESHOT) { //oneshot so we have to wait for it to be finished
		int wstatus;
		waitpid(pid, &wstatus, 0);
		sess->pid = 0;
	}
}

int main(int argc, char** argv)
{
    if (argc == 2) {
        if (strcmp(argv[1], "halt") == 0) {
            kill(1, SIGKILL);
            return 0;
        }
		return 1;
    }

    int inittab = open("/etc/initrc", O_RDONLY);
    if (inittab < 0) {
        puts("init: cant open initrc\n");
        return 1;
    }
	
	//parse initrc
    char buf[64];
    char line[32];
    int line_len = 0;
    session_count = 0;

    while (1) {
        int n = read(inittab, buf, sizeof(buf));
        if (n <= 0) break;

        for (int i = 0; i < n; i++) {
            char ch = buf[i];

            if (ch == '\n') {
                line[line_len] = '\0';
                parse_line(line);
                line_len = 0;
            } else {
                if (line_len < sizeof(line) - 1)
                    line[line_len++] = ch;
            }
        }
    }

    close(inittab);
    
    if (session_count == 0) return 1;

	

    puts("init: starting sessions\n");
    for (int i = 0; i < session_count; i++) {
		session_t* sess = &sessions[i];
		start_session(sess);
    }
    
	int wstatus;
	int pid;
    while((pid = waitpid(-1, &wstatus, 0)) > 0) {
    	for (int i = 0; i < session_count; i++) {
    		session_t* sess = &sessions[i];
			if (sess->pid == pid && sess->type == SESS_RESTART) {
				start_session(sess);
			}
		}
	}
    
    return 0;
}

