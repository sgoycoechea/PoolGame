#include "../include/Point.h"

using namespace std;

Point::Point(float x, float y, float z){
    this->x = x;
    this->y = y;
    this->z = z;
}

float Point::getX() {
  return x;
}

float Point::getY() {
  return y;
}

float Point::getZ() {
  return z;
}
