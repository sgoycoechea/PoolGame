#include "../include/Rotation.h"


using namespace std;

Rotation::Rotation(float angle, Point* direction){
    this->angle = angle;
    this->direction = direction;
}

Point* Rotation::getDirection() {
  return direction;
}

float Rotation::getAngle() {
  return angle;
}

void Rotation::setAngle(float angle) {
  this->angle = angle;
}
