#ifndef GET_STRING_H
#define GET_STRING_H

#include "extra_lists.h"

void create_head(record* head);

record* get_record(record* head, char* fileName);

char** get_command(char* command, int num);

char** get_command_monitor(char* command, int num);

char** get_date(char* date);

#endif