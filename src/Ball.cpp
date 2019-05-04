#include "../include/Ball.h"
#include <math.h>
#include "SDL/SDL_opengl.h"
#define M_PI 3.1415926

using namespace std;

Ball::Ball(float x, float y, float z, float rad, Color* color){
    this->posX = x;
    this->posY = y;
    this->posZ = z;
    this->rad = rad;
    this->velX = 0;
    this->velY = 0;
    this->velZ = 0;
    this->rotX = 0;
    this->rotY = 0;
    this->rotZ = 0;
    this->rotAng = 0;
    this->color = color;
}

void Ball::setVelocity(float x, float y, float z){
    this->velX = x;
    this->velY = y;
    this->velZ = z;
}


void Ball::updatePosAndVel(float time, float lTop, float wTop, float wBorder){

    float velDecrease = time / 20;
    float posFactor = time / 100;

    // Ball is against a border
    if (posX >= lTop/2 - rad - wBorder || posX <= -lTop/2 + rad + wBorder)
        velX = -velX;
    if (posZ >= wTop/2 - rad - wBorder || posZ <= -wTop/2 + rad + wBorder)
        velZ = -velZ;


    float diffX = posX;
    float diffY = posY;
    float diffZ = posZ;
    posX += velX * posFactor;
    posZ += velZ * posFactor;


    diffX = posX - diffX;
    diffY = posY - diffY;
    diffZ = posZ - diffZ;
    float distanceMovedInXZPlane = sqrt(pow(diffX,2) + pow(diffZ,2));
    float distanceMoved = sqrt(pow(distanceMovedInXZPlane,2) + pow(diffY,2));
    float circumference = 2 * M_PI * rad;

    // Ball rotation
    if(velX != 0 || velY != 0 || velZ != 0){
        rotX = velZ;
        rotY = -velY;
        rotZ = -velX;
        rotAng += distanceMoved / circumference * 180;
    }

    // Decrease velocity
    if (velDecrease > 0){

        // X velocity
        if (velX > 0){
            velX -= velDecrease;
            if (velX < 0) velX = 0;
        }
        else if (velX < 0){
            velX += velDecrease;
            if (velX > 0) velX = 0;
        }

        // Y velocity
        if (velY > 0){
            velY -= velDecrease;
            if (velY < 0) velY = 0;
        }
        else if (velY < 0){
            velY += velDecrease;
            if (velY > 0) velY = 0;
        }

        // Z velocity
        if (velZ > 0){
            velZ -= velDecrease;
            if (velZ < 0) velZ = 0;
        }
        else if (velZ < 0){
            velZ += velDecrease;
            if (velZ > 0) velZ = 0;
        }
    }
}

void Ball::draw(float lats, float longs, GLuint texture) {
    float i, j;
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
        glRotatef(rotAng, rotX, rotY, rotZ); // Ball rotation when it moves

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
