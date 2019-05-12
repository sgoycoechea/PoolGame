#include <iostream>
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "FreeImage.h"
#include "include/Color.h"
#include "include/Point.h"
#include "include/Ball.h"
#include "include/objloader.h"
#include <math.h>
#include <vector>
#include <chrono>
#include <ctime>
#include <fstream>
#include <unistd.h>
#include <thread>
#include <algorithm>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glut.h>

#define _USE_MATH_DEFINES
#define FPS 60
#define M_PI 3.1415926

using namespace std;


void writeOutput(string text){
    std::ofstream outfile;
    outfile.open("output.txt", std::ios_base::app);
    outfile << text;
}


void updateCam(float &x,float &y, float &z, float x_angle, float y_angle, float radius){
    z = cos(y_angle*M_PI/180) * cos(x_angle*M_PI/180) * radius;
    x = sin(y_angle*M_PI/180) * cos(x_angle*M_PI/180) * radius;
    y = sin(x_angle*M_PI/180) * radius;
}

Ball** initializeBalls(double ballRad, double ballMass, float ballSeparation){
    Ball** balls = new Ball*[16];
    Color* whiteColor = new Color(230, 230, 230);
    float height = ballRad;
    float whiteBallDistance = 1.5;

    // White ball
    balls[0] = new Ball(-2, height, 0, ballRad, ballMass, whiteColor, true);
    // First line
    balls[1] = new Ball(whiteBallDistance, height, 0, ballRad, ballMass, whiteColor, false);
    // Second line
    balls[2] = new Ball(whiteBallDistance + ballSeparation, height, ballRad, ballRad, ballMass, whiteColor, false);
    balls[3] = new Ball(whiteBallDistance + ballSeparation, height, -ballRad, ballRad, ballMass, whiteColor, false);
    // Third line
    balls[4] = new Ball(whiteBallDistance + ballSeparation * 2, height, 0, ballRad, ballMass, whiteColor, false);
    balls[5] = new Ball(whiteBallDistance + ballSeparation * 2, height, ballRad * 2, ballRad, ballMass, whiteColor, false);
    balls[6] = new Ball(whiteBallDistance + ballSeparation * 2, height, -ballRad * 2, ballRad, ballMass, whiteColor, false);
    // Forth line
    balls[7] = new Ball(whiteBallDistance + ballSeparation * 3, height, ballRad, ballRad, ballMass, whiteColor, false);
    balls[8] = new Ball(whiteBallDistance + ballSeparation * 3, height, -ballRad, ballRad, ballMass, whiteColor, false);
    balls[9] = new Ball(whiteBallDistance + ballSeparation * 3, height, ballRad + ballRad * 2, ballRad, ballMass, whiteColor, false);
    balls[10] = new Ball(whiteBallDistance + ballSeparation * 3, height, -ballRad - ballRad * 2, ballRad, ballMass, whiteColor, false);
    // Fifth line
    balls[11] = new Ball(whiteBallDistance + ballSeparation * 4, height, 0, ballRad, ballMass, whiteColor, false);
    balls[12] = new Ball(whiteBallDistance + ballSeparation * 4, height, ballRad * 2, ballRad, ballMass, whiteColor, false);
    balls[13] = new Ball(whiteBallDistance + ballSeparation * 4, height, ballRad * 4, ballRad, ballMass, whiteColor, false);
    balls[14] = new Ball(whiteBallDistance + ballSeparation * 4, height, -ballRad * 2, ballRad, ballMass, whiteColor, false);
    balls[15] = new Ball(whiteBallDistance + ballSeparation * 4, height, -ballRad * 4, ballRad, ballMass, whiteColor, false);

    return balls;
}


void drawCue(int numSteps, float radius, float hl, Ball* whiteBall, float cueRotAng1, float cueRotAng2, float strength){
    float a = 0.0f;
    float step = (2 * M_PI) / (float)numSteps;
    double ballRad = whiteBall->getRad();


    glPushMatrix();

    // Move the cue next to the white ball
    glTranslatef(whiteBall->getPosX(), whiteBall->getPosY(), whiteBall->getPosZ());

    // Rotate the cue based on the mouse movements
    glRotatef(cueRotAng1, 0, 1, 0);
    glRotatef(cueRotAng2, 1, 0, 0);

    // Initial cue position and rotation
    glTranslatef(0, hl + ballRad + strength/4 * ballRad, 0);
    glRotatef(-90, 1, 0, 0);

    // Draw the cylinder
    glBegin(GL_TRIANGLE_STRIP);
    glColor3ub(152, 118, 84);
    for (int i = 0; i <= numSteps; i++)
    {
        float x = cos(a) * radius;
        float y = sin(a) * radius;
        glNormal3f(x / radius, y / radius, 0);
        //glTexCoord2f(0,i*2*radius*M_PI/numSteps);
        glVertex3f(x, y, -hl);
        glNormal3f(x / radius, y / radius, 0);
        //glTexCoord2f(2*hl,i*2*radius*M_PI/numSteps);
        glVertex3f(x, y, hl);
        a += step;
    }
    glEnd();
    glPopMatrix();
}

bool moveBalls(Ball** balls, float time, float tableLength, float tableWidth, GLuint* ballTextures, int &scoreStripped, int &scoreSolid){

    bool gameEnded = false;
    for (int i = 0; i < 16; i++){
        bool enteredHole = balls[i]->updatePosAndVel(time, tableLength, tableWidth, balls);

        if (enteredHole && !balls[i]->isWhiteBall()){
            if (i == 4)
                gameEnded = true;
            else if (i < 9)
                scoreStripped++;
            else
                scoreSolid++;
        }
    }

    if (scoreStripped == 7 || scoreSolid == 7)
        gameEnded = true;

    return gameEnded;
}

void hitBall(Ball* whiteBall, float cueRotAng1, float cueRotAng2, int strength){

    float strengthFactor = 2;

    // Convert to radians
    cueRotAng1 = cueRotAng1 * M_PI / 180;
    cueRotAng2 = cueRotAng2 * M_PI / 180;

    // Initial cue position
    float x = 0;
    float y = 1;
    float z = 0;

    // Vertical cue rotation
    float rotatedY = cos(cueRotAng2) * y - sin(cueRotAng2) * z;
    float rotatedZ = sin(cueRotAng2) * y + cos(cueRotAng2) * z;

    // Horizontal cue rotation
    float rotatedX = cos(cueRotAng1) * x - sin(cueRotAng1) * rotatedZ;
    rotatedZ = sin(cueRotAng1) * x + cos(cueRotAng1) * rotatedZ;

    whiteBall->setVelocity(new Point(rotatedX * strength * strengthFactor, 0, -rotatedZ * strength * strengthFactor));
}


void applyCollision(Ball** balls, int ball1Idx, int ball2Idx, double ballRad, bool** colliding){

    Ball* ball1 = balls[ball1Idx];
    Ball* ball2 = balls[ball2Idx];
    Point* ball1Pos = ball1->getPos();
    Point* ball2Pos = ball2->getPos();
    Point* ball1Vel = ball1->getVel();
    Point* ball2Vel = ball2->getVel();
    double ball1Mass = ball1->getMass();
    double ball2Mass = ball2->getMass();

    Point* normalVector = (*ball1Pos) - ball2Pos;
    double magnitude = normalVector->magnitude();

    if (magnitude < ballRad * 2 && !colliding[ball1Idx][ball2Idx] && !ball1->isInHole() && !ball2->isInHole()){

        colliding[ball1Idx][ball2Idx] = true;
        colliding[ball2Idx][ball1Idx] = true;

        Point* unitVector = (*normalVector) / magnitude;
        Point* tangentVector = new Point(-unitVector->getZ(), -unitVector->getY(), unitVector->getX());

        double vector1NormalMagnitude = unitVector->dotProduct(ball1Vel);
        double vector1TangentMagnitude = tangentVector->dotProduct(ball1Vel);
        double vector2NormalMagnitude = unitVector->dotProduct(ball2Vel);
        double vector2TangentMagnitude = tangentVector->dotProduct(ball2Vel);

        // Because of conservation of kinetic energy
        double newVector1NormalMagnitude = (vector1NormalMagnitude * (ball1Mass - ball2Mass) + 2 * ball2Mass * vector2NormalMagnitude) / (ball1Mass + ball2Mass);
        double newVector2NormalMagnitude = (vector2NormalMagnitude * (ball2Mass - ball1Mass) + 2 * ball1Mass * vector1NormalMagnitude) / (ball1Mass + ball2Mass);

        // These are the same
        double newVector1TangentMagnitude = vector1TangentMagnitude;
        double newVector2TangentMagnitude = vector2TangentMagnitude;

        Point* newVector1Normal = (*unitVector) * newVector1NormalMagnitude;
        Point* newVector1Tangent = (*tangentVector) * newVector1TangentMagnitude;
        Point* newVector2Normal = (*unitVector) * newVector2NormalMagnitude;
        Point* newVector2Tangent = (*tangentVector) * newVector2TangentMagnitude;

        Point* newVector1Velocity = (*newVector1Normal) + newVector1Tangent;
        Point* newVector2Velocity = (*newVector2Normal) + newVector2Tangent;

        ball1->setVelocity(newVector1Velocity);
        ball2->setVelocity(newVector2Velocity);
    }

    // If the balls are no longer touching, set colliding to false
    else if (magnitude > ballRad * 2){
        colliding[ball1Idx][ball2Idx] = false;
        colliding[ball2Idx][ball1Idx] = false;
    }
}

void applyCollisions(Ball** balls, double ballRad, bool** colliding){
    for (int i = 0; i < 16; i++)
        for(int j = i+1; j < 16; j++)
            applyCollision(balls, i, j, ballRad, colliding);
}

GLuint loadTexture(string file){
    FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(file.c_str());
    FIBITMAP* bitmap = FreeImage_Load(fif, file.c_str());
    bitmap = FreeImage_ConvertTo24Bits(bitmap);
    int texW = FreeImage_GetWidth(bitmap);
    int texH = FreeImage_GetHeight(bitmap);
    void* data = FreeImage_GetBits(bitmap);
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texW, texH, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    delete data;
    return texture;
}

GLuint* loadBallTextures(){
    GLuint* textures = new GLuint[15];

    // Get the texture of all 15 balls
    for (int i = 1; i <= 15; i++){
        string file = "resources/ball" + to_string(i) + ".jpg";
        textures[i-1] = loadTexture(file);
    }

    // Swap 8th ball into the corresponding position
    GLuint aux = textures[3];
    textures[3] = textures[7];
    textures[7] = aux;

    return textures;
}


bool ballsNotMoving(Ball** balls){
    for (int i = 0; i < 16; i++)
        if (balls[i]->isMoving())
            return false;
    return true;
}

void drawBalls(Ball** balls, GLuint* textures, bool applyTextures){
    int i = 0;
    if (balls[0]->isInHole() && ballsNotMoving(balls))
        balls[0]->setInHole(false);
    balls[0]->draw(15,15,-1);
    for (int i = 1; i < 16; i++){
        if (applyTextures)
            balls[i]->draw(15,15, textures[i - 1]);
        else
            balls[i]->draw(15,15, -1);
    }
}

void drawTable(std::vector< glm::vec3 > vertices, std::vector< glm::vec2 > uvs, std::vector< glm::vec3 > normals, GLuint texture, bool applyTextures){

    if (applyTextures){
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
    glPushMatrix();

    glTranslatef(-2.67,0.53,-0.54);
    glScalef(0.035,0.035,0.035);
    glRotatef(90,0,0,1);
    glRotatef(90,0,1,0);
    glColor3ub(230, 230, 230);

    glBegin(GL_QUADS);

    for (int i = 0; i < vertices.size(); i++){
        glNormal3f(normals[i][0], normals[i][1], normals[i][2]);
        glTexCoord2f(uvs[i][0], uvs[i][1]);
        glVertex3f(vertices[i][0], vertices[i][1], vertices[i][2]);
    }

    glEnd();
    glPopMatrix();
    if (applyTextures)
        glDisable(GL_TEXTURE_2D);
}


void camOnTopOfCue(float cueRotAng1, float cueRotAng2, Ball* whiteBall){
    float x = 0;
    float y = 1;
    float z = 0;

    // Convert to radians
    cueRotAng1 = cueRotAng1 * M_PI / 180;
    cueRotAng2 = cueRotAng2 * M_PI / 180;

    // Create a vector rotatedX, rotatedY, rotatedZ with the direction of the cue
    // Vertical cue rotation
    float rotatedY = cos(cueRotAng2) * y - sin(cueRotAng2) * z;
    float rotatedZ = sin(cueRotAng2) * y + cos(cueRotAng2) * z;
    // Horizontal cue rotation
    float rotatedX = cos(cueRotAng1) * x - sin(cueRotAng1) * rotatedZ;
    rotatedZ = sin(cueRotAng1) * x + cos(cueRotAng1) * rotatedZ;

    // Make camera distance constant by dividing by the magnitude of the vector and multiplying by a factor
    float magnitude = sqrt(pow(rotatedX, 2) + pow(rotatedY, 2) + pow(rotatedZ, 2));
    rotatedX *= 7 / magnitude;
    rotatedY *= 7 / magnitude;
    rotatedZ *= 7 / magnitude;

    float camPosX = whiteBall->getPosX() - rotatedX;
    float camPosY = whiteBall->getPosY() + rotatedY + 2; // Add a constant so the camera is a bit above the cue
    float camPosZ = whiteBall->getPosZ() + rotatedZ;

    gluLookAt(camPosX, camPosY, camPosZ, whiteBall->getPosX(), whiteBall->getPosY(), whiteBall->getPosZ(), 0, 1, 0);
}

void drawRoom(GLuint floorTexture, GLuint wallTexture, bool applyTextures){
    int roomLength = 20;
    int roomWidth = 15;
    int roomHeight = 20;
    int floorRepeat = 7;
    int wallRepeat = 3;
    float roomFloor = -2.49;

    if (applyTextures){
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
    }
    // Floor

    glColor3ub(230, 230, 230);

    glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    glTexCoord2f(floorRepeat, floorRepeat);
    glVertex3f(roomLength, roomFloor, roomWidth);
    glTexCoord2f(floorRepeat, 0);
    glVertex3f(roomLength, roomFloor, -roomWidth);
    glTexCoord2f(0, 0);
    glVertex3f(-roomLength, roomFloor, -roomWidth);
    glTexCoord2f(0, floorRepeat);
    glVertex3f(-roomLength, roomFloor, roomWidth);
    glEnd();

    if (applyTextures)
        glBindTexture(GL_TEXTURE_2D, wallTexture);

    // Ceiling
    glBegin(GL_QUADS);
    glNormal3f(0,-1,0);
    glTexCoord2f(wallRepeat, wallRepeat);
    glVertex3f(roomLength, roomFloor + roomHeight, roomWidth);
    glTexCoord2f(wallRepeat, 0);
    glVertex3f(roomLength, roomFloor + roomHeight, -roomWidth);
    glTexCoord2f(0, 0);
    glVertex3f(-roomLength, roomFloor + roomHeight, -roomWidth);
    glTexCoord2f(0, wallRepeat);
    glVertex3f(-roomLength, roomFloor + roomHeight, roomWidth);
    glEnd();

    //Walls
    glBegin(GL_QUADS);
    glNormal3f(0,0,-1);
    glTexCoord2f(0, wallRepeat);
    glVertex3f(roomLength, roomFloor, roomWidth);
    glTexCoord2f(wallRepeat, wallRepeat);
    glVertex3f(roomLength, roomFloor, -roomWidth);
    glTexCoord2f(wallRepeat, 0);
    glVertex3f(roomLength, roomFloor + roomHeight, -roomWidth);
    glTexCoord2f(0,0);
    glVertex3f(roomLength, roomFloor + roomHeight, roomWidth);

    glTexCoord2f(wallRepeat, wallRepeat);
    glNormal3f(1,0,0);
    glVertex3f(roomLength, roomFloor + roomHeight, -roomWidth);
    glTexCoord2f(wallRepeat, 0);
    glVertex3f(roomLength, roomFloor, -roomWidth);
    glTexCoord2f(0, 0);
    glVertex3f(-roomLength, roomFloor, -roomWidth);
    glTexCoord2f(0, wallRepeat);
    glVertex3f(-roomLength, roomFloor + roomHeight, -roomWidth);

    glTexCoord2f(0, wallRepeat);
    glNormal3f(0,0,-1);
    glVertex3f(-roomLength, roomFloor + roomHeight, roomWidth);
    glTexCoord2f(wallRepeat, wallRepeat);
    glVertex3f(-roomLength, roomFloor + roomHeight, -roomWidth);
    glTexCoord2f(wallRepeat, 0);
    glVertex3f(-roomLength, roomFloor, -roomWidth);
    glTexCoord2f(0, 0);
    glVertex3f(-roomLength, roomFloor, roomWidth);

    glTexCoord2f(wallRepeat, wallRepeat);
    glNormal3f(1,0,0);
    glVertex3f(roomLength, roomFloor, roomWidth);
    glTexCoord2f(wallRepeat, 0);
    glVertex3f(roomLength, roomFloor + roomHeight, roomWidth);
    glTexCoord2f(0,0);
    glVertex3f(-roomLength, roomFloor + roomHeight, roomWidth);
    glTexCoord2f(0, wallRepeat);
    glVertex3f(-roomLength, roomFloor, roomWidth);
    glEnd();

    if (applyTextures)
        glDisable(GL_TEXTURE_2D);
}

void initializeSDL(){
    if (SDL_Init(SDL_INIT_VIDEO)<0) {
        cerr << "Failed to initialize SDL: " << SDL_GetError() << endl;
        exit(1);
    }

    atexit(SDL_Quit);
    Uint32 flags = SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_OPENGL;

    if (SDL_SetVideoMode(1280, 720, 32, flags)==NULL) {
        cerr << "Failed to initialize view mode: " << SDL_GetError() << endl;
        exit(1);
    }

    if (SDL_EnableKeyRepeat(30, 10)<0) {
        cerr << "Failed to initialize key-repeat mode: " << SDL_GetError() << endl;
        exit(1);
    }
}

// Returns a 16*16 boolean matrix with false in every entry
bool** getCollisionMatrix(){
    bool** colliding = new bool*[16];
        for (int i = 0; i < 16; i++)
            colliding[i] = new bool[16];
        for(int i = 0; i < 16; i++)
            for(int j = 0; j < 16; j++)
                colliding[i][j] = false;
    return colliding;
}

void setLighting(float lightPositionX, float lightPositionZ, int lightColor, bool applyTextures){

    float lightR = 2.0f;
    float lightG = 2.0f;
    float lightB = 2.0f;

    if (!applyTextures){
        lightR = 1.0f;
        lightG = 1.0f;
        lightB = 1.0f;
    }

    if (lightColor == 1)
        lightR = 8.0f;
    if (lightColor == 2)
        lightG = 8.0f;
    if (lightColor == 3)
        lightB = 8.0f;

    GLfloat position[] = {lightPositionX, 5, lightPositionZ, 1};
    GLfloat ambient[]  = {lightR / 10, lightG / 10, lightB / 10, 0.4f};
    GLfloat specular[]  = {lightR, lightG, lightB, 2.0f};
    GLfloat diffuse[]  = {lightR, lightG, lightB, 2.0f};
    GLfloat direction[] = {0, -1, 0};

    GLfloat materialAmbient[] = {.2, .2, .2, 1};
    GLfloat materialDiffuse[] = {.8, .8, .8, 1};

    glPushMatrix();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glPopMatrix();
}

void drawHUD(int time, int scoreStripped, int scoreSolid, float strength, bool gameEnded){

    float red = 0;
    float green = 255;
    float strProportion = (strength - 4.0) / 16.0;

    if (strProportion <= 0.5){
        red = 255 * strProportion * 2;
    }
    else{
        red = 255;
        green = 255 * (1 - strProportion) * 2;
    }

    // Draw HUD
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);

    string timeLabel = "Time: " + to_string(time);
    string scoreLabel1 = "Stripped: " + to_string(scoreStripped);
    string scoreLabel2 = "Solid: " + to_string(scoreSolid);
    string endGameLabel = "Game finished!";
    string helpLabel = "Press H for Help";


    glColor3ub(170, 170, 170);

    glRasterPos2f(0.25, 0.96);
    for(int i = 0; i < scoreLabel1.size(); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreLabel1[i]);

    glRasterPos2f(0.1, 0.96);
    for(int i = 0; i < timeLabel.size(); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timeLabel[i]);

    glRasterPos2f(0.35, 0.96);
    for(int i = 0; i < scoreLabel2.size(); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreLabel2[i]);

    glRasterPos2f(0.65, 0.96);
    for(int i = 0; i < helpLabel.size(); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, helpLabel[i]);

    if (gameEnded){
        glRasterPos2f(0.5, 0.96);
        for(int i = 0; i < endGameLabel.size(); i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, endGameLabel[i]);
    }

    glBegin(GL_QUADS);
    glColor3ub(red, green, 0);
    glVertex2f(0.8, 0.94);
    glVertex2f(0.8, 0.97);
    glVertex2f(0.81 + strProportion * 0.1, 0.97);
    glVertex2f(0.81 + strProportion * 0.1, 0.94);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex2f(0.8, 0.94);
    glVertex2f(0.8, 0.97);
    glVertex2f(0.91, 0.97);
    glVertex2f(0.91, 0.94);
    glVertex2f(0.8, 0.94);
    glEnd();

    glBegin(GL_QUADS);
    glColor3ub(30, 30, 30);
    glVertex2f(0, 0.9);
    glVertex2f(0, 1);
    glVertex2f(1, 1);
    glVertex2f(1, 0.9);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}


void drawMenu(bool showMenu, GLuint helpTexture){
    if(showMenu){
        // Draw Menu
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, helpTexture);


        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, 1.0, 1.0, 0.0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();



        glBegin(GL_QUADS);
        glColor3f(255, 255, 255);
        glTexCoord2f(0,1);
        glVertex2f(0.03 , 0.05);
        glTexCoord2f(0, 0);
        glVertex2f(0.03, 0.6);
        glTexCoord2f(1, 0);
        glVertex2f(0.3, 0.6);
        glTexCoord2f(1, 1);
        glVertex2f(0.3, 0.05);
        glEnd();

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, helpTexture);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);
    }
}




int main(int argc, char *argv[]) {

    // Initialize viewMode 0 camera
    float camX = 0;
    float camY = 6;
    float camZ = -6;
    float camAngA = 0;
    float camAngB = -45;
    float camRad = -8.4852; // sqrt(y^2 + z^2)

    bool moveCam = false;
    bool moveCue = false;

    initializeSDL();

    glMatrixMode(GL_PROJECTION);
    float color = 0;
    glClearColor(color, color, color, 1);
    gluPerspective(45, 1280/720.f, 0.1, 100);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);

    // Load textures and models
    GLuint* ballTextures = loadBallTextures();
    GLuint tableTexture = loadTexture("resources/PoolTable.jpg");
    GLuint floorTexture = loadTexture("resources/floor.jpg");
    GLuint wallTexture = loadTexture("resources/wall.jpg");
    GLuint helpTexture = loadTexture("resources/help.jpg");
    std::vector< glm::vec3 > vertices, normals;
    std::vector< glm::vec2 > uvs;
    loadOBJ("resources/PoolTable.obj", vertices, uvs, normals);

    // Size of table, balls and cue
    float tableLength = 7.9;
    float tableWidth = 3.5;
    double ballRad = tableLength / 50;
    double ballMass = 1;
    float ballSeparation = ballRad*1.75;
    int cueLength = 2.3;

    // Initial cue rotation
    float cueRotAng1 = 90;
    float cueRotAng2 = -80;

    bool** colliding = getCollisionMatrix(); // if balls i and j are colliding => colliding[i][j] = true and colliding[j][i] = true
    Ball** balls = initializeBalls(ballRad, ballMass, ballSeparation);

    SDL_Event event;
    float strength = 4;
    int viewMode = 0;
    bool quit = false;
    bool pause = false;
    bool wireframe = false;
    bool applyTextures = true;
    bool slowMotion = false;
    bool flatShading = false;
    auto lastFrameTime = clock();
    int lightColor = 0;
    int scoreStripped = 0;
    int scoreSolid = 0;
    float lightPositionX = 0;
    float lightPositionZ = 0;
    bool showMenu = false;
    bool gameEnded = false;

    do{
        auto currentTime = clock();
        float frameTime = (float)(currentTime - lastFrameTime);
        lastFrameTime = clock();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        // Set camera position
        if (viewMode == 0)
            gluLookAt(camX, camY, -camZ, 0, 0, 0, 0, 1, 0);
        else if (viewMode == 1)
            gluLookAt(0, 7, 0, 0, 0, 0, 0, 0, -1);
        else
            camOnTopOfCue(cueRotAng1, cueRotAng2, balls[0]);

        setLighting(lightPositionX, lightPositionZ, lightColor, applyTextures);

        // Draw objects and apply physics
        drawRoom(floorTexture, wallTexture, applyTextures);
        drawBalls(balls, ballTextures, applyTextures);
        drawTable(vertices, uvs, normals, tableTexture, applyTextures);
        if (ballsNotMoving(balls))
           drawCue(10, 0.04, cueLength, balls[0], cueRotAng1, cueRotAng2, strength);
        if (!pause && !gameEnded){
            applyCollisions(balls, ballRad, colliding);
            if (slowMotion)
                gameEnded = moveBalls(balls, frameTime / 120, tableLength, tableWidth, ballTextures, scoreStripped, scoreSolid);
            else
                gameEnded = moveBalls(balls, frameTime / 40, tableLength, tableWidth, ballTextures, scoreStripped, scoreSolid);
        }

        drawHUD(currentTime / 1000, scoreStripped, scoreSolid, strength, gameEnded);
        drawMenu(showMenu, helpTexture);

        // Process events
        int xm,ym;
        SDL_GetMouseState(&xm, &ym);
        while(SDL_PollEvent(&event)){
            switch(event.type){

            case SDL_MOUSEBUTTONDOWN:
                if (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3))
                    moveCam=true;
                if (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
                    moveCue=true;
                break;
            case SDL_MOUSEBUTTONUP:
                moveCam = false;
                if (moveCue){
                    moveCue = false;
                    if (ballsNotMoving(balls) && !pause && strength > 4)
                        hitBall(balls[0], cueRotAng1, cueRotAng2, strength);
                    strength = 4;
                }
                break;

            case SDL_MOUSEMOTION:
                // Move cam
                if (moveCam && viewMode == 0){
                    if (event.motion.yrel < 0 && camAngB < 80 )
                        camAngB -= event.motion.yrel*0.4;
                    else if (event.motion.yrel >= 0 && camAngB > -80)
                        camAngB -= event.motion.yrel*0.4;

                    camAngA += event.motion.xrel * 0.4;
                    updateCam(camX, camY, camZ, camAngB, camAngA, camRad);
                }

                // Move cue
                if (moveCue && !pause && ballsNotMoving(balls)){
                    cueRotAng1+= event.motion.xrel * 0.4;
                    strength += event.motion.yrel * 0.4;

                    if (strength > 20) strength = 20;
                    if (strength < 4) strength = 4;
                    while(cueRotAng1 >= 360) cueRotAng1 -= 360;
                    while(cueRotAng1 < 0) cueRotAng1 += 360;
                }

                break;
            case SDL_QUIT:
                quit = true;
                break;

            case SDL_KEYUP:{
                switch(event.key.keysym.sym){
                    case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_q:
                    quit = true;
                    break;
                case SDLK_v:
                    viewMode++;
                    if (viewMode > 2)
                        viewMode = 0;
                    break;
                case SDLK_p:
                    pause = !pause;
                    break;
                case SDLK_m:
                    slowMotion = !slowMotion;
                    break;
                case SDLK_n:
                    if (wireframe)
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    else
                        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    wireframe = !wireframe;
                    break;
                case SDLK_t:
                    applyTextures = !applyTextures;
                    break;
                case SDLK_c:
                    if (flatShading)
                        glShadeModel(GL_SMOOTH);
                    else
                        glShadeModel(GL_FLAT);
                    flatShading = !flatShading;
                    break;
                case SDLK_l:
                    lightColor++;
                    if (lightColor > 3)
                        lightColor = 0;
                    break;
                case SDLK_h:
                    showMenu = !showMenu;
                    break;
                default:
                    break;
                }
            }
            case SDL_KEYDOWN:{
                switch(event.key.keysym.sym){

                case SDLK_s:{
                    if (viewMode == 0){
                        camRad -= .05;
                        updateCam(camX, camY, camZ, camAngB, camAngA, camRad);
                    }
                    }
                    break;
                case SDLK_w:{
                    if (camRad < 0 && viewMode == 0)
                        camRad += .05;
                    updateCam(camX, camY, camZ, camAngB, camAngA, camRad);
                    }
                    break;
                case SDLK_DOWN:
                    if (lightPositionX > -5)
                        lightPositionX -= .5;
                    break;
                case SDLK_UP:
                    if (lightPositionX < 5)
                        lightPositionX += .5;
                    break;
                case SDLK_RIGHT:
                    if (lightPositionZ < 5)
                        lightPositionZ += .5;
                    break;
                case SDLK_LEFT:
                    if (lightPositionZ > -5)
                        lightPositionZ -= .5;
                    break;

                default:
                    break;
                }
                }
                break;
            default:
                break;
            }
        }
        SDL_GL_SwapBuffers();

        // Force 30fps cap
        currentTime = clock();
        frameTime = (float)(currentTime - lastFrameTime);
        if (frameTime < 33.3)
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(33.3 - frameTime)));


    }while(!quit);
    return 0;
}
