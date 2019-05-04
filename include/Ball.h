#ifndef BALL_H
#define BALL_H
#include "Color.h"
#include "SDL/SDL_opengl.h"

using namespace std;

class Ball{
    private:
        float posX;
        float posY;
        float posZ;
        float velX;
        float velY;
        float velZ;
        float rotX;
        float rotY;
        float rotZ;
        float rotAng;
        float rad;
        Color* color;
    public:
        Ball(float, float, float, float, Color*);
        void setVelocity(float, float, float);
        void draw(float, float, GLuint);
        void updatePosAndVel(float, float, float, float);
};

#endif
