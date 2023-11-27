#include <iostream>
#include <cstdlib>
#include <cmath>
#include <GLUT/GLUT.h>
using namespace std;
//this is a change
// Define window dimensions
const int windowWidth = 800;
const int windowHeight = 1000;

// Bird parameters
float birdX = 100.0f;
float birdY = windowHeight / 2.0f;
float birdRadius = 35.0f;
float birdVelocity = 1.0f;
float gravity = 0.5f;
float jumpForce = 10.0f;

// Pipe parameters
const int numPipes = 11;
float pipeWidth = 50.0f;
float pipeHeight = 300.0f;
const float pipeSpacing = 500.0f; // Increase the value as needed
float pipeVelocity = 5.0f;
float pipes[numPipes] = {0.0f};
float pipeGaps[numPipes] = {0.0f};

int score = 0;
bool gameEnded = false;

int window;

void myinit(){
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    window = glutCreateWindow("Flappy Bird");
}
void initializePipes() {
    const float totalPipeWidth = numPipes * pipeWidth + (numPipes - 1) * pipeSpacing;
    const float initialX = windowWidth + totalPipeWidth / 5.0f;

    for (int i = 0; i < numPipes; ++i) {
        pipes[i] = initialX + i * (pipeWidth + pipeSpacing);

        // Set random gap positions for each pipe
        pipeGaps[i] = rand() % (int)(windowHeight * 0.3f) + windowHeight * 0.2f;
    }
}


void drawScore() {
    glColor3f(1.0f, 1.0f, 1.0f);  // White color for the score
    glRasterPos2f(windowWidth - 100, windowHeight - 20);

    // Draw "Score: "
    std::string scoreText = "Score: ";
    for (char character : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, character);
    }

    // Convert the score to a string to draw each digit separately
    std::string scoreStr = std::to_string(score);
    for (char digit : scoreStr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, digit);
    }
}


// Function to draw a circle
void drawCircle(float x, float y, float radius) {
    const int numSegments = 100;
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < numSegments; ++i) {
        float theta = 2.0f * 3.141516 * (float)i/(float)numSegments;
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
    glColor3f(1.0f, 1.0f, 0.0f);  // Yellow color
    drawCircle(birdX, birdY, birdRadius);
}

float drawPipe(float x, float gapY) {
    float gapHeight = windowHeight * 0.3f; // You can adjust this value as needed
    glColor3f(0.0f, 1.0f, 0.0f);  // Green color
    // Top pipe
    drawRectangle(x, gapY + gapHeight, pipeWidth, windowHeight - gapY - gapHeight);
    // Bottom pipe
    drawRectangle(x, 0.0f, pipeWidth, gapY);
    return gapHeight;
}




// Function to update the game state
void update() {
    if (!gameEnded) {
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
                cout << "Score: " << score << endl;
            }
        }

    }

    glutPostRedisplay();  // Request a redraw
}

          
void restartGame() {
    birdY = windowHeight / 2.0f;
    birdVelocity = 0.0f;
    score = 0;
    gameEnded = false;
    initializePipes();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == ' ' && !gameEnded) {
        birdVelocity = jumpForce;
    }
    else if ((key == 'r' || key == 'R') && gameEnded) {
        restartGame();
    }
    else if (key == 'q' || key =='Q'){
        glutDestroyWindow(window);
        exit(0);
    }

}


void drawGround() {
    glColor3f(0, 1, 0);  // Gray color for the ground
    drawRectangle(0.0, 0.0, windowWidth, 50.0);
}


void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.094, 0.78, 0.98, 1);
    drawGround();
    drawBird();
    // Draw pipes with gaps
    for (int i = 0; i < numPipes; ++i) {
        drawPipe(pipes[i], pipeGaps[i]);
    }

    drawScore();  // Draw the score

    glutSwapBuffers();
}




int main(int argc, char** argv) {
    // Initialize pipe positions
    for (int i = 0; i < numPipes; ++i) {
        pipes[i] = windowWidth + i * pipeSpacing;
    }
    glutInit(&argc, argv);
    myinit();
    glOrtho(0.0, windowWidth, 0.0, windowHeight, -100.0, 100.0);
    glutDisplayFunc(drawScene);
    glutIdleFunc(update);
    glutKeyboardFunc(keyboard);

    glutSwapBuffers();

    initializePipes();

    glutMainLoop();

    return 0;
}
