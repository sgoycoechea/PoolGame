#include "../include/Ball.h"
#include "../include/Rotation.h"
#include <math.h>
#include "SDL/SDL_opengl.h"
#define M_PI 3.1415926
#include <fstream>

using namespace std;

Ball::Ball(double x, double y, double z, double rad, double mass, Color* color, bool isWhiteBall){
    this->posX = x;
    this->posY = y;
    this->posZ = z;
    this->rad = rad;
    this->mass = mass;
    this->velX = 0;
    this->velY = 0;
    this->velZ = 0;
    this->color = color;
    this->inHole = false;
    this->isWhiteBall = isWhiteBall;
}

void Ball::setVelocity(double x, double y, double z){
    this->velX = x;
    this->velY = y;
    this->velZ = z;
}

void Ball::setVelocity(Point* velVector){
    this->velX = velVector->getX();
    this->velY = velVector->getY();
    this->velZ = velVector->getZ();
}

bool Ball::isMoving(){
    return (!inHole && (velX != 0 || velY != 0 || velZ != 0));
}

bool Ball::checkEntersHole(float lTop, float wTop){
    float d = 1.5;
    if ( (posX > lTop/2 - rad*d || posX < -lTop/2 + rad*d || (posX < rad*d/2 && posX > -rad*d/2 )) && (posZ > wTop/2 - rad*d || posZ < -wTop/2 + rad*d)){
        velX = 0;
        velY = 0;
        velZ = 0;

        if (isWhiteBall){
            posX = -2;
            posZ = 0;
        }

        inHole = true;
        return true;
    }

    return false;
}

bool Ball::isInHole(){
    return inHole;
}

void Ball::setInHole(bool inHole){
    this->inHole = inHole;
}

void Ball::updatePosAndVel(double time, double lTop, double wTop, Ball** balls){

    double velDecrease = time / 3;
    double posFactor = time / 100;

    if (inHole) return;
    if (checkEntersHole(lTop, wTop)) return;

    // Ball is against a border
    if (posX >= lTop/2 - rad || posX <= -lTop/2 + rad){
        velX = -velX;

        if (posX >= lTop/2 - rad)
            posX = lTop/2 - rad;
        else
            posX = -lTop/2 + rad;
    }

    if (posZ >= wTop/2 - rad || posZ <= -wTop/2 + rad){
        velZ = -velZ;

        if (posZ >= wTop/2 - rad)
            posZ = wTop/2 - rad;
        else
            posZ = -wTop/2 + rad;
    }

    double diffX = posX;
    double diffY = posY;
    double diffZ = posZ;
    posX += velX * posFactor;
    posZ += velZ * posFactor;

    diffX = posX - diffX;
    diffY = posY - diffY;
    diffZ = posZ - diffZ;
    double distanceMoved = sqrt(pow(diffX,2) + pow(diffY,2) + pow(diffZ,2));
    double circumference = 2 * M_PI * rad;

    // Ball rotation
    if(velX != 0 || velY != 0 || velZ != 0){

        float magnitude = sqrt(pow(velX,2) + pow(velY,2) + pow(velZ,2));
        float rotX = velZ / magnitude;
        float rotY = -velY / magnitude;
        float rotZ = -velX / magnitude;
        float rotAng = distanceMoved / circumference * 180;

        Rotation* lastRotation = (Rotation*)rotations.back();

        // If the last rotation had the same direction
        if (rotations.size() > 0 && ((int)(lastRotation->getDirection()->getX() - rotX)  == 0) && ((int)(lastRotation->getDirection()->getY() - rotY) == 0) && ((int)(lastRotation->getDirection()->getZ() - rotZ) == 0)){
            lastRotation->setAngle(lastRotation->getAngle() + rotAng);
        }
        else{
            Point* rotDir = new Point(rotX, rotY, rotZ);
            Rotation* newRot = new Rotation(rotAng, rotDir);
            rotations.push_back(newRot);
        }

        // Decrease velocity
        if (velDecrease > 0){
            if (magnitude < velDecrease){
                velX = 0;
                velY = 0;
                velZ = 0;
            }

            else{
                float decreaseFactor = magnitude / (magnitude - velDecrease);
                velX /= decreaseFactor;
                velY /= decreaseFactor;
                velZ /= decreaseFactor;
            }

        }
    }
}

double Ball::getPosX(){
    return posX;
}

double Ball::getPosY(){
    return posY;
}

double Ball::getPosZ(){
    return posZ;
}

double Ball::getMass(){
    return mass;
}

Point* Ball::getPos(){
    return new Point(posX, posY, posZ);
}

Point* Ball::getVel(){
    return new Point(velX, velY, velZ);
}

double Ball::getRad(){
    return rad;
}

void Ball::setPos(Point* pos){
    posX = pos->getX();
    posY = pos->getY();
    posZ = pos->getZ();
}

void Ball::draw(float lats, float longs, GLuint texture) {

    if (inHole) return;

    double i, j;
    for(i = 0; i <= lats; i++)
    {
        double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);

        double lat1 = M_PI * (-0.5 + (double) i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        glPushMatrix();

        glTranslatef(posX, posY, posZ); // Ball movement

        // Ball rotation when it moves
        for (std::vector<Rotation*>::reverse_iterator  it = rotations.rbegin() ; it != rotations.rend(); ++it){
            Rotation* rot = (Rotation*)(*it);
            glRotatef(rot->getAngle(), rot->getDirection()->getX(), rot->getDirection()->getY(), rot->getDirection()->getZ()); // Ball rotation when it moves
        }

        // To fix texture initial position
        glRotatef(90,0,1,0);
        glRotatef(-90,1,0,0);

        glBegin(GL_QUAD_STRIP);
        glColor3ub( color->getR(), color->getG(), color->getB());

        for(j = 0; j <= longs; j++)
        {
            double lng = 2 * M_PI * (double) (j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);

            double s1, s2, t;
            s1 = ((double) i) / lats;
            s2 = ((double) i + 1) / lats;
            t = ((double) j) / longs;

            glTexCoord2d(s1, t);
            glNormal3d(x * zr0, y * zr0, z0);
            glVertex3d(rad*x * zr0, rad*y * zr0, rad*z0);

            glTexCoord2d(s2, t);
            glNormal3d(x * zr1, y * zr1, z1);
            glVertex3d(rad*x * zr1,rad* y * zr1, rad*z1);
        }
        glEnd();
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
    }
 }
