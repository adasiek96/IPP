/** @file
    Interface of the game engine.

 */

#ifndef ENGINE_H
#define ENGINE_H

/** The pointer to the structure in which the data of the game is stored.
 */
typedef struct Data* Database;

/** The structure in which data of the game is stored.
 */
struct Data;

/** Initializes a game. Needed before the first INIT.
 * @return Database which includes the data of the game.
 */
Database start_game();

/** Frees memory. Needed after finishing the game.
 * @param[in] w Includes information about winning.
 * @param[in] database Includes the data of the game.
 */
void end_game(int w, Database database);

/** Initializes a game with size of the board, the number of rounds and positions of kings.
 * @param[in] n Size of the board
 * @param[in] k Number of the rounds.
 * @param[in] p Number of the player.
 * @param[in] x1 Column number of the first king.
 * @param[in] y1 Row number of the first king.
 * @param[in] x2 Column number of the second king.
 * @param[in] y2 Row number of the second king.
 * @param[in] database Includes the data of the game.
 * @return
 * 1 - standard return with printing top-left corner of the board.\n
 * 42 - input error.
 */
int init(int n, int k, int p, int x1, int y1, int x2, int y2, Database database);

/** Makes a movement.
 * @param[in] x1 Column number before a movement.
 * @param[in] y1 Row number before a movement.
 * @param[in] x2 Column number after a movement.
 * @param[in] y2 Row number after a movement.
 * @param[in] database Includes data of the game.
 * @return
 * 1 - standard return with printing top-left corner of the board.\n
 * 11 - player 1 won.\n
 * 12 - player 2 won.\n
 * 13 - draw.\n
 * 42 - input error. */
int move(int x1, int y1, int x2, int y2, Database database);

/** Produces a knight.
 * @param[in] x1 Column number of a peasant who produces a knight.
 * @param[in] y1 Row number of a peasant who produces a knight.
 * @param[in] x2 Column number where a knight appears after its production.
 * @param[in] y2 Row number where a knight appears after its production.
 * @param[in] database Includes data of the game.
 * @return
 * 1 - standard return with printing top-left corner of the board.\n
 * 42 - input error.
 */
int produce_knight(int x1, int y1, int x2, int y2, Database database);

/** Produces a peasant.
 * @param[in] x1 Column number of a peasant who produces another peasant.
 * @param[in] y1 Row number of a peasant who produces another peasant.
 * @param[in] x2 Column number where a peasant appears after production.
 * @param[in] y2 Row number where a peasant appears after production.
 * @param[in] database Includes data of the game.
 * @return
 * 1 - standard return with printing top-left corner of the board.\n
 * 42 - input error.
 */
int produce_peasant(int x1, int y1, int x2, int y2, Database database);

/** Finishes the turn of the player.
 * @param[in] database Includes data of the game.
 * 0 - standard return without printing top-left corner of the board.\n
 * 13 - draw.
 */
int end_turn(Database database);

/** Prints (into stdout) top-left corner of the board of the size m x m where m = min(n,10).
 * @param[in] database Includes data of the game.
 */
void print_topleft(Database database);

#endif /* ENGINE_H */