/** @file
    Implementation of the game engine.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engine.h"
#include "engine_private.h"


/** The structure in which the data of the game is stored.
 */
struct Data {
    /** The size of the board.
     */
    int n;
    /** The number of rounds.
     */
    int k;
    /** The number of the player whose turn it is at the moment.
     */
    int turn;
    /** The number of "half-rounds".
     * This is the sum of the rounds of both players so far plus 2 (initialisation of the game).
     */
    int q;
    /** The list of the entities.
     */
    Entities entities;
};

/** The structure in which the data of the entities is stored.
 */
struct Entity {
    /** The type of the entity.
     */
    char type;
    /** The size of the board.
     */
    int x;
    /** The column number of the entity.
     */
    int y;
    /** The row number of the entity.
     */
    int units;
    /** The amount of units the entity can use.
     * 0 - the entity has just made a movement in this round.
     * 1 - the entity can only move in this round.
     * 2 - if the entity is a peasant, it can produce a new entity in this round.
     */
    int p;
    /** The pointer to the previous entity.
     */
    Entities previous;
    /** The pointer to the next entity.
     */
    Entities next;
};


Entities find_entity(int x, int y, Database database) {
    struct Entity *actual = database->entities;
    while (actual != NULL && (actual->x != x || actual->y != y))
        actual = actual->next;
    return actual;
}

void create_new_entity(int x, int y, char type, Database database) {
    Entities new_entity;
    //creating of the entity
    new_entity = (Entities) malloc(sizeof(struct Entity));
    new_entity->previous = NULL;
    new_entity->next = NULL;
    new_entity->type = type;
    new_entity->x = x;
    new_entity->y = y;
    new_entity->units = 1;
    new_entity->p = database->turn;
    //inserting the entity
    if (database->entities != NULL) {
        struct Entity* old_first = database->entities;
        old_first->previous = new_entity;
        new_entity->next = old_first;
    }
    database->entities = new_entity;
}

int check_and_init_entity(int x, int y, char type, Database database) {
    Entities entity;
    entity = find_entity(x, y, database);
    if (entity == NULL && database->q == 2) return 42;
    if (entity == NULL) create_new_entity(x, y, type, database);
    else if (entity->x != x || entity->y != y) return 42;
    return 0;
}

void delete_entity(struct Entity *actual, Database database) {
    struct Entity *p = actual->previous;
    struct Entity *n = actual->next;
    if (p != NULL) {
        p->next = n;
        if (n != NULL) n->previous = p;
    }
    else {
        database->entities = n;
        if (n != NULL) n->previous = NULL;
    }
    free(actual);
}

void delete_all_entities(Database database) {
    struct Entity *actual = database->entities;
    while (actual != NULL) {
        actual = actual->next;
        delete_entity(database->entities, database);
    }
}

void delete_structure(Database database) {
    free(database);
}


Database start_game() {
    Database database;
    database = (Database) malloc(sizeof(struct Data));
    memset(database, 0, sizeof(struct Data));
    return database;
}

void end_game(int w, Database database) {
    if (w == 11) fprintf(stderr, "%s\n", "player 1 won");
    if (w == 12) fprintf(stderr, "%s\n", "player 2 won");
    if (w == 13) fprintf(stderr, "%s\n", "draw");
    delete_all_entities(database);
    delete_structure(database);
}

int init(int n, int k, int p, int x1, int y1, int x2, int y2, Database database) {

    if (k <= 0) return 42;
    if (x1 > n || x2 > n || y1 > n || y2 > n || p > 2) return 42;
    if (x1 <= 0 || x2 <= 0 || y1 <= 0 || y2 <= 0) return 42;
    if (x1 > n - 3 || x2 > n - 3) return 42;
    if (abs(x1 - x2) < 8 && abs(y1 - y2) < 8) return 42;

    database->q++;
    if (database->turn == p) return 42;

    if (database->n == 0) database->n = n;
    else if (database->n != n) return 42;

    if (database->k == 0) database->k = k;
    else if (database->k != k) return 42;

    database->turn = 1;
    if (check_and_init_entity(x1, y1, 'K', database) == 42) return 42;
    if (check_and_init_entity(x1 + 1, y1, 'C', database) == 42) return 42;
    if (check_and_init_entity(x1 + 2, y1, 'R', database) == 42) return 42;
    if (check_and_init_entity(x1 + 3, y1, 'R', database) == 42) return 42;

    database->turn = 2;
    if (check_and_init_entity(x2, y2, 'k', database) == 42) return 42;
    if (check_and_init_entity(x2 + 1, y2, 'c', database) == 42) return 42;
    if (check_and_init_entity(x2 + 2, y2, 'r', database) == 42) return 42;
    if (check_and_init_entity(x2 + 3, y2, 'r', database) == 42) return 42;

    if (database->q == 2) database->turn = 1;
    else database->turn = p;

    return 1;
}

int move(int x1, int y1, int x2, int y2,  Database database) {
    if (abs(x1 - x2) > 1 || abs(y1 - y2) > 1) return 42;
    if (x2 > database->n || y2 > database->n) return 42;
    struct Entity *actual = find_entity(x1, y1, database);
    if (actual == NULL) return 42;
    if (actual->p != database->turn) return 42;
    if (actual->units <= 0) return 42;
    struct Entity *entity = find_entity(x2, y2, database);
    //change of the position
    if (entity == NULL) {
        actual->x = x2;
        actual->y = y2;
        actual->units--;
    }
    else {
        //battle
        if (actual->p != entity->p) {
            if (actual->type == 'C' || actual->type == 'c') {
                if (entity->type == 'C' || entity->type == 'c')
                    delete_entity(entity, database);
                delete_entity(actual, database);
            }
            else if (actual->type == 'R' || actual->type == 'r') {
                if (entity->type == 'R' || entity->type == 'r') {
                    delete_entity(actual, database);
                    delete_entity(entity, database);
                }
                else {
                    char t = entity->type;
                    delete_entity(entity, database);
                    //moving
                    actual->x = x2;
                    actual->y = y2;
                    actual->units--;
                    if (t == 'K') return 12;
                    if (t == 'k') return 11;
                }
            }
            else if (actual->type == 'K' || actual->type == 'k') {
                if (entity->type == 'R' || entity->type == 'r') {
                    char t = actual->type;
                    delete_entity(actual, database);
                    //player 1 won
                    if (t == 'K') return 12;
                    if (t == 'k') return 11;
                }
                else if ((entity->type == 'C' || entity->type == 'c')) {
                    delete_entity(entity, database);
                    //moving
                    actual->x = x2;
                    actual->y = y2;
                    actual->units--;
                }
                else {
                    //draw
                    delete_entity(entity, database);
                    delete_entity(actual, database);
                    //print_topleft(database);
                    return 13;
                }
            }
            else return 42;
        }
        else return 42;
    }
    return 1;
}

int produce_knight(int x1, int y1, int x2, int y2, Database database) {
    if (abs(x1 - x2) > 1 || abs(y1 - y2) > 1) return 42;
    if (x2 > database->n || y2 > database->n) return 42;
    struct Entity *actual = find_entity(x1, y1, database);
    if (actual == NULL) return 42;
    if (actual->p != database->turn) return 42;
    if (actual->units < 2) return 42;
    struct Entity *entity = find_entity(x2, y2, database);
    if (entity != NULL) return 42;
    else {
        if ((database->turn == 1 && database->q < 6)
            || (database->turn == 2 && database->q < 7)) return 42;
        if ((actual->type == 'C' || actual->type == 'c')
            && actual->units == 2) {
            actual->units = 0;
            if (actual->type == 'C') create_new_entity(x2, y2, 'R', database);
            else create_new_entity(x2, y2, 'r', database);
        }
        else return 42;
    }
    return 1;
}

int produce_peasant(int x1, int y1, int x2, int y2, Database database) {
    if (abs(x1 - x2) > 1 || abs(y1 - y2) > 1) return 42;
    if (x2 > database->n || y2 > database->n) return 42;
    struct Entity *actual = find_entity(x1, y1, database);
    if (actual == NULL) return 42;
    if (actual->p != database->turn) return 42;
    if (actual->units < 2) return 42;
    struct Entity *entity = find_entity(x2, y2, database);
    if (entity != NULL) return 42;
    else {
        if ((database->turn == 1 && database->q < 6)
            || (database->turn == 2 && database->q < 7)) return 42;
        if ((actual->type == 'C' || actual->type == 'c')
            && actual->units == 2) {
            actual->units = 0;
            if (actual->type == 'C') create_new_entity(x2, y2, 'C', database);
            else create_new_entity(x2, y2, 'c', database);
        }
        else return 42;
    }
    return 1;
}

int end_turn(Database database) {
    struct Entity *actual = database->entities;
    if (database->turn == 2) {
        while (actual != NULL) {
            if (actual->units < 2) actual->units++;
            actual = actual->next;
        }
        database->k--;
        database->turn = 1;
    }
    else database->turn = 2;
    database->q++;
    if (database->k == 0) return 13;
    return 0;
}

void print_topleft(Database database) {
    int m;
    if (database->n < 10) m = database->n;
    else m = 10;
    char P[m][m];
    int i, j;
    for (i = 0; i < m; i++)
        for (j = 0; j < m; j++)
            P[i][j] = '.';
    struct Entity *actual = database->entities;
    while (actual != NULL) {
        if (actual->x <= 10 && actual->y <= 10)
            P[actual->x - 1][actual->y - 1] = actual->type;
        actual = actual->next;
    }
    for (i = 0; i < m; i++) {
        for (j = 0; j < m; j++)
            printf("%c", P[j][i]);
        printf("\n");
    }
    printf("\n");
}