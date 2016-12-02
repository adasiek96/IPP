/** @file
    Interface of the game engine (private functions).

 */

#ifndef ENGINE_PRIVATE_H
#define ENGINE_PRIVATE_H

/** The pointer to the structure in which data of the entities are stored.
 */
typedef struct Entity* Entities;

/** The structure in which data of the entities are stored.
 */
struct Entity;

/** Finds entity which stands on the field (x,y).
 * @param[in] x Column number of the entity.
 * @param[in] y Row number of the entity.
 * @param[in] database Includes data of the game.
 * @return The pointer to the entity which we were searching for.
 */
Entities find_entity(int x, int y, Database database);

/** Creates a new entity on the field (x,y).
 * @param[in] x Column number of the entity.
 * @param[in] y Row number of the entity.
 * @param[in] type Type of the entity.
 * @param[in] database Includes data of the game.
 */
void create_new_entity(int x, int y, char type, Database database);

/** Check the correctness while creating a new entity on the field (x,y) during initialisation of the board.
 * @param[in] x Column number of the entity.
 * @param[in] y Row number of the entity.
 * @param[in] type Type of the entity.
 * @param[in] database Includes data of the game.
 */
int check_and_init_entity(int x, int y, char type, Database database);

/** Deletes the current entity from the database.
 * @param[in] *actual Pointer to the entity which we want to delete.
 * @param[in] database Includes data of the game.
 */
void delete_entity(struct Entity *actual, Database database);

/** Deletes all entities from the database.
 * @param[in] database Includes data of the game.
 */
void delete_all_entities(Database database);

/** Deletes the whole structure (database).
 * @param[in] database Includes data of the game.
 */
void delete_structure(Database database);

#endif /* ENGINE_PRIVATE_H */