#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include "./Library/SOIL/include/SOIL.h"
#include "./Library/glut/glut.h"

#pragma comment( lib, "./Library/glut/glut32.lib" )
#pragma comment( lib, "./Library/SOIL/lib/SOIL.lib" )

int windowWidth = 800;
int windowHeight = 600;
int leftBound, rightBound;
int score = 0;
int numCoins = 5;
int Width,Height,channels;
float coinSpeed[5], leiSpeed = 5.0f;
unsigned char* image;
float coinX[5];
float coinY[5];
bool coinStop[5];
float leiX=0, leiY = windowHeight/2-80.0f,gaiaX=0,gaiaY=-windowHeight/2+40.0f;
float catcherX = 0.0f,times=5.0f;
float catcherY = -windowHeight / 2 + 30.0f;
float catcherWidth = 100.0f,best=0.0f;
GLuint lightTexture,leiTexture,helcaTexture,gaiaTexture,gaiaLeft,leiLeft,ready,go,darkLight;

void drawScene();
void update(int value);
void handleKeypress(unsigned char key, int x, int y);
void reshape(int width, int height);
bool right=true,left=false,leiRight=false,drawReady=false,drawGo=false;

void loadTexture(const char* str,GLuint &texture) {
        glEnable(GL_TEXTURE_2D);
        image = SOIL_load_image(str, &Width,&Height,&channels, SOIL_LOAD_RGBA);
        if (image) {
            texture = SOIL_create_OGL_texture(image, Width, Height, channels, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
            SOIL_free_image_data(image);
        }else {
            printf("SOIL loading error: '%s'\n", SOIL_last_result());
        }
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
void draw(float x, float y, GLuint &texture,float a,float b) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(x - a, y - b);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(x + a, y - b);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(x + a, y + b);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(x - a, y + b);

    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}
void init() {
    srand(time(NULL));
    for (int i = 0; i < numCoins; i++) {
        coinX[i] = (rand() % windowWidth);
        coinY[i] = windowHeight;
        coinStop[i] = false;
    }
    times = 3.5f;
    loadTexture("./Images/helca.png", helcaTexture);
    loadTexture("./Images/lei.png", leiTexture);
    loadTexture("./Images/lei.png", leiLeft);
    loadTexture("./Images/gaia.png", gaiaTexture);
    loadTexture("./Images/gaia.png", gaiaLeft);
    loadTexture("./Images/light.png", lightTexture);
    loadTexture("./Images/darkLight.png", darkLight);
    loadTexture("./Images/ready.png", ready);
    loadTexture("./Images/go.png", go);
    
    leftBound = -windowWidth / 2; rightBound = windowWidth / 2;
    for (int i = 0; i < numCoins; i++) coinSpeed[i] = rand() % 5 + 1;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
}

void drawText(std::string str,float f,float x,float y,int a) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x,y);
    std::string scoreStr =a==1? str + std::to_string((int)f): str + std::to_string(f);
    if (a == 0) scoreStr = scoreStr.substr(0, 11);
    for (char c : scoreStr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    leiY = windowHeight / 2 - 80.0f; gaiaY = -windowHeight / 2 + 70.0f;
    draw(0, 0, helcaTexture, windowWidth/2, windowHeight/2);
    if (leiRight)draw(leiX, leiY, leiTexture, 85, 85);
    else draw(leiX, leiY, leiTexture, -85, 85);
    for (int i = 0; i < numCoins-1; i++) {
        draw(coinX[i], coinY[i], lightTexture, 30, 30);
    }
    draw(coinX[numCoins-1], coinY[numCoins-1], darkLight, 30, 30);
    if (right)draw(gaiaX, gaiaY, gaiaTexture, 80, 80);
    if (left)draw(gaiaX, gaiaY, gaiaLeft, -80, 80);
    if (drawReady) draw(0, 20, ready, 150, 150);
    if (drawGo) draw(0, -145, go, 150, 150);
    
    
    drawText("Score: ",score,-windowWidth / 2 + 10.0f, windowHeight / 2 - 30.0f,1);
    drawText("Time: ",times, -windowWidth / 2 + 10.0f, windowHeight / 2 - 60.0f,0);
    drawText("Best Score: ",best, -windowWidth / 2 + 190.0f, windowHeight / 2 - 30.0f,1);
    glutSwapBuffers();
}
void updateScore(int value) {
    times -= 0.01;
    glutPostRedisplay();
    if (times < 0) {
        times = 30.0f;
        for (int i = 0; i < numCoins; i++) {
            coinX[i] = (rand() % windowWidth);
            coinY[i] = -(rand() % windowHeight);
            coinStop[i] = false;
        }
        score = 0; drawReady = false; drawGo = false;
        glutTimerFunc(10, update, 0);
    }else if (times >= 1.0f && times<=2.0f) {
        drawReady = true;
        glutTimerFunc(10, updateScore, 0);
    }else if (times <= 1.0f) {
        drawGo = true;
        glutTimerFunc(10, updateScore, 0);
    }else glutTimerFunc(10, updateScore, 0);
}
void updateCoin(int value) {
    for (int i = 0; i < numCoins; i++) {
        coinY[i] -= coinSpeed[i];
        if (coinY[i] < -windowHeight / 2) {
            coinStop[i] = true;
            coinY[i] = -windowHeight;
        }
        if (fabs(coinX[i] - gaiaX) < 50.0f && fabs(coinY[i] - gaiaY) < 50.0f) {
            coinStop[i] = true;
            coinY[i] = -windowHeight;
            if ((i == numCoins - 1) && score-2>=0) score -= 2;
            else score++;
            if (score < 0) score = 0;
        }
    }
    int cnt = 0;
    for (int i = 0; i < numCoins; i++) {
        if (coinStop[i]) cnt++;
    }
    glutPostRedisplay();
    if (cnt != numCoins) glutTimerFunc(10, updateCoin, 0);
    else{
        best = score > best ? score : best;
        glutTimerFunc(10, updateScore, 0);
    }
}
void update(int value) {
    for (int i = 0; i < numCoins; i++) {
        coinY[i] -= coinSpeed[i];
        if (coinY[i] < -windowHeight / 2) {
            coinX[i] = leiX+10;
            coinY[i] = leiY-80;
        }
        if (fabs(coinX[i] - gaiaX) < 50.0f && fabs(coinY[i] - gaiaY) < 50.0f) {
            coinX[i] = leiX+10;
            coinY[i] = leiY -80;
            if (i == numCoins - 1) score -= 2;
            else score++;
            if (score < 0) score = 0;
        } 
    }
    int speed = rand() % 10+2 ;
    if (leiX < leftBound) {
        leiSpeed = speed; leiRight = false;
        rightBound = rand() % windowWidth/2;
    }
    else if (leiX > rightBound) {
        leiSpeed = -speed; leiRight = true;
        leftBound = -rand() % windowWidth/2;
    }
    leiX += leiSpeed;
    times -= 0.01;
    glutPostRedisplay();
    if (times < 0){ 
        times = 3.5f;
        glutTimerFunc(10, updateCoin, 0);
    }else glutTimerFunc(10, update, 0);
}
void handleKeypress(unsigned char key, int x, int y) {
    if ((key == 'a' || key=='A') && gaiaX - 10.0f > 0.0f - (float)windowWidth / 2 + 50.0f) {
        gaiaX -= 15.0f; left = true; right = false;
    }else if ((key == 'd' || key == 'D') && gaiaX+10.0f < 0.0f+(float)windowWidth/2-50.0f){
        gaiaX += 15.0f; left = false; right = true;
    }
}
void reshape(int Width, int Height) {
    windowWidth = Width;
    windowHeight = Height;
    glViewport(0, 0, Width, Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-Width / 2, Width / 2, -Height / 2, Height / 2, -1, 1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Coin Catcher Game");
    srand(time(NULL));
    init();
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(reshape);
    glutTimerFunc(10, updateScore, 0);
    glutMainLoop();
    return 0;
}