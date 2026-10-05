#pragma once

#include <stdint.h>
#define SHELL_BUILTIN_COUNT 10
#define BUILTIN_COUNT 11
extern const char builtin_names[][BUILTIN_COUNT]; 
extern char line_buff[32];

int builtin_exit(int argc, char** argv);
int builtin_pwd(int argc, char** argv);
int builtin_cd(int argc, char** argv); 
int builtin_cat(int argc, char** argv); 
int builtin_ls(int argc, char** argv);
int builtin_uname(int argc, char** argv);
int builtin_kill(int argc, char** argv);
int builtin_help(int argc, char** argv);
int builtin_ps(int argc, char** argv);
int builtin_free(int argc, char** argv);
int builtin_sh(int argc, char** argv);


typedef struct {
	char flag;
	uint8_t index; //argv index to the value of the argument or flag
} arg_t;

typedef struct {
	uint8_t builtin_index;
	uint8_t filecount;
	uint8_t files[6];
} arg_parser_t;

//parse arguments from the argument list. 
int arg_parse(arg_parser_t* parser, arg_t* arg_list, int arg_list_size, char** argv);

void show_help(int command);
char* strip_path(char* str); 
int get_builtin_index(const char* name); 
int execute_builtin(int index, int argc, char** argv);

