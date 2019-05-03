#ifndef BALL_H
#define BALL_H
#include "Color.h"

using namespace std;

class Ball{
    private:
        float posX;
        float posY;
        float posZ;
        float velX;
        float velY;
        float velZ;
        float rad;
        Color* color;
    public:
        Ball(float, float, float, float, Color*);
        float getPosX();
        float getPosY();
        float getPosZ();
        float getVelX();
        float getVelY();
        float getVelZ();
        float getRad();
        void setVelocity(float, float, float);
        void draw(float, float);
        void updatePosAndVel(float, float, float, float);
};

#endif
