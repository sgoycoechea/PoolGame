#ifndef ROTATION_H
#define ROTATION_H
#include "../include/Point.h"

using namespace std;

class Rotation{
    private:
        float angle;
        Point* direction;
    public:
        Rotation(float, Point*);
        float getAngle();
        Point* getDirection();
        void setAngle(float);
};

#endif
