#ifndef BALL_H
#define BALL_H
#include "SDL/SDL_opengl.h"
#include "Color.h"
#include "Point.h"
#include "Rotation.h"
#include <vector>
#include <math.h>

#define M_PI 3.1415926

using namespace std;

class Ball{
    private:
        double posX;
        double posY;
        double posZ;
        double velX;
        double velY;
        double velZ;
        double rad;
        double mass;
        Color* color;
        vector<Rotation*> rotations;
        bool inHole;
        bool isWhite;
        bool checkEntersHole(float, float);
        void checkTableCollision(float, float);
        void applyRotation(double);
        void decreaseVelocity(double);
        bool hasSameDirection(float, float);

    public:
        Ball(double, double, double, double, double, Color*, bool);
        void setVelocity(double, double, double);
        void setVelocity(Point*);
        void draw(float, float, GLuint);
        bool updatePosAndVel(double, double, double, Ball**);
        double getPosX();
        double getPosY();
        double getPosZ();
        double getRad();
        double getMass();
        bool isMoving();
        Point* getPos();
        Point* getVel();
        void setPos(Point*);
        bool isInHole();
        void setInHole(bool);
        bool isWhiteBall();
};

#endif
