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
    Color* redColor = new Color(70,10,10);
    Color* whiteColor = new Color(230, 230, 230);
    float height = ballRad;
    float whiteBallDistance = 1.5;

    // White ball
    balls[0] = new Ball(-2, height, 0, ballRad, ballMass, whiteColor, true);
    // First line
    balls[1] = new Ball(whiteBallDistance, height, 0, ballRad, ballMass, redColor, false);
    // Second line
    balls[2] = new Ball(whiteBallDistance + ballSeparation, height, ballRad, ballRad, ballMass, redColor, false);
    balls[3] = new Ball(whiteBallDistance + ballSeparation, height, -ballRad, ballRad, ballMass, redColor, false);
    // Third line
    balls[4] = new Ball(whiteBallDistance + ballSeparation * 2, height, 0, ballRad, ballMass, redColor, false);
    balls[5] = new Ball(whiteBallDistance + ballSeparation * 2, height, ballRad * 2, ballRad, ballMass, redColor, false);
    balls[6] = new Ball(whiteBallDistance + ballSeparation * 2, height, -ballRad * 2, ballRad, ballMass, redColor, false);
    // Forth line
    balls[7] = new Ball(whiteBallDistance + ballSeparation * 3, height, ballRad, ballRad, ballMass, redColor, false);
    balls[8] = new Ball(whiteBallDistance + ballSeparation * 3, height, -ballRad, ballRad, ballMass, redColor, false);
    balls[9] = new Ball(whiteBallDistance + ballSeparation * 3, height, ballRad + ballRad * 2, ballRad, ballMass, redColor, false);
    balls[10] = new Ball(whiteBallDistance + ballSeparation * 3, height, -ballRad - ballRad * 2, ballRad, ballMass, redColor, false);
    // Fifth line
    balls[11] = new Ball(whiteBallDistance + ballSeparation * 4, height, 0, ballRad, ballMass, redColor, false);
    balls[12] = new Ball(whiteBallDistance + ballSeparation * 4, height, ballRad * 2, ballRad, ballMass, redColor, false);
    balls[13] = new Ball(whiteBallDistance + ballSeparation * 4, height, ballRad * 4, ballRad, ballMass, redColor, false);
    balls[14] = new Ball(whiteBallDistance + ballSeparation * 4, height, -ballRad * 2, ballRad, ballMass, redColor, false);
    balls[15] = new Ball(whiteBallDistance + ballSeparation * 4, height, -ballRad * 4, ballRad, ballMass, redColor, false);

    return balls;
}


void drawCue(int numSteps, float radius, float hl, Ball* whiteBall, float cueRotAng1, float cueRotAng2, float strength){
    Color* brownColor = new Color(216, 156, 104);
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
    glColor3ub(brownColor->getR(), brownColor->getG(), brownColor->getB());
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

void moveBalls(Ball** balls, float time, float tableLength, float tableWidth){
    for (int i = 0; i < 16; i++)
        balls[i]->updatePosAndVel(time, tableLength, tableWidth, balls);
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

    // Shuffle textures to get random ball positions
    GLuint blackTexture = textures[7];
    srand(time(0));
    random_shuffle(textures, textures + 15);

    // Swap black ball into 8th position
    int blackPosition = -1;
    for (int i = 0; i < 15; i++)
        if (textures[i] == blackTexture)
            blackPosition = i;
    GLuint aux = textures[3];
    textures[3] = textures[blackPosition];
    textures[blackPosition] = aux;

    return textures;
}


bool ballsNotMoving(Ball** balls){
    for (int i = 0; i < 16; i++)
        if (balls[i]->isMoving())
            return false;
    return true;
}

void drawBalls(Ball** balls, GLuint* textures){
    int i = 0;
    if (balls[0]->isInHole() && ballsNotMoving(balls))
        balls[0]->setInHole(false);
    balls[0]->draw(15,15,-1);
    for (int i = 1; i < 16; i++){
        balls[i]->draw(15,15, textures[i - 1]);
    }
}

void drawTable(std::vector< glm::vec3 > vertices, std::vector< glm::vec2 > uvs, std::vector< glm::vec3 > normals, GLuint texture){

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPushMatrix();

    glTranslatef(-2.67,0.53,-0.54);
    glScalef(0.035,0.035,0.035);
    glRotatef(90,0,0,1);
    glRotatef(90,0,1,0);

    glBegin(GL_QUADS);

    for (int i = 0; i < vertices.size(); i++){
        glNormal3f(normals[i][0], normals[i][1], normals[i][2]);
        glTexCoord2f(uvs[i][0], uvs[i][1]);
        glVertex3f(vertices[i][0], vertices[i][1], vertices[i][2]);
    }

    glEnd();
    glPopMatrix();
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
    float camPosY = whiteBall->getPosY() + rotatedY + 3; // Add a constant so the camera is a bit above the cue
    float camPosZ = whiteBall->getPosZ() + rotatedZ;

    gluLookAt(camPosX, camPosY, camPosZ, whiteBall->getPosX(), whiteBall->getPosY(), whiteBall->getPosZ(), 0, 1, 0);
}

void drawRoom(GLuint floorTexture, GLuint wallTexture){
    int roomLength = 20;
    int roomWidth = 15;
    int roomHeight = 20;
    int floorRepeat = 7;
    int wallRepeat = 3;
    float roomFloor = -2.49;

    glColor3ub(181, 150, 97);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floorTexture);

    // Floor

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
    bool applyTextures = false;
    bool slowMotion = false;
    bool flatShading = false;
    auto lastFrameTime = clock();



    do{
        auto currentTime = clock();
        float frameTime = (float)(currentTime - lastFrameTime);
        lastFrameTime = clock();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();





        //LIGHTING

        glPushMatrix();



        GLfloat red_light_diffuse[] = {1.0, 0.0, 0.0, 1.0};
        GLfloat position[] = {10.0f, 10.0f, 10.0f, 1.0f};  // light spot
        GLfloat ambient[]  = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat diffuse[]    = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat specular[] = {1.0f, 1.0f, 1.0f, 0.0f};
        GLfloat direction[] = {0.0f, 0.0f, -1.0f};

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0); // habilita la luz 0

        glLightfv(GL_LIGHT0, GL_POSITION, position);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction); //HACE ALGO?


        //glLightfv(GL_LIGHT0, GL_DIFFUSE, red_light_diffuse);

        //TEXUTRA CON LUCES
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glPopMatrix();

        // Set camera position
        if (viewMode == 0)
            gluLookAt(camX, camY, -camZ, 0, 0, 0, 0, 1, 0);
        else if (viewMode == 1)
            gluLookAt(0, 7, 0, 0, 0, 0, 0, 0, -1);
        else
            camOnTopOfCue(cueRotAng1, cueRotAng2, balls[0]);

        // Draw objects and apply physics
        drawRoom(floorTexture, wallTexture);
        drawBalls(balls, ballTextures);
        drawTable(vertices, uvs, normals, tableTexture);
        if (ballsNotMoving(balls))
           drawCue(100, 0.04, cueLength, balls[0], cueRotAng1, cueRotAng2, strength);
        if (!pause){
            applyCollisions(balls, ballRad, colliding);
            if (slowMotion)
                moveBalls(balls, frameTime / 120, tableLength, tableWidth);
            else
                moveBalls(balls, frameTime / 40, tableLength, tableWidth);
        }

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
            case SDL_KEYDOWN:{
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
                case SDLK_b:
                    applyTextures = !applyTextures;
                    break;
                case SDLK_c:
                    if (flatShading)
                        glShadeModel(GL_SMOOTH);
                    else
                        glShadeModel(GL_FLAT);
                    flatShading = !flatShading;
                    break;
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
