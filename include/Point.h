#ifndef POINT_H
#define POINT_H

using namespace std;

class Point{
    private:
        float x;
        float y;
        float z;
    public:
        Point(float, float, float);
        float getX();
        float getY();
        float getZ();
};

#endif
