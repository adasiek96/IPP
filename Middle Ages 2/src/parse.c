/** @file
   Implementation of the parser.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

command* create_new_command() {
    command *new_command;
    new_command = (command*) malloc(sizeof(struct def_command));
    memset(new_command, 0, sizeof(struct def_command));
    return new_command;
}

void delete_command(command* this_command) {
    free(this_command);
}

void input_error(command* this_command) {
    delete_command(this_command);
    fprintf(stderr, "%s\n", "input error");
    exit(42);
}

command* send_error_info(command* new_command) {
    new_command->name[0] = 'X';
    return new_command;
}

command* parse_command(char tmp[], command* new_command) {

    //reading the name of the function
    int i = 0;
    do {
        if (i == 15) {
            send_error_info(new_command);
            return new_command;
        }
        new_command->name[i] = tmp[i];
        i++;
    } while (tmp[i] != ' ' && tmp[i] != '\0');
    new_command->name[i] = '\0';


    char c[MAX];
    int j = 0;

    //reading parameters of the function
    while (tmp[i] != '\0' && j <= 7) {

        //reading the next parameter
        i++;
        memset(c, '\0', MAX);
        int k = 0;
        do {
            c[k] = tmp[i];
            k++;
            i++;
        } while (tmp[i] != '\0' && tmp[i] != ' ');
        c[k] = '\0';

        char d[k];
        memset(d, '\0', (size_t)(k + 1));
        int p = 0;
        while (p < k) {
            if ((int)c[p] < 48 || (int)c[p] > 57 || c[p] == '\0') {
                send_error_info(new_command);
                return new_command;
            }
            d[p] = c[p];
            p++;
        }
        if (atoi(d) == -1) {
            send_error_info(new_command);
            return new_command;
        }
        if (j == 7) {
            send_error_info(new_command);
            return new_command;
        }
        new_command->data[j] = atoi(d);
        j++;
    }

    //checking the correctness
    char *f = new_command->name;
    if (strcmp(f, "INIT") == 0 && new_command->data[6] == 0) {
        send_error_info(new_command);
        return new_command;
    }
    if ((strcmp(f, "MOVE") == 0 || strcmp(f, "PRODUCE_KNIGHT") == 0 || strcmp(f, "PRODUCE_PEASANT") == 0)
        && (new_command->data[4] != 0 || new_command->data[3] == 0)) {
        send_error_info(new_command);
        return new_command;
    }
    if (strcmp(f, "END_TURN") == 0 && new_command->data[0] != 0) {
        send_error_info(new_command);
        return new_command;
    }

    return new_command;
}