#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parse.h"
#include "engine.h"

int main() {

    Database database = start_game();

    command *new_command = create_new_command();
    int w = -1, counter = 0;

    while (1) {

        int tmpn = 0;
        char tmp1;
        if ((tmp1 = (char) getc(stdin)) == EOF) break;
        w = 0;
        counter++;
        char tmp2[MAX];
        memset(tmp2, '\0', MAX);

        while (tmp1 != '\n') {
            if (tmpn == MAX) {
                end_game(w, database);
                input_error(new_command);
            }
            tmp2[tmpn] = tmp1;
            tmp1 = (char) getc(stdin);
            tmpn++;
        }
        tmp2[tmpn] = '\0';

        new_command = parse_command(tmp2, new_command);

        if ((counter == 1 || counter == 2) && strcmp(new_command->name, "INIT") != 0) {
            end_game(w, database);
            input_error(new_command);
        }
        if (strcmp(new_command->name, "X") == 0) {
            end_game(w, database);
            input_error(new_command);
        }

        if (strcmp(new_command->name, "INIT") == 0) {
            w = init(new_command->data[0],
                     new_command->data[1],
                     new_command->data[2],
                     new_command->data[3],
                     new_command->data[4],
                     new_command->data[5],
                     new_command->data[6],
                     database);
        }
        else if (strcmp(new_command->name, "MOVE") == 0) {
            w = move(new_command->data[0],
                     new_command->data[1],
                     new_command->data[2],
                     new_command->data[3],
                     database);
        }
        else if (strcmp(new_command->name, "PRODUCE_KNIGHT") == 0) {
            w = produce_knight(new_command->data[0],
                               new_command->data[1],
                               new_command->data[2],
                               new_command->data[3],
                               database);
        }
        else if (strcmp(new_command->name, "PRODUCE_PEASANT") == 0) {
            w = produce_peasant(new_command->data[0],
                                new_command->data[1],
                                new_command->data[2],
                                new_command->data[3],
                                database);
        }
        else if (strcmp(new_command->name, "END_TURN") == 0) {
            w = end_turn(database);
        }
        else {
            end_game(w, database);
            input_error(new_command);
        }

        // 0 - without printing
        // 1 - with printing

        // 11 - player 1 won
        // 12 - player 2 won
        // 13 - draw

        // 42 - input error

        if (w == 42) {
            end_game(w, database);
            input_error(new_command);
        }
        if (w == 11 || w == 12) {
            print_topleft(database);
            break;
        }
        if (w == 13) break;

        if (w == 1) print_topleft(database);

        memset(new_command, 0, sizeof(struct def_command));
    }
    if (w == -1) {
        end_game(w, database);
        input_error(new_command);
    }

    delete_command(new_command);
    end_game(w, database);

    return 0;
}