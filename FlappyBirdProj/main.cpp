#include <iostream>
#include <cstdlib>
#include <cmath>
#include <GLUT/GLUT.h>
#include "stb_images.h"
using namespace std;

// Define window dimensions
const int windowWidth = 800;
const int windowHeight = 1000;

// Bird parameters
float birdX = 100.0f;
float birdY = windowHeight / 2.0f + 30;
float birdRadius = 20.0f;
float birdVelocity = 0.0f;
float gravity = 0.5f;
float jumpForce = 10.0f;

// -N- background cloud parameter
const int numClouds = 3;
float cloudRadius = 40.0f;
float cloudSpacing = 300.0f;
float clouds[numClouds] = { 0.0f };

// -M- Level variables
int currentLevel = 1;
int targetScore = 1; // num of pipes passed to complete the level
bool inStartMenu = true;

// Pipe parameters
const int numPipes = 4;
float pipeWidth = 50.0f;
float pipeHeight = 300.0f;
float pipeSpacing = 150.0f; // Increase the value as needed
/*
 -A- it would be better if we decreased the spacing as the level increase to make the level more challenging
 */
// -M- velocity increases for each level
float pipeVelocity = 5.0f; //-A-Initial pipe velocity won't be affected update shoud be after passing
float pipes[numPipes] = {0.0f};
float pipeGaps[numPipes] = {0.0f};

int score = 0;
bool gameEnded = false;
int window;

bool levelCompleted = false;
bool showLevelMenu = false;
bool showLevelMessage = false;
bool gamePaused = false;
//-A- Created the my init function
void myinit() {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    window = glutCreateWindow("Flappy Bird");
}

// -M- Added A start Menu
void drawStartMenu() {
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(windowWidth / 2 - 100, windowHeight / 2);
    string startText = "Flappy Bird";
    for (char character : startText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, character);
    }
    glRasterPos2f(windowWidth / 2 - 100, windowHeight / 2 - 50);
    string instructionText = "Press SPACE to jump";
    for (char character : instructionText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, character);
    }
    glRasterPos2f(windowWidth / 2 - 100, windowHeight / 2 - 100);
    startText = "Press ENTER to start";
    for (char character : startText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, character);
    }
    //-A- Added the press q to quit
    glRasterPos2f(windowWidth / 2 - 100, windowHeight / 2 - 150);
    string quitText = "Press Q to quit";
    for (char character : quitText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, character);
    }
}

void initializePipes() {
    const float totalPipeWidth = numPipes * pipeWidth + (numPipes - 1) * pipeSpacing;
    const float initialX = windowWidth;

    for (int i = 0; i < numPipes; ++i) {
        pipes[i] = initialX + i * (pipeWidth + pipeSpacing);

        // Set random gap positions for each pipe
        pipeGaps[i] = rand() % (int)(windowHeight * 0.3f) + windowHeight * 0.2f;
    }
}

void drawScore() {
    glColor3f(1.0f, 1.0f, 1.0f); // White color for the score
    glRasterPos2f(windowWidth - 100, windowHeight - 20);

    // Draw "Score: "
    string scoreText = "Score: ";
    for (char character : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, character);
    }

    // Convert the score to a string to draw each digit separately
    string scoreStr = to_string(score);
    for (char digit : scoreStr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, digit);
    }
}
void showLevel() {
    glColor3f(1, 1, 1);
    glRasterPos2f(windowWidth - 100, windowHeight - 50);
    string text_to_show = "Level: ";
    for (char character : text_to_show) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, character);
    }
    text_to_show = to_string(currentLevel);
    for (char character : text_to_show) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, character);
    }
}

// Function to draw a circle
void drawCircle(float x, float y, float radius) {
    const int numSegments = 100;
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < numSegments; ++i) {
        float theta = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(numSegments);
        float xPos = x + radius * cos(theta);
        float yPos = y + radius * sin(theta);
        glVertex2f(xPos, yPos);
    }
    glEnd();
}

// Function to draw a rectangle
void drawRectangle(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

// Function to draw the bird
void drawBird() {
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow color
    drawCircle(birdX, birdY, birdRadius);
}

//-N- Function to draw the cloud
void drawCloud(float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f); // White color for the clouds

    // Draw multiple circles to create a cloud shape
    for (int i = 0; i < 3; ++i) {
        drawCircle(x + i * 30.0f, y, cloudRadius);
    }
}

float drawPipe(float x, float gapY) {
    float gapHeight = windowHeight * 0.3f; // You can adjust this value as needed
    glColor3f(0.0f, 1.0f, 0.0f);          // Green color
    // Top pipe
    drawRectangle(x, gapY + gapHeight, pipeWidth, windowHeight - gapY - gapHeight);
    // Bottom pipe
    drawRectangle(x, 0.0f, pipeWidth, gapY);
    return gapHeight;
}

//-A- Added Function to adjust the game based on the level
void updateLevel() {
    currentLevel++;
    initializePipes();
    pipeVelocity = 5.0f + currentLevel;
    score = 0;           //-A-reset Score since we started a new level
    targetScore += 1;     // add more pipes to complete the level
    levelCompleted = false;
    showLevelMenu = false;
}
// Function to update the game state
void update() {
    if (!gameEnded && !gamePaused) {
        // Update cloud positions
        for (int i = 0; i < numClouds; ++i) {
            clouds[i] -= pipeVelocity;
            // If a cloud goes off-screen, reset its position
            if (clouds[i] + cloudRadius < 0) {
                clouds[i] = windowWidth + i * cloudSpacing;
            }
        }
        // Update bird position based on velocity
        birdY += birdVelocity;

        // Apply gravity to bird velocity
        birdVelocity -= gravity;

        // Check for collision with top and bottom boundaries
        if (birdY + birdRadius > windowHeight || birdY - birdRadius < 0) {
            cout << "Game Over!" << endl;
            gameEnded = true;
        }

        // Update pipe positions
        for (int i = 0; i < numPipes; ++i) {
            pipes[i] -= pipeVelocity;

            // Check if the bird is within the horizontal bounds of the current pipe
            if (birdX + birdRadius > pipes[i] && birdX - birdRadius < pipes[i] + pipeWidth) {
                // Check for collision only if the pipe is on the screen
                if (pipes[i] + pipeWidth > 0) {
                    float gapY = pipeGaps[i];
                    float gapHeight = drawPipe(pipes[i], gapY);
                    // Check for collision with bird (top part of the pipe)
                    if (birdY + birdRadius > gapY + gapHeight || birdY - birdRadius < gapY) {
                        cout << "Game Over!" << endl;
                        gameEnded = true;
                    }
                }
            }

            // If a pipe goes off-screen, reset its position and increase the score
            if (pipes[i] + pipeWidth < birdX - birdRadius) {
                pipes[i] = windowWidth;
                score++;
                cout << "Score: " << score << " | Level: " << currentLevel << endl;

                // -M- if the player passed the level
                if (score >= targetScore) {
                    levelCompleted = true;
                    showLevelMessage = true;
                    birdX = 100.0f;
                    birdY = windowHeight / 2.0f + 30;
                }
            }
        }

        // -A- Check for level completion and update accordingly
        if (levelCompleted) {
                    gamePaused = true;
                    showLevelMessage = true;
                }

        glutPostRedisplay(); // Request a redraw
    }
}


void restartGame() {
    birdY = windowHeight / 2.0f;
    birdVelocity = 0.0f;
    score = 0;
    gameEnded = false;
    currentLevel = 1;
    targetScore = 5;
    initializePipes();
    pipeVelocity = 5.0f + 0.5f * currentLevel;
    levelCompleted = false;
    showLevelMenu = false;
}

// Function to draw the level completion menu
void drawLevelMenu() {
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(windowWidth / 2 - 150, windowHeight / 2);
    string levelCompleteText = "Level " + to_string(currentLevel) + " Completed!";
    for (char character : levelCompleteText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, character);
    }
    glRasterPos2f(windowWidth / 2 - 100, windowHeight / 2 - 50);
    string nextLevelText = "Press N for the next level";
    for (char character : nextLevelText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, character);
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (inStartMenu) {
        if (key == 13) { // Enter in ASCII
            inStartMenu = false;
        }
    } else {
        if (key == ' ' && !gameEnded && !gamePaused) {
            birdVelocity = jumpForce;
        } else if ((key == 'r' || key == 'R') && gameEnded) {
            restartGame();
        } else if (key == 'q' || key == 'Q') {
            glutDestroyWindow(window);
            exit(0);
        } else if (key == 'n' || key == 'N') {
            if (showLevelMessage && gamePaused) {
                // Move to the next level
                showLevelMessage = false;
                levelCompleted = false;
                updateLevel();
                gamePaused = false; // Resume the game after moving to the next level
                birdX = 100.0f;
                birdY = windowHeight / 2.0f + 30;
            }
        } else if (key == 13 && gameEnded) {
            restartGame();
        }
    }
}



void drawGround() {
    glColor3f(0, 0, 0); // Gray color for the ground
    drawRectangle(0.0f, 0.0f, windowWidth, 100.0f);
}


void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.05, 0.78, 0.98, 1);

    if (inStartMenu) {
        drawStartMenu();
    } else {
        if (gameEnded) {
            drawStartMenu();
        } else {
            drawGround();
            drawBird();

            // Draw clouds in the background
            for (int i = 0; i < numClouds; ++i) {
                drawCloud(clouds[i], windowHeight - 100);
            }

            // Draw pipes with gaps
            for (int i = 0; i < numPipes; ++i) {
                drawPipe(pipes[i], pipeGaps[i]);
            }

            drawScore(); // Draw the score
            showLevel(); // Indicates which level the user is on

            // Draw the level completion message if necessary
            if (showLevelMessage) {
                glColor3f(1.0, 1.0, 1.0);
                glRasterPos2f(windowWidth / 2 - 150, windowHeight / 2);
                string levelPassText = "You passed Level " + to_string(currentLevel) + "!";
                for (char character : levelPassText) {
                    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, character);
                }
                glRasterPos2f(windowWidth / 2 - 100, windowHeight / 2 - 50);
                string nextLevelText = "Press N for the next level";
                for (char character : nextLevelText) {
                    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, character);
                }
                gamePaused = true; // Pause the game after showing the level completion message
            }
        }
    }

    glutSwapBuffers();
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    myinit();
    glOrtho(0.0, windowWidth, 0.0, windowHeight, -10.0, 10.0);
    glutDisplayFunc(drawScene);
    glutIdleFunc(update);
    glutKeyboardFunc(keyboard);

    // Initialize pipe positions
    for (int i = 0; i < numPipes; ++i) {
        pipes[i] = windowWidth + i * pipeSpacing;
    }
    glutSwapBuffers();

    initializePipes();

    glutMainLoop();

    return 0;
}
