#ifndef BALL_H
#define BALL_H
#include "Color.h"
#include "Point.h"
#include "SDL/SDL_opengl.h"

using namespace std;

class Ball{
    private:
        double posX;
        double posY;
        double posZ;
        double velX;
        double velY;
        double velZ;
        double rotX;
        double rotY;
        double rotZ;
        double rotAng;
        double rad;
        double mass;
        Color* color;
    public:
        Ball(double, double, double, double, double, Color*);
        void setVelocity(double, double, double);
        void setVelocity(Point*);
        void draw(float, float, GLuint);
        void updatePosAndVel(double, double, double, double, Ball**);
        double getPosX();
        double getPosY();
        double getPosZ();
        double getMass();
        bool isMoving();
        Point* getPos();
        Point* getVel();
        void setPos(Point*);
};

#endif
