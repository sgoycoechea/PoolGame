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

 void drawQuad(vector<Point*> points, Color* color){
    glBegin(GL_QUADS);
        glColor3ub( color->getR(), color->getG(), color->getB());
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

vector<Ball*> initializeBalls(float ballRad, float ballSeparation){
    Color* redColor = new Color(70,10,10);
    Color* whiteColor = new Color(230, 230, 230);

    // White ball
    Ball* whiteBall = new Ball(-2, ballRad, 0, ballRad, whiteColor);
    // First line
    Ball* ball1 = new Ball(1, ballRad, 0, ballRad, redColor);
    // Second line
    Ball* ball2 = new Ball(1+ballSeparation, ballRad, ballRad, ballRad, redColor);
    Ball* ball3 = new Ball(1+ballSeparation, ballRad, -ballRad, ballRad, redColor);
    // Third line
    Ball* ball4 = new Ball(1+ballSeparation*2, ballRad, 0, ballRad, redColor);
    Ball* ball5 = new Ball(1+ballSeparation*2, ballRad, ballRad*2, ballRad, redColor);
    Ball* ball6 = new Ball(1+ballSeparation*2, ballRad, -ballRad*2, ballRad, redColor);
    // Forth line
    Ball* ball7 = new Ball(1+ballSeparation*3, ballRad, ballRad, ballRad, redColor);
    Ball* ball8 = new Ball(1+ballSeparation*3, ballRad, -ballRad, ballRad, redColor);
    Ball* ball9 = new Ball(1+ballSeparation*3, ballRad, ballRad + ballRad*2, ballRad, redColor);
    Ball* ball10 = new Ball(1+ballSeparation*3, ballRad, -ballRad - ballRad*2, ballRad, redColor);
    // Fifth line
    Ball* ball11 = new Ball(1+ballSeparation*4, ballRad, 0, ballRad, redColor);
    Ball* ball12 = new Ball(1+ballSeparation*4, ballRad, ballRad*2, ballRad, redColor);
    Ball* ball13 = new Ball(1+ballSeparation*4, ballRad, ballRad*2*2, ballRad, redColor);
    Ball* ball14 = new Ball(1+ballSeparation*4, ballRad, -ballRad*2, ballRad, redColor);
    Ball* ball15 = new Ball(1+ballSeparation*4, ballRad, -ballRad*2*2, ballRad, redColor);

    vector<Ball*> balls;
    balls.push_back(whiteBall);
    balls.push_back(ball1);
    balls.push_back(ball2);
    balls.push_back(ball3);
    balls.push_back(ball4);
    balls.push_back(ball5);
    balls.push_back(ball6);
    balls.push_back(ball7);
    balls.push_back(ball8);
    balls.push_back(ball9);
    balls.push_back(ball10);
    balls.push_back(ball11);
    balls.push_back(ball12);
    balls.push_back(ball13);
    balls.push_back(ball14);
    balls.push_back(ball15);
    return balls;
}

void writeOutput(string text){
    ofstream myfile;
    myfile.open ("output.txt");
    myfile << text;
    myfile.close();
}

void drawBalls(vector<Ball*> balls, GLuint* textures){
    int i = 0;
    for (std::vector<Ball*>::iterator it = balls.begin() ; it != balls.end(); ++it){
        if (i == 0)
            (*it)->draw(50,50,textures[10]);
        else
            (*it)->draw(50,50, textures[i - 1]);
        i++;
    }
}

void moveBalls(vector<Ball*> balls, float time, float lTop, float wTop, float wBorder){
    for (std::vector<Ball*>::iterator it = balls.begin() ; it != balls.end(); ++it)
        (*it)->updatePosAndVel(time, lTop, wTop, wBorder);
}

void drawTable(float lTop, float wTop, float lBottom, float wBottom, float h, float wBorder, float hBorder){

    Color* greenColor = new Color(35,123,86);
    Color* brownColor1 = new Color(60,36,21);
    Color* brownColor2 = new Color(60,36,21);

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
    drawPrism(table, greenColor, brownColor1);
    drawPrism(border1, brownColor2, brownColor2);
    drawPrism(border2, brownColor2, brownColor2);
    drawPrism(border3, brownColor2, brownColor2);
    drawPrism(border4, brownColor2, brownColor2);
}

GLuint* loadTextures(){

    GLuint* textures = new GLuint[15];

    // Get the texture of all 15 balls
    for (int i = 1; i <= 15; i++){
        string archivo = "images/ball" + to_string(i) + ".jpg";

        FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(archivo.c_str());
        FIBITMAP* bitmap = FreeImage_Load(fif, archivo.c_str());
        bitmap = FreeImage_ConvertTo24Bits(bitmap);

        int texW = FreeImage_GetWidth(bitmap);
        int texH = FreeImage_GetHeight(bitmap);

        void* datos = FreeImage_GetBits(bitmap);

        GLuint textura;
        glGenTextures(1, &textura);

        glBindTexture(GL_TEXTURE_2D, textura);

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texW, texH, 0, GL_BGR, GL_UNSIGNED_BYTE, datos);

        delete datos;

        textures[i-1] = textura;
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
    bool moverCam=false;
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
    float ballRad = lTop / 50;
    float ballSeparation = ballRad*1.75;

    float lBottom = 5;
    float wBottom = 3;

    float h = 1.6;
    float wBorder = h/6;
    float hBorder = h/4;
    // -----------------------------------

    auto lastFrameTime = clock();
    bool fin=false;
    SDL_Event evento;
    vector<Ball*> balls = initializeBalls(ballRad, ballSeparation);
    balls[0]->setVelocity(7,0,10);
    do{


        auto currentTime = clock();
        float frameTime = (float)(currentTime - lastFrameTime);
        lastFrameTime = clock();



        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(x,y,-z,0,0,0,0,1,0);


        drawTable(lTop, wTop, lBottom, wBottom, h, wBorder, hBorder);
        moveBalls(balls, frameTime/40, lTop, wTop, wBorder);
        drawBalls(balls, textures);



        int xm,ym;
        SDL_GetMouseState(&xm, &ym);
        while(SDL_PollEvent(&evento)){
            switch(evento.type){

            case SDL_MOUSEBUTTONDOWN:
                if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3)){
                    moverCam=true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                moverCam=false;
                break;
            case SDL_MOUSEMOTION:
                if(moverCam){
                    if (evento.motion.yrel<0 && angb < 80 )
                        angb-=evento.motion.yrel*0.4;//factor de ajuste: 0,4
                    else if (evento.motion.yrel>=0 && angb > -80)
                        angb-=evento.motion.yrel*0.4;//factor de ajuste: 0,4
                    anga+=evento.motion.xrel*0.4;//factor de ajuste: 0,4
                    actualizarCam(x,y,z,angb,anga,rad);
                }
                break;
            case SDL_QUIT:
                fin = true;
                break;
            case SDL_KEYDOWN:{
                switch(evento.key.keysym.sym){
                case SDLK_ESCAPE:
                    fin = true;
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

        // Force 60fps
        currentTime = clock();
        frameTime = (float)(currentTime - lastFrameTime);
        if (frameTime < 17)
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(17- frameTime)));


    }while(!fin);
    return 0;
}
