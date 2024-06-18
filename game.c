#include <GL/glut.h>
#include <SOIL/SOIL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "music.h"
#include <math.h>
#include "game.h"
#include "texture.h"
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <semaphore.h>
sem_t key_sem;
GLuint topimagetexture;

typedef struct {
    int x;
    int y;
    int cost;
    int priority;  // Lower values have higher priority
} Node;

typedef struct {
    float x, y;
    float size;
    int lives;
    int score;
    float angle_open;
    float dx, dy;
    GLuint texture;
    int rotation;
} Pacman;

Pacman player;

bool blinkState = true;  // Start with power pellets visible
bool eatGhost=false;
bool powerUpActive = false;
float powerUpTimer = 0;
float powerUpDuration = 500;  // Duration of power-up effect in milliseconds
bool mouthOpening = true;
#define WALL 1
#define FOOD 2
#define POWER 3
#define EMPTY 0
#define DEG2RAD 0.017453292519943295 // PI / 180

#define ROWS 26
#define COLS 39


int maze[ROWS][COLS]={
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 1, 1, 2, 1, 1, 1, 3, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1},
    {1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 1, 2, 2, 2, 1, 1, 2, 2, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1},
    {1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 1, 2, 1, 2, 1, 2, 2, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 2, 1, 2, 1},
    {1, 2, 1, 2, 1, 2, 2, 2, 1, 2, 2, 1, 2, 1, 1, 1, 2, 1, 2, 2, 2, 1, 1, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 1, 2, 1},
    {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 2, 2, 2, 2, 1, 2, 2, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 2, 1, 2, 2, 1, 2, 1},
    {1, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 2, 1, 2, 1},
    {1, 2, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 2, 1},
    {1, 2, 1, 2, 2, 2, 1, 2, 1, 3, 2, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 1, 2, 1, 1},
    {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 1, 2, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1},
    {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 2, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 2, 2, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1},
    {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 2, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1},
    {1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1},
    {1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 2, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 2, 1, 2, 2, 1, 2, 1, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 2, 2, 1},
    {1, 1, 1, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 1, 1},
    {1, 2, 3, 1, 2, 2, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1},
    {1, 2, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 1, 2, 2, 1},
    {1, 2, 2, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

//ghost

int numGhosts = 4;
float startX[4] = {440, 460, 480, 500};  // Different start X positions for each ghost
float startY[4] = {400, 400, 400, 400};  // Different start Y positions for each ghost
void initGhosts(int numGhosts) {
    for (int i = 0; i <= numGhosts; i++) {
        ghosts[i].x = startX[i];
        ghosts[i].y = startY[i];
        ghosts[i].dx = ((rand() % 2) ? 0.1 : -0.1);  // Random initial direction
        ghosts[i].dy = ((rand() % 2) ? 0.1 : -0.1);
        ghosts[i].size = 12.5; // Example size
        ghosts[i].isActive = true;
    }
}

void initGraphics(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void toggleBlink(int value) {
    blinkState = !blinkState;  // Toggle the state
    glutPostRedisplay();       // Redraw the screen to update the blink
    glutTimerFunc(500, toggleBlink, 0);  // Set the timer for another toggle
}
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    initGraphics(width, height);
}

void initGame() {
    player.size = 24;
    player.x = 187;
    player.y = 350;
    player.dx = 0;
    player.dy = 0;
    player.texture = 0;  // Ensure no texture is used if you draw Pacman geometrically
    player.rotation = 0;
    player.lives = 3;
    player.score = 0;
    player.angle_open = 0.40f * M_PI;  // Set an initial mouth angle
    for (int i = 0; i < 4; i++)
    {initGhosts(i);}
    topimagetexture = loadTexture("images/Title.png");
        playmusic("music/game.wav");
    glutPostRedisplay(); // Force the initial display
}

void renderToppng() {
    glColor3f(1.0f, 1.0f, 1.0f); // Reset color to white
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, topimagetexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0); glVertex2f(0, 0);
    glTexCoord2f(1.0, 1.0); glVertex2f(950, 0);
    glTexCoord2f(1.0, 0.0); glVertex2f(950, 100);
    glTexCoord2f(0.0, 0.0); glVertex2f(0, 100);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
void renderMaze() {
    int tileSize = 25;  // Each tile is 25x25 pixels
     float lineThickness = 2.0f;  // Set the thickness of the lines

    glLineWidth(lineThickness);  // Set the line width for the walls
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            float x = col * tileSize;
            float y = row * tileSize;

            if (maze[row][col] == WALL) {
                glColor3f(0.2f, 0.5f, 1.0f); // Wall color

                // Only draw the left line if this is the first column or there is no wall to the left
                if (col == 0 || maze[row][col - 1] != WALL) {
                    glBegin(GL_LINES);
                    glVertex2f(x, y);
                    glVertex2f(x, y + tileSize);
                    glEnd();
                }

                // Only draw the top line if this is the first row or there is no wall above
                if (row == 0 || maze[row - 1][col] != WALL) {
                    glBegin(GL_LINES);
                    glVertex2f(x, y);
                    glVertex2f(x + tileSize, y);
                    glEnd();
                }

                // Only draw the right line if this is the last column or there is no wall to the right
                if (col == COLS - 1 || maze[row][col + 1] != WALL) {
                    glBegin(GL_LINES);
                    glVertex2f(x + tileSize, y);
                    glVertex2f(x + tileSize, y + tileSize);
                    glEnd();
                }

                // Only draw the bottom line if this is the last row or there is no wall below
                if (row == ROWS - 1 || maze[row + 1][col] != WALL) {
                    glBegin(GL_LINES);
                    glVertex2f(x, y + tileSize);
                    glVertex2f(x + tileSize, y + tileSize);
                    glEnd();
                }
            } else if (maze[row][col] == FOOD) {
                GLfloat maxSize;
                glGetFloatv(GL_POINT_SIZE_MAX, &maxSize); // Get the maximum size of point that can be rendered
                float foodPointSize = fmin(maxSize, 2.0f); // Use 10 or the maximum size if it's smaller

                glColor3f(1.0f, 1.0f, 0.0f); // Food color
                glPointSize(foodPointSize);
                glBegin(GL_POINTS);
                glVertex2f(x + tileSize/2, y + tileSize/2);
                glEnd();
                }
               else if (maze[row][col] == POWER && blinkState) { // Power Pellet
                float powerPelletSize = 12.0f; // Larger size for power pellets
                glColor3f(1.0f, 0.0f, 0.0f); // Distinct color for power pellets (Cyan)
                glBegin(GL_POLYGON);  // Use GL_POLYGON for a solid circle appearance
                for(int i = 0; i < 360; i++) {
                    float degInRad = i * DEG2RAD;
                    glVertex2f(x + tileSize / 2 + cos(degInRad) * (powerPelletSize / 2),
                               y + tileSize / 2 + sin(degInRad) * (powerPelletSize / 2));
                }
                glEnd();
            }
        }
    }
}


void renderScoreandlives()
{
       // Render Score
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 950, 650, 0);  // Set coordinate system for text rendering
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();    
	glColor3f(1.0f, 1.0f, 1.0f);  // Set text color to white
	glRasterPos2f(50, 50); // Position for the score text
	char scoreText[20];
	sprintf(scoreText, "Score: %d", player.score);
	for (int i = 0; scoreText[i] != '\0'; i++) {  // Iterate through each character in the string
	    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreText[i]);  // Display each character
	}
	
	// Render Lives
	glRasterPos2f(850, 50);  // Position for the lives text
	char livesText[20];
	sprintf(livesText, "Lives: %d", player.lives);
	for (int i = 0; livesText[i] != '\0'; i++) {  // Iterate through each character in the string
	    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, livesText[i]);  // Display each character
	}
}

void renderPacman() {
    float radius = player.size / 2;
    int num_segments = 32; // Number of segments for a smooth circle
    float angle;

    glColor3f(1.0f, 1.0f, 0.0f); // Yellow color for Pacman

    glPushMatrix();
    glTranslatef(player.x, player.y, 0);
    glRotatef(player.rotation, 0, 0, 1); // Rotate Pacman to face the correct direction

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0); // Center of Pacman

    float mouth_open_angle = player.angle_open / 2; // Half on each side of the direction
    float start_angle = 2 * M_PI - mouth_open_angle; // Start drawing from here
    float end_angle = mouth_open_angle; // Stop drawing here

    for (int i = 0; i <= num_segments; i++) {
        angle = 2 * M_PI * i / num_segments;
        if (!(angle < end_angle || angle > start_angle)) {
            glVertex2f(radius * cos(angle), radius * sin(angle));
        }
    }

    glEnd();
    glPopMatrix();
}

void renderGhosts() {
    float colors[4][3] = {
        {1.0f, 0.0f, 0.0f},  // Red
        {1.0f, 1.0f, 0.0f},  // Yellow
        {0.0f, 1.0f, 1.0f},  // Cyan
        {1.0f, 0.75f, 0.80f} // Pink
    };
    float blueColor[3] = {0.0f, 0.0f, 1.0f}; // Blue color for ghosts
    int num_segments = 20;  // Smoothness of the semi-circle
    float ghostHeight = ghosts[0].size * 2.5;  // Total height of the ghost
    float ghostWidth = ghosts[0].size * 1;   // Total width of the ghost

    for (int i = 0; i < numGhosts; i++) {
        if (ghosts[i].isActive) {
            if (ghosts[i].isActive) {
            if (powerUpActive) {
                glColor3fv(blueColor);
            } else {
                eatGhost=false;
                powerUpTimer = 0;
                glColor3fv(colors[i]);
            }
            }

            // Upper part of the ghost (semi-circle)
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(ghosts[i].x, ghosts[i].y);  // Center of semi-circle should be at ghost's position
		float radius = ghostHeight / 4;  // Using ghostHeight to define the radius for both dimensions
		for (int j = 0; j <= num_segments; j++) {
		    float theta = M_PI * j / num_segments;  // Ranges from 0 to Pi to create a half-circle
		    glVertex2f(ghosts[i].x + radius * cos(theta), ghosts[i].y - radius * sin(theta));
		}
		glEnd();


            // Lower part of the ghost (sharp triangular cuts)
		glBegin(GL_QUADS);
		// Draw the main rectangle for the ghost's body
		float rectHeight = ghostHeight / 3;  // Main body height is most of the ghost's height
		float rectWidth = ghostWidth + 4;    // Width of the ghost
		float baseX = ghosts[i].x - rectWidth / 2;
		float baseY = ghosts[i].y;

		// Draw the rectangle
		glVertex2f(baseX, baseY);
		glVertex2f(baseX + rectWidth, baseY);
		glVertex2f(baseX + rectWidth, baseY + rectHeight);
		glVertex2f(baseX, baseY + rectHeight);
		glEnd();

		// Adding triangular cuts at the bottom
		glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 0.0f, 0.0f);
		float cutWidth = rectWidth / 2;  // Base width for each cut
		float cutHeight = ghostHeight * 0.15;  // Height of the triangular cuts

		// First triangular cut
		float midPoint1X = baseX + cutWidth / 2;  // Middle point for the first triangle peak
		glVertex2f(baseX, baseY + rectHeight);                // Left base of the first triangle
		glVertex2f(baseX + cutWidth, baseY + rectHeight);     // Right base of the first triangle
		glVertex2f(midPoint1X, baseY + rectHeight - cutHeight);  // Peak of the first triangle

		// Second triangular cut
		float midPoint2X = baseX + 3 * cutWidth / 2;  // Middle point for the second triangle peak
		glVertex2f(baseX + cutWidth, baseY + rectHeight);               // Left base of the second triangle
		glVertex2f(baseX + 2 * cutWidth, baseY + rectHeight);           // Right base of the second triangle
		glVertex2f(midPoint2X, baseY + rectHeight - cutHeight);         // Peak of the second triangle
		glEnd();





            // Eyes
            float eyeRadius = radius / 4;  
            float eyeXOffset = radius / 3;  // Reduced offset to bring eyes closer
            float eyeYOffset = -radius / 2; 
            float pupilRadius = eyeRadius / 2;  // Pupil is smaller than the eye

            glColor3f(1.0f, 1.0f, 1.0f);  // White for the eyes
            // Left eye
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(ghosts[i].x - eyeXOffset, ghosts[i].y + eyeYOffset);
            for (int k = 0; k <= num_segments; k++) {
                float angle = 2 * M_PI * k / num_segments;
                glVertex2f(ghosts[i].x - eyeXOffset + eyeRadius * cos(angle), ghosts[i].y + eyeYOffset + eyeRadius * sin(angle));
            }
            glEnd();

            // Right eye
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(ghosts[i].x + eyeXOffset, ghosts[i].y + eyeYOffset);
            for (int k = 0; k <= num_segments; k++) {
                float angle = 2 * M_PI * k / num_segments;
                glVertex2f(ghosts[i].x + eyeXOffset + eyeRadius * cos(angle), ghosts[i].y + eyeYOffset + eyeRadius * sin(angle));
            }
            glEnd();

            // Pupils (black)
            glColor3f(0.0f, 0.0f, 0.0f);  // Black for the pupils
            // Left pupil
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(ghosts[i].x - eyeXOffset, ghosts[i].y + eyeYOffset);
            for (int l = 0; l <= num_segments; l++) {
                float angle = 2 * M_PI * l / num_segments;
                glVertex2f(ghosts[i].x - eyeXOffset + pupilRadius * cos(angle), ghosts[i].y + eyeYOffset + pupilRadius * sin(angle));
            }
            glEnd();

            // Right pupil
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(ghosts[i].x + eyeXOffset, ghosts[i].y + eyeYOffset);
            for (int l = 0; l <= num_segments; l++) {
                float angle = 2 * M_PI * l / num_segments;
                glVertex2f(ghosts[i].x + eyeXOffset + pupilRadius * cos(angle), ghosts[i].y + eyeYOffset + pupilRadius * sin(angle));
            }
            glEnd();
        }
    }
}



void renderGame() {
    if (player.lives > 0) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderToppng();
        renderMaze();
        renderScoreandlives();
        renderPacman();
        renderGhosts();
        glutSwapBuffers();
    }
}

void canEat(float newX, float newY) {
    int PACMAN_RADIUS = player.size / 10;
    int TILE_SIZE = 25;
    int newCellX = (int)((newX + PACMAN_RADIUS) / TILE_SIZE);
    int newCellY = (int)((newY + PACMAN_RADIUS) / TILE_SIZE);

    // Calculate boundaries of Pacman based on the proposed new center position
    float leftEdge = newX - PACMAN_RADIUS;
    float rightEdge = newX + PACMAN_RADIUS;
    float topEdge = newY - PACMAN_RADIUS;
    float bottomEdge = newY + PACMAN_RADIUS;

        // Check boundaries against the maze grid cells
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                if (maze[row][col] == FOOD) {
                    // Calculate the boundaries of the food cell
                    float foodLeft = col * TILE_SIZE;
                    float foodRight = foodLeft + TILE_SIZE;
                    float foodTop = row * TILE_SIZE;
                    float foodBottom = foodTop + TILE_SIZE;

                    // Detect overlap
                    bool horizontalOverlap = leftEdge < foodRight && rightEdge > foodLeft;
                    bool verticalOverlap = topEdge < foodBottom && bottomEdge > foodTop;

                    if (horizontalOverlap && verticalOverlap) {
                        maze[row][col] = 0;  // Collision detected
                        player.score += 10;  // Increase score
                    }
                }
                else if(maze[row][col] == POWER && !eatGhost) {
                     // Calculate the boundaries of the food cell
                    float powerLeft = col * TILE_SIZE;
                    float powerRight = powerLeft + TILE_SIZE;
                    float powerTop = row * TILE_SIZE;
                    float powerBottom = powerTop + TILE_SIZE;

                    // Detect overlap
                    bool horizontalOverlap = leftEdge < powerRight && rightEdge > powerLeft;
                    bool verticalOverlap = topEdge < powerBottom && bottomEdge > powerTop;

                    if (horizontalOverlap && verticalOverlap) {
                        eatGhost=true;
                        powerUpActive = true;
                        powerUpTimer = 0;
                        maze[row][col] = 0;  // Collision detected
                    }
                }
            }
    }
}


bool canMoveGhost(Ghost *ghost, float newX, float newY) {
    int ghostRadius = ghost->size;
    int TILE_SIZE = 25;

    // Calculate boundaries of Ghost based on the proposed new center position
    float leftEdge = newX - ghostRadius;
    float rightEdge = newX + ghostRadius;
    float topEdge = newY - ghostRadius;
    float bottomEdge = newY + ghostRadius;

    // Check boundaries against the maze grid cells
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (maze[row][col] == WALL) {
                // Calculate the boundaries of the wall cell
                float wallLeft = col * TILE_SIZE;
                float wallRight = wallLeft + TILE_SIZE;
                float wallTop = row * TILE_SIZE;
                float wallBottom = wallTop + TILE_SIZE;

                // Detect overlap
                bool horizontalOverlap = leftEdge < wallRight && rightEdge > wallLeft;
                bool verticalOverlap = topEdge < wallBottom && bottomEdge > wallTop;

                if (horizontalOverlap && verticalOverlap) {
                    return false;  // Collision detected
                }
            }
        }
    }

    return true;  // No collision
}
void Sp_moveGhost(Ghost *ghost){
if (!ghost->isActive) return;

    float proposedX = ghost->x + ghost->dx;
    float proposedY = ghost->y + ghost->dy;

    if (canMoveGhost(ghost, proposedX, proposedY)) {
        ghost->x = proposedX;
        ghost->y = proposedY;
    } else {
        // Randomly choose a new direction when hitting a wall
        int direction = rand() % 4;
        switch (direction) {
            case 0:
                ghost->dx = 2.5; ghost->dy = 0;  // Move right
                break;
            case 1:
                ghost->dx = -2.5; ghost->dy = 0;  // Move left
                break;
            case 2:
                ghost->dx = 0; ghost->dy = 2.5;  // Move down
                break;
            case 3:
                ghost->dx = 0; ghost->dy = -2.5;  // Move up
                break;
        }
    }
            usleep(10000); 
            sem_post(&key_sem);
}
void moveGhost(Ghost *ghost) {
    if (!ghost->isActive) return;
    sem_wait(&key_sem);
    float proposedX = ghost->x + ghost->dx;
    float proposedY = ghost->y + ghost->dy;

    if (canMoveGhost(ghost, proposedX, proposedY)) {
        ghost->x = proposedX;
        ghost->y = proposedY;
    } else {
        // Randomly choose a new direction when hitting a wall
        int direction = rand() % 4;
        switch (direction) {
            case 0:
                ghost->dx = 0.5; ghost->dy = 0;  // Move right
                break;
            case 1:
                ghost->dx = -0.5; ghost->dy = 0;  // Move left
                break;
            case 2:
                ghost->dx = 0; ghost->dy = 0.5;  // Move down
                break;
            case 3:
                ghost->dx = 0; ghost->dy = -0.5;  // Move up
                break;
        }
    }
            usleep(10000); 
        
}

// Heuristic function for estimating the cost from the current node to the target
int heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

bool isValid(int x, int y) {
    return x >= 0 && x < COLS && y >= 0 && y < ROWS && maze[y][x] != WALL;
}

int findPath(Ghost *ghost, int targetX, int targetY) {
    int TILE_SIZE=25;
    AStarNode nodes[ROWS][COLS];
    bool openSet[ROWS][COLS] = {false};
    bool closedSet[ROWS][COLS] = {false};
    int i, j, k, l;

    int startX = ghost->x / TILE_SIZE;
    int startY = ghost->y / TILE_SIZE;

    // Initialize all nodes
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            nodes[i][j].x = j;
            nodes[i][j].y = i;
            nodes[i][j].gCost = INT_MAX;
            nodes[i][j].hCost = INT_MAX;
            nodes[i][j].fCost = INT_MAX;
            nodes[i][j].parentX = -1;
            nodes[i][j].parentY = -1;
        }
    }

    i = startY;
    j = startX;
    nodes[i][j].gCost = 0;
    nodes[i][j].hCost = heuristic(j, i, targetX, targetY);
    nodes[i][j].fCost = nodes[i][j].hCost;

    openSet[i][j] = true;

    while (1) {
        AStarNode *current = NULL;
        int currentX = 0, currentY = 0;

        for (k = 0; k < ROWS; k++) {
            for (l = 0; l < COLS; l++) {
                if (openSet[k][l] && (!current || nodes[k][l].fCost < current->fCost)) {
                    current = &nodes[k][l];
                    currentY = k;
                    currentX = l;
                }
            }
        }

        if (!current) break;

        if (currentX == targetX && currentY == targetY) {
            // Reconstruct path
            ghost->pathLength = 0;
            while (current->parentX != -1 && ghost->pathLength < MAX_PATH_LENGTH) {
                ghost->path[ghost->pathLength][0] = current->x;
                ghost->path[ghost->pathLength][1] = current->y;
                ghost->pathLength++;
                int tempX = current->parentX;
                int tempY = current->parentY;
                current = &nodes[tempY][tempX];
            }
            return ghost->pathLength > 0;
        }

        openSet[currentY][currentX] = false;
        closedSet[currentY][currentX] = true;

        int neighbors[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
        for (k = 0; k < 4; k++) {
            int neighborX = currentX + neighbors[k][0];
            int neighborY = currentY + neighbors[k][1];

            if (isValid(neighborX, neighborY) && !closedSet[neighborY][neighborX]) {
                int gCost = current->gCost + 1;
                int hCost = heuristic(neighborX, neighborY, targetX, targetY);
                int fCost = gCost + hCost;

                if (!openSet[neighborY][neighborX] || fCost < nodes[neighborY][neighborX].fCost) {
                    nodes[neighborY][neighborX].gCost = gCost;
                    nodes[neighborY][neighborX].hCost = hCost;
                    nodes[neighborY][neighborX].fCost = fCost;
                    nodes[neighborY][neighborX].parentX = currentX;
                    nodes[neighborY][neighborX].parentY = currentY;
                    openSet[neighborY][neighborX] = true;
                }
            }
        }
    }

    return false;  // Path not found
}
/* AI ghost movement
// Usage example
void moveGhost(Ghost *ghost) {
        int TILE_SIZE=25;
    int targetX = (int)(player.x / TILE_SIZE);
    int targetY = (int)(player.y / TILE_SIZE);

    if (ghost->isActive && (ghost->pathIndex >= ghost->pathLength || !ghost->pathLength)) {
        if (!findPath(ghost, targetX, targetY)) {
            printf("No path found!\n");
        }
        ghost->pathIndex = 0;
    }

    if (ghost->pathLength > 0 && ghost->pathIndex < ghost->pathLength) {
        ghost->x = ghost->path[ghost->pathIndex][0] * TILE_SIZE;
        ghost->y = ghost->path[ghost->pathIndex][1] * TILE_SIZE;
        ghost->pathIndex++;
    }
}*/

int canMove(float newX, float newY) {
    int PACMAN_RADIUS = player.size/2;
    int TILE_SIZE=25;
    int newCellX = (int)((newX + PACMAN_RADIUS) / TILE_SIZE);
    int newCellY = (int)((newY + PACMAN_RADIUS) / TILE_SIZE);

    // Calculate boundaries of Pacman based on the proposed new center position
    float leftEdge = newX - PACMAN_RADIUS;
    float rightEdge = newX + PACMAN_RADIUS;
    float topEdge = newY - PACMAN_RADIUS;
    float bottomEdge = newY + PACMAN_RADIUS;

    // Check boundaries against the maze grid cells
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (maze[row][col] == WALL) {
                // Calculate the boundaries of the wall cell
                float wallLeft = col * TILE_SIZE;
                float wallRight = wallLeft + TILE_SIZE;
                float wallTop = row * TILE_SIZE;
                float wallBottom = wallTop + TILE_SIZE;

                // Detect overlap
                bool horizontalOverlap = leftEdge < wallRight && rightEdge > wallLeft;
                bool verticalOverlap = topEdge < wallBottom && bottomEdge > wallTop;

                if (horizontalOverlap && verticalOverlap) {
                    return 0;  // Collision detected
                }
            }
        }
    }

    return 1;  // No collision
}



void handleKeypress(unsigned char key, int x, int y) {
    int movementSpeed = 1;  // Speed of movement

    switch (key) {
        case 'w':  // Move up
            player.dy = -movementSpeed;
            player.dx = 0;
            player.rotation = 270;  // Facing upwards
            break;
        case 's':  // Move down
            player.dy = movementSpeed;
            player.dx = 0;
            player.rotation = 90;  // Facing downwards
            break;
        case 'a':  // Move left
            player.dx = -movementSpeed;
            player.dy = 0;
            player.rotation = 180;  // Facing left
            break;
        case 'd':  // Move right
            player.dx = movementSpeed;
            player.dy = 0;
            player.rotation = 0;    // Facing right
            break;
    }

    // Reset mouth opening
    player.angle_open = 0.2f * M_PI;  // Set a standard initial open angle

    glutPostRedisplay();
}

//Check collision of ghost with the pacman
void checkCollisions() {
    for (int i = 0; i < numGhosts; i++) {
        float distance = sqrt(pow(ghosts[i].x - player.x, 2) + pow(ghosts[i].y - player.y, 2));
        if (distance < (player.size / 2 + ghosts[i].size / 2)) {
            if(eatGhost){
	    ghosts[i].x = startX[i];
            ghosts[i].y = startY[i];
            player.score += 200;  // Increase score
            powerUpActive = false;
            powerUpTimer = 0;
            }else{
            player.lives -= 1;
            player.x = 187;  // Reset Pacman's position
            player.y = 350;
            if (player.lives <= 0) {
                player.dx = 0;
                player.dy = 0;
                return; // Stop further processing once lives are zero
            }
          }
        }
    }

}

//Last screen after 0 live
void displayGameOver() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen
    glColor3f(1.0f, 0.0f, 0.0f);  // Red color for "Game Over" text
    glRasterPos2f(375, 325);  // Position the text at the center of the window
    const char message[] = "Game Over";
    for (const char* c = message; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
    glutSwapBuffers();
}
void timerPallet(){
 if (powerUpActive) {
        powerUpTimer +=0.5;   // Increase power-up timer
        if (powerUpTimer >= powerUpDuration) {
            powerUpActive = false;
            powerUpTimer = 0;
            eatGhost = false;
        }
    }
}
//Keep updating game
void updateGame() {
    if (player.lives > 0) {
        float proposedX = player.x + player.dx;
        float proposedY = player.y + player.dy;
        
        if (canMove(proposedX, proposedY)) {
            player.x = proposedX;
            player.y = proposedY;
            canEat(proposedX, proposedY);
        } else {
            player.dx = 0;
            player.dy = 0;
        }
        float mouthSpeed = 0.1; // Control the speed of mouth opening/closing

        if (mouthOpening) {
        player.angle_open += mouthSpeed;
        if (player.angle_open >= 0.40 * M_PI) { // Maximum open angle
            mouthOpening = false;
        }
	    } else {
		player.angle_open -= mouthSpeed;
		if (player.angle_open <= 0.05 * M_PI) { // Minimum open angle
		    mouthOpening = true;
		}
        }
        checkCollisions();  // Check for collisions between Pacman and ghosts
        timerPallet();
        glutPostRedisplay();
        glutTimerFunc(1000 / 60, updateGame, 0);
    } else {
        displayGameOver(); // Display game over if no lives are left
    }
}

void *pacman_thread(void *arg) {
    // Pacman-related tasks
    glutKeyboardFunc(handleKeypress);
    updateGame();
    glutMainLoop();
    return NULL;
}

void runGame(int argc, char **argv) {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(950, 650);
    glutCreateWindow("Pacman Game");
    initGraphics(950, 650);
    initGame();
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(renderGame);
    glutTimerFunc(500, toggleBlink, 0);  // Start blinking timer
    // Create a separate thread for Pacman functionalities
    sem_init(&key_sem, 0, 2);
    pthread_t pacman_thread_id;
    pthread_create(&pacman_thread_id, NULL, pacman_thread, NULL);
}
