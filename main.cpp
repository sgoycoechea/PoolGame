#include <iostream>
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "FreeImage.h"
#include "include/Color.h"
#include "include/Point.h"
#include "include/Ball.h"
#include <math.h>
#include <vector>
#include <chrono>
#include <ctime>
#include <fstream>
#include <unistd.h>
#include <thread>
#include <algorithm>
#define _USE_MATH_DEFINES
#define FPS 60
#define M_PI 3.1415926

using namespace std;
void actualizarCam(float &x,float &y, float &z, float x_angle, float y_angle,float radius){
    z = cos(y_angle*M_PI/180) * cos(x_angle*M_PI/180) * radius;
    x = sin(y_angle*M_PI/180) * cos(x_angle*M_PI/180) * radius;
    y = sin(x_angle*M_PI/180) * radius;
}

void actualizarCue(float &x,float &y, float &z, float x_angle, float y_angle,float radius){
    z = cos(y_angle*M_PI/180) * cos(x_angle*M_PI/180) * radius;
    x = sin(y_angle*M_PI/180) * cos(x_angle*M_PI/180) * radius;
    y = sin(x_angle*M_PI/180) * radius;
}

 void drawQuad(vector<Point*> points, Color* color){
    glBegin(GL_QUADS);
        glColor3ub(color->getR(), color->getG(), color->getB());
        glVertex3f(points[0]->getX(), points[0]->getY(), points[0]->getZ());
        glVertex3f(points[1]->getX(), points[1]->getY(), points[1]->getZ());
        glVertex3f(points[2]->getX(), points[2]->getY(), points[2]->getZ());
        glVertex3f(points[3]->getX(), points[3]->getY(), points[3]->getZ());
    glEnd();
}
void drawPrism(vector<Point*> points, Color* topColor, Color* restOfColors){
        // TOP
        vector<Point*> topPoints {points[0], points[1], points[2], points[3]};
        drawQuad(topPoints, topColor);
        // BOTTOM
        vector<Point*> bottomPoints {points[4], points[5], points[6], points[7]};
        drawQuad(bottomPoints, restOfColors);
        // FRONT
        vector<Point*> frontPoints {points[0], points[4], points[5], points[1]};
        drawQuad(frontPoints, restOfColors);
        // BACK
        vector<Point*> backPoints {points[3], points[2], points[6], points[7]};
        drawQuad(backPoints, restOfColors);
        // LEFT
        vector<Point*> leftPoints {points[0], points[3], points[7], points[4]};
        drawQuad(leftPoints, restOfColors);
        // RIGHT
        vector<Point*> rightPoints {points[1], points[5], points[6], points[2]};
        drawQuad(rightPoints, restOfColors);
}

Ball** initializeBalls(double ballRad, double ballMass, float ballSeparation){
    Ball** balls = new Ball*[16];
    Color* redColor = new Color(70,10,10);
    Color* whiteColor = new Color(230, 230, 230);

    // White ball
    balls[0] = new Ball(-2, ballRad, 0, ballRad, ballMass, whiteColor);
    // First line
    balls[1] = new Ball(1, ballRad, 0, ballRad, ballMass, redColor);
    // Second line
    balls[2] = new Ball(1+ballSeparation, ballRad, ballRad, ballRad, ballMass, redColor);
    balls[3] = new Ball(1+ballSeparation, ballRad, -ballRad, ballRad, ballMass, redColor);
    // Third line
    balls[4] = new Ball(1+ballSeparation*2, ballRad, 0, ballRad, ballMass, redColor);
    balls[5] = new Ball(1+ballSeparation*2, ballRad, ballRad*2, ballRad, ballMass, redColor);
    balls[6] = new Ball(1+ballSeparation*2, ballRad, -ballRad*2, ballRad, ballMass, redColor);
    // Forth line
    balls[7] = new Ball(1+ballSeparation*3, ballRad, ballRad, ballRad, ballMass, redColor);
    balls[8] = new Ball(1+ballSeparation*3, ballRad, -ballRad, ballRad, ballMass, redColor);
    balls[9] = new Ball(1+ballSeparation*3, ballRad, ballRad + ballRad*2, ballRad, ballMass, redColor);
    balls[10] = new Ball(1+ballSeparation*3, ballRad, -ballRad - ballRad*2, ballRad, ballMass, redColor);
    // Fifth line
    balls[11] = new Ball(1+ballSeparation*4, ballRad, 0, ballRad, ballMass, redColor);
    balls[12] = new Ball(1+ballSeparation*4, ballRad, ballRad*2, ballRad, ballMass, redColor);
    balls[13] = new Ball(1+ballSeparation*4, ballRad, ballRad*2*2, ballRad, ballMass, redColor);
    balls[14] = new Ball(1+ballSeparation*4, ballRad, -ballRad*2, ballRad, ballMass, redColor);
    balls[15] = new Ball(1+ballSeparation*4, ballRad, -ballRad*2*2, ballRad, ballMass, redColor);

    return balls;
}

void writeOutput(string text){
    std::ofstream outfile;
    outfile.open("output.txt", std::ios_base::app);
    outfile << text;
}

void drawBalls(Ball** balls, GLuint* textures){
    int i = 0;
    balls[0]->draw(50,50,-1);
    for (int i = 1; i < 16; i++){
        balls[i]->draw(50,50, textures[i - 1]);
    }
}


void drawCue(int numSteps,float radius, float hl,float* arrX_1,float* arrY_1, Ball* whiteBall, int ang, float rotX, float rotY, float rotZ, int strength){


    Color* brownColor = new Color(216, 156, 104);
    float a = 0.0f;
    float step = (2*M_PI) / (float)numSteps;

    glPushMatrix();

    double rad = whiteBall->getRad();
    glTranslatef(whiteBall->getPosX(), whiteBall->getPosY(), whiteBall->getPosZ());
    glRotatef(ang, rotX, rotY, rotZ);
    glTranslatef(0, hl + rad + strength/4 * rad, 0);
    glRotatef(-90, 1, 0, 0);


    glBegin(GL_TRIANGLE_STRIP);
    glColor3ub(brownColor->getR(), brownColor->getG(), brownColor->getB());
    for (int i = 0; i <= numSteps; i++)
    {
        float x = cos(a) * radius;
        float y = sin(a) * radius;
        arrX_1[i]=x;
        arrY_1[i]=y;
        glNormal3f(x/radius,y/radius,0);
        glTexCoord2f(0,i*2*radius*M_PI/numSteps);
        glVertex3f(x,y,-hl);
        glNormal3f(x/radius,y/radius, 0);
        glTexCoord2f(2*hl,i*2*radius*M_PI/numSteps);
        glVertex3f(x,y, hl);
        a += step;
    }
    glEnd();
    glPopMatrix();
}

void moveBalls(Ball** balls, float time, float lTop, float wTop, float wBorder){
    for (int i = 0; i < 16; i++)
        balls[i]->updatePosAndVel(time, lTop, wTop, wBorder, balls);
}

void hitBall(Ball* whiteBall, float cueRotX, float cueRotY, float cueRotZ, float cueAngA, float cueAngB, int strength){
    whiteBall->setVelocity(new Point(cueRotX * strength, 0, cueRotZ * strength));
}


void applyCollision(Ball** balls, int ball1Idx, int ball2Idx, double ballRad, int** lastCollisions){

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

    int currentTime = (int)(clock());

    if (magnitude < ballRad * 2 && currentTime - lastCollisions[ball1Idx][ball2Idx] > 50){

        lastCollisions[ball1Idx][ball2Idx] = currentTime;
        lastCollisions[ball2Idx][ball1Idx] = currentTime;

        Point* unitVector = (*normalVector) / magnitude;
        Point* tangentVector = new Point(-unitVector->getZ(), -unitVector->getY(), unitVector->getX());



        // Don't let balls get inside each other
        /*
        double diff = ballRad * 2 - magnitude;
        double diffX1 = unitVector->getX() * diff / 2;
        double diffY1 = unitVector->getY() * diff / 2;
        double diffZ1 = unitVector->getZ() * diff / 2;
        double diffX2 = unitVector->getX() * diff / 2;
        double diffY2 = unitVector->getY() * diff / 2;
        double diffZ2 = unitVector->getZ() * diff / 2;

        if (ball1->getPosX() > ball2->getPosX()){
            diffX2 = -diffX2;}
        else{
            diffX1 = -diffX1;}
        if (ball1->getPosY() > ball2->getPosY()){
            diffY2 = -diffY2;}
        else{
            diffY1 = -diffY1;}
        if (ball1->getPosZ() > ball2->getPosZ()){
            diffZ2 = -diffZ2;}
        else{
            diffZ1 = -diffZ1;}

        ball1->setPos(new Point(ball1->getPosX() + diffX1, ball1->getPosY() + diffY1, ball1->getPosZ() + diffZ1));
        ball2->setPos(new Point(ball2->getPosX() + diffX2, ball2->getPosY() + diffY2, ball2->getPosZ() + diffZ2));

        */



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
}

void applyCollisions(Ball** balls, double ballRad, int** lastCollisions){
    for (int i = 0; i < 16; i++)
        for(int j = i+1; j < 16; j++)
            applyCollision(balls, i, j, ballRad, lastCollisions);
}

void drawTable(float lTop, float wTop, float lBottom, float wBottom, float h, float wBorder, float hBorder){

    Color* greenColor = new Color(35, 123, 86);
    Color* brownColor = new Color(60, 36, 21);

    vector<Point*> table;
    table.push_back(new Point(-lTop/2,0,wTop/2));
    table.push_back(new Point(lTop/2,0,wTop/2));
    table.push_back(new Point(lTop/2,0,-wTop/2));
    table.push_back(new Point(-lTop/2,0,-wTop/2));
    table.push_back(new Point(-lBottom/2,-h,wBottom/2));
    table.push_back(new Point(lBottom/2,-h,wBottom/2));
    table.push_back(new Point(lBottom/2,-h,-wBottom/2));
    table.push_back(new Point(-lBottom/2,-h,-wBottom/2));

    vector<Point*> border1;
    border1.push_back(new Point(-lTop/2, hBorder, wTop/2));
    border1.push_back(new Point(lTop/2, hBorder, wTop/2));
    border1.push_back(new Point(lTop/2, hBorder, wTop/2 - wBorder));
    border1.push_back(new Point(-lTop/2, hBorder, wTop/2 - wBorder));
    border1.push_back(new Point(-lTop/2, 0,wTop/2));
    border1.push_back(new Point(lTop/2, 0,wTop/2));
    border1.push_back(new Point(lTop/2, 0,wTop/2 - wBorder));
    border1.push_back(new Point(-lTop/2, 0,wTop/2 - wBorder));

    vector<Point*> border2;
    border2.push_back(new Point(-lTop/2, hBorder, -wTop/2 + wBorder));
    border2.push_back(new Point(lTop/2, hBorder, -wTop/2 + wBorder));
    border2.push_back(new Point(lTop/2, hBorder, -wTop/2));
    border2.push_back(new Point(-lTop/2, hBorder, -wTop/2));
    border2.push_back(new Point(-lTop/2, 0, -wTop/2 + wBorder));
    border2.push_back(new Point(lTop/2, 0, -wTop/2 + wBorder));
    border2.push_back(new Point(lTop/2, 0, -wTop/2));
    border2.push_back(new Point(-lTop/2, 0, -wTop/2));

    vector<Point*> border3;
    border3.push_back(new Point(lTop/2 - wBorder,hBorder,wTop/2));
    border3.push_back(new Point(lTop/2, hBorder,wTop/2));
    border3.push_back(new Point(lTop/2, hBorder,-wTop/2));
    border3.push_back(new Point(lTop/2 - wBorder,hBorder,-wTop/2));
    border3.push_back(new Point(lTop/2 - wBorder, 0,wTop/2));
    border3.push_back(new Point(lTop/2, 0,wTop/2));
    border3.push_back(new Point(lTop/2, 0,-wTop/2));
    border3.push_back(new Point(lTop/2 - wBorder, 0,-wTop/2));

    vector<Point*> border4;
    border4.push_back(new Point(-lTop/2,hBorder,wTop/2));
    border4.push_back(new Point(-lTop/2+ wBorder, hBorder,wTop/2));
    border4.push_back(new Point(-lTop/2+ wBorder, hBorder,-wTop/2));
    border4.push_back(new Point(-lTop/2,hBorder,-wTop/2));
    border4.push_back(new Point(-lTop/2, 0,wTop/2));
    border4.push_back(new Point(-lTop/2 + wBorder, 0,wTop/2));
    border4.push_back(new Point(-lTop/2 +  wBorder, 0,-wTop/2));
    border4.push_back(new Point(-lTop/2 , 0,-wTop/2));

    // Draw table
    drawPrism(table, greenColor, brownColor);
    drawPrism(border1, brownColor, brownColor);
    drawPrism(border2, brownColor, brownColor);
    drawPrism(border3, brownColor, brownColor);
    drawPrism(border4, brownColor, brownColor);
}

GLuint* loadTextures(){
    GLuint* textures = new GLuint[15];

    // Get the texture of all 15 balls
    for (int i = 1; i <= 15; i++){
        string file = "images/ball" + to_string(i) + ".jpg";

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

        textures[i-1] = texture;
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


float x,y,z;
int main(int argc, char *argv[]) {
    float anga=0;
    float angb=-45;
    float rad=-8.4852; // sqrt(y^2 + z^2)
    bool moveCam=false;
    bool moveCue=false;
    if(SDL_Init(SDL_INIT_VIDEO)<0) {
        cerr << "No se pudo iniciar SDL: " << SDL_GetError() << endl;
        exit(1);
    }
    x=0;
    y=6;
    z=-6;
    atexit(SDL_Quit);

    Uint32 flags = SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_OPENGL;

    if(SDL_SetVideoMode(640, 480, 32, flags)==NULL) {
        cerr << "No se pudo establecer el modo de video: " << SDL_GetError() << endl;
        exit(1);
    }

    if(SDL_EnableKeyRepeat(10, 10)<0) {
        cerr << "No se pudo establecer el modo key-repeat: " << SDL_GetError() << endl;
        exit(1);
    }

    glMatrixMode(GL_PROJECTION);

    float color = 0;
    glClearColor(color, color, color, 1);

    gluPerspective(45, 640/480.f, 0.1, 100);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);

    GLuint* textures = loadTextures();


    // ---- Size of table and balls -----

    // Pool table's length = width * 2
    // Pool table's length = ballRadius * 50
    float lTop = 8;
    float wTop = lTop/2;
    double ballRad = lTop / 50;
    double ballMass = 1;
    float ballSeparation = ballRad*1.75;

    float lBottom = 5;
    float wBottom = 3;

    float h = 1.6;
    float wBorder = h/6;
    float hBorder = h/4;
    // -----------------------------------

    // --------- Colision checks ---------

    int** lastCollisions = new int*[16];
        for (int i = 0; i < 16; i++)
            lastCollisions[i] = new int[16];
        for(int i = 0; i < 16; i++)
            for(int j = 0; j < 16; j++)
                lastCollisions[i][j] = 0;

    // -----------------------------------

    int cueAngA = 0;
    int cueAngB = 0;

    float cueRotX = 0;
    float cueRotY = 0;
    float cueRotZ = 0;

    int strength = 4;

    auto lastFrameTime = clock();
    bool quit=false;
    SDL_Event event;
    Ball** balls = initializeBalls(ballRad, ballMass, ballSeparation);

    do{
        auto currentTime = clock();
        float frameTime = (float)(currentTime - lastFrameTime);
        lastFrameTime = clock();


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(x,y,-z,0,0,0,0,1,0);


        drawTable(lTop, wTop, lBottom, wBottom, h, wBorder, hBorder);

        applyCollisions(balls, ballRad, lastCollisions);
        moveBalls(balls, frameTime/40, lTop, wTop, wBorder);
        drawBalls(balls, textures);



        int numSteps = 100;
        int cueLength = 2.3;
        float arrX_1[numSteps+1];
        float arrY_1[numSteps+1];

        if (!balls[0]->isMoving())
            drawCue(numSteps, 0.04, cueLength, arrX_1, arrY_1, balls[0], cueAngB + cueAngA, cueRotX, cueRotY, cueRotZ, strength);



        int xm,ym;
        SDL_GetMouseState(&xm, &ym);
        while(SDL_PollEvent(&event)){
            switch(event.type){

            case SDL_MOUSEBUTTONDOWN:
                if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3)){
                    moveCam=true;
                }
                if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)){
                    moveCue=true;
                }

                break;
            case SDL_MOUSEBUTTONUP:
                moveCam=false;
                moveCue=false;
                break;
            case SDL_MOUSEMOTION:
                if(moveCam){
                    if (event.motion.yrel<0 && angb < 80 )
                        angb-=event.motion.yrel*0.4;//factor de ajuste: 0,4
                    else if (event.motion.yrel>=0 && angb > -80)
                        angb-=event.motion.yrel*0.4;//factor de ajuste: 0,4
                    anga+=event.motion.xrel*0.4;//factor de ajuste: 0,4
                    actualizarCam(x,y,z,angb,anga,rad);
                }
                if(moveCue){
                    cueAngA+=event.motion.xrel*0.4;//factor de ajuste: 0,4
                    cueAngB+=event.motion.yrel*0.4;//factor de ajuste: 0,4
                    actualizarCue(cueRotX, cueRotY, cueRotZ, cueAngB, cueAngA, cueLength + ballRad);
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
                case SDLK_s:{
                    rad-=.05;//factor de ajuste: 0,05
                    actualizarCam(x,y,z,angb,anga,rad);
                    }
                    break;
                case SDLK_w:{
                    if(rad<0)
                        rad+=.05;//factor de ajuste: 0,05
                    actualizarCam(x,y,z,angb,anga,rad);
                    }
                    break;
                case SDLK_UP:{
                        if (!balls[0]->isMoving())
                            hitBall(balls[0], cueRotX, cueRotY, cueRotZ, cueAngA, cueAngB, strength);
                    }
                    break;

                case SDLK_LEFT:{
                        if (strength > 4)
                            strength -= 2;
                    }
                    break;
                case SDLK_RIGHT:{
                    if (strength < 20)
                        strength += 4;
                    }
                    break;

                case SDLK_F11:{
                    flags ^= SDL_FULLSCREEN;
                    if(SDL_SetVideoMode(640, 480, 32, flags)==NULL) {
                        fprintf(stderr, "No se pudo establecer el modo de video: %s\n", SDL_GetError());
                        exit(1);
                    }

                    glMatrixMode(GL_PROJECTION);

                    float color = 0;
                    glClearColor(color, color, color, 1);

                    gluPerspective(45, 640/480.f, 0.1, 100);

                    glMatrixMode(GL_MODELVIEW);
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

        // Force 60fps cap
        currentTime = clock();
        frameTime = (float)(currentTime - lastFrameTime);
        if (frameTime < 17)
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(17- frameTime)));


    }while(!quit);
    return 0;
}
