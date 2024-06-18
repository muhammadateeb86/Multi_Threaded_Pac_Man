#ifndef GAME_H
#define GAME_H
#include <pthread.h>
#include <stdbool.h>
#include "texture.h"

#define MAX_PATH_LENGTH 1000

typedef struct {
    float x, y;
    float dx, dy;
    int size;
    GLuint texture;
    bool isActive;
    int id;
    int state;
    int path[MAX_PATH_LENGTH][2];  // Path storage
    int pathLength;  // Length of the path
    int pathIndex;  // Current index in the path
    pthread_mutex_t lock;  // Mutex for synchronizing access to this ghost's properties

} Ghost;

typedef struct {
    int x, y;
    int gCost;
    int hCost;
    int fCost;
    int parentX;
    int parentY;
} AStarNode;


extern Ghost ghosts[];
extern int numGhosts;

void initGhosts(int numGhosts);
void moveGhost(Ghost *ghost);
void renderGhosts();
bool canMoveGhost(Ghost *ghost, float newX, float newY);
void runGame(int argc, char **argv);
void Sp_moveGhost(Ghost *ghost);
#endif

