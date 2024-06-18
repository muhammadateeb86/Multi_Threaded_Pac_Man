#include <GL/glut.h>
#include <SOIL/SOIL.h>
#include <semaphore.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "music.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "texture.h"
#include "menu.h"
#include "game.h"  // Assuming this provides necessary declarations
extern sem_t game_sem;  // This semaphore must be defined in your shared context
int id;

// Globals for texture and window management
GLuint menuTexture;
int windowWidth = 800;
int windowHeight = 600;

void initOpenGL();
void display();
void mouse(int button, int state, int x, int y);
void startGame();
void exitGame();
void initMenu();

void initOpenGL() {
    glClearColor(0.0, 0.0, 0.0, 1.0); // Set clear color to black
    glEnable(GL_TEXTURE_2D);          // Enable Texture Mapping
    glViewport(0, 0, windowWidth, windowHeight); // Set the viewport
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, windowWidth, windowHeight, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, menuTexture);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0); glVertex2f(0.0, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex2f(windowWidth, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex2f(windowWidth, windowHeight);
        glTexCoord2f(0.0, 0.0); glVertex2f(0.0, windowHeight);
    glEnd();
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (x > 300 && x < 510 && y > 370 && y < 418) {
            startGame();
        } else if (x > 325 && x < 624 && y > 435 && y < 500) {
            exitGame();
        }
    }
}

void startGame() {
    printf("Starting game...\n");
    sem_post(&game_sem);  // Signal to start the game logic
    // Close the current window
    glutDestroyWindow(id);
    pthread_exit(NULL);
}

void exitGame() {
    printf("Exiting game...\n");
    exit(0);  // Forcefully exit the program
}

void initMenu(int argc, char** argv) {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    id = glutCreateWindow("Menu with SOIL");
    initOpenGL();
    menuTexture = loadTexture("images/menu.png");
    playmusic("music/gamestart.mp3");
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMainLoop();
}

