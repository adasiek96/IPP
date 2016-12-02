/** @file
   Interface of the parser.

*/

#ifndef PARSE_H
#define PARSE_H

/** The maximum length of the correct command.
 */
#define MAX 102

/** The structure in which the data of the command is stored.
 */
typedef struct def_command {
    /** The name of the command.
     */
    char name[16];
    /** The parameters of the command.
     */
    int data[7];
} command;

/** Allocates the memory for a new command.
 */
command* create_new_command();

/** Reads a command.
 * @param[in] tmp[] The string from the input (one line).
 * @param[in] *new_command The empty command structure.
 * Returns command with data points using 'command' structure.
 */
command* parse_command(char tmp[], command* new_command);

/** Frees the memory of the command.
 */
void delete_command(command* this_command);

/** Exit the program with the exit code 42 (input error).
 */
void input_error(command* this_command);

/** Input error has occurred so it returns the command marked as incorrect.
 */
command* send_error_info(command* new_command);

#endif /* PARSE_H */