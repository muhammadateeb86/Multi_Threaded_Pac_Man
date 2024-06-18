#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include "menu.c"
#include "game.h"
#include <GL/glut.h>

typedef struct {
    int argc;
    char **argv;
} ThreadArgs;

#define NUM_GHOSTS 4
Ghost ghosts[NUM_GHOSTS];

pthread_t menu_thread, game_thread, ghostsThreads[NUM_GHOSTS];
sem_t menu_sem, game_sem, moveSem_ghost;

void* menu_thread_func(void* arg) {
    ThreadArgs *args = (ThreadArgs *) arg;
    initMenu(args->argc, args->argv);  // Pass argc and argv to initMenu
    return NULL;
}

void* game_thread_func(void* arg) {
    ThreadArgs *args = (ThreadArgs *) arg;
    sem_wait(&game_sem);
    runGame(args->argc, args->argv);  // Pass argc and argv to runGame
    sem_post(&menu_sem);  // Signal menu thread to exit
    return NULL;
}

void* ghostRoutine(void* args) {
    Ghost* ghost = (Ghost*)args;
    while (1) {
        pthread_mutex_lock(&ghost->lock);
        if (ghost->isActive) {
            // Update ghost's position or state
            if(ghost->id==0||ghost->id==1)
            moveGhost(ghost);
            else
             Sp_moveGhost(ghost);
        }
        pthread_mutex_unlock(&ghost->lock);
    }
    return NULL;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    sem_init(&menu_sem, 0, 0);
    sem_init(&game_sem, 0, 0);
    sem_init(&moveSem_ghost, 0, 1);  // Initialize semaphore

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    ThreadArgs args = {argc, argv};  // Store argc and argv in a struct

    pthread_create(&menu_thread, &attr, menu_thread_func, &args);
    pthread_create(&game_thread, &attr, game_thread_func, &args);
    for (int i = 0; i < NUM_GHOSTS; i++) {
        pthread_mutex_init(&ghosts[i].lock, NULL);
        ghosts[i].isActive = 1;  // Assuming all ghosts are active initially
        ghosts[i].id=i;
        pthread_create(&ghostsThreads[i], &attr, ghostRoutine, (void*)&ghosts[i]);
    }

    pthread_attr_destroy(&attr);  // Clean up the attribute object
    // Wait for threads to finish (not required since they are detached, but you might wait on a condition or semaphore)
    sem_destroy(&moveSem_ghost);
    pthread_exit(NULL);  // Exit the main thread while other threads may continue running
}

