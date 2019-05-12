#include "../include/Ball.h"

using namespace std;

Ball::Ball(double x, double y, double z, double rad, double mass, Color* color, bool isWhite){
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
    this->isWhite = isWhite;
}

bool Ball::isWhiteBall(){
    return isWhite;
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

bool Ball::checkEntersHole(float tableLength, float tableWidth){
    float d = 1.5;
    if ( (posX > tableLength/2 - rad*d || posX < -tableLength/2 + rad*d || (posX < rad*d/2 && posX > -rad*d/2 )) && (posZ > tableWidth/2 - rad*d || posZ < -tableWidth/2 + rad*d) ){
        velX = 0;
        velY = 0;
        velZ = 0;

        if (isWhite){
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

// If ball collides against a border of the table, change its velocity direction
void Ball::checkTableCollision(float tableLength, float tableWidth){

    if (posX >= tableLength/2 - rad || posX <= -tableLength/2 + rad){
        velX = -velX;
        if (posX >= tableLength/2 - rad)
            posX = tableLength/2 - rad;
        else
            posX = -tableLength/2 + rad;
    }

    if (posZ >= tableWidth/2 - rad || posZ <= -tableWidth/2 + rad){
        velZ = -velZ;
        if (posZ >= tableWidth/2 - rad)
            posZ = tableWidth/2 - rad;
        else
            posZ = -tableWidth/2 + rad;
    }
}

bool Ball::hasSameDirection(float rotX, float rotZ){

    Rotation* lastRotation = (Rotation*)rotations.back();

    bool res = lastRotation->getDirection()->getX() - rotX < 0.01 && lastRotation->getDirection()->getX() - rotX > -0.01
                && lastRotation->getDirection()->getZ() - rotZ < 0.01 && lastRotation->getDirection()->getZ() - rotZ > -0.01;

    return res;
}

void Ball::applyRotation(double distanceMoved){

    double circumference = 2 * M_PI * rad;

    if(velX != 0 || velY != 0 || velZ != 0){

        float magnitude = sqrt(pow(velX,2) + pow(velY,2) + pow(velZ,2));
        float rotX = velZ / magnitude;
        float rotY = -velY / magnitude;
        float rotZ = -velX / magnitude;
        float rotAng = distanceMoved / circumference * 180;



        // If the last rotation had the same direction
        if (rotations.size() > 0 && hasSameDirection(rotX, rotZ)){
            Rotation* lastRotation = (Rotation*)rotations.back();
            lastRotation->setAngle(lastRotation->getAngle() + rotAng);
        }
        else{
            Point* rotDir = new Point(rotX, rotY, rotZ);
            Rotation* newRot = new Rotation(rotAng, rotDir);
            rotations.push_back(newRot);
        }
    }
}

void Ball::decreaseVelocity(double time){
    double velDecrease = time / 3;
    float magnitude = sqrt(pow(velX,2) + pow(velY,2) + pow(velZ,2));

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

bool Ball::updatePosAndVel(double time, double tableLength, double tableWidth, Ball** balls){

    double posFactor = time / 100;

    if (inHole) return false;
    if (checkEntersHole(tableLength, tableWidth)) return true;

    checkTableCollision(tableLength, tableWidth);

    // Update position
    double diffX = velX * posFactor;
    double diffY = velY * posFactor;
    double diffZ = velZ * posFactor;
    posX += velX * posFactor;
    posZ += velZ * posFactor;

    double distanceMoved = sqrt(pow(diffX,2) + pow(diffY,2) + pow(diffZ,2));
    applyRotation(distanceMoved);

    decreaseVelocity(time);

    return false;
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

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPushMatrix();

    // Ball movement
    glTranslatef(posX, posY, posZ);

    // Ball rotation when it moves
    for (std::vector<Rotation*>::reverse_iterator  it = rotations.rbegin() ; it != rotations.rend(); ++it){
        Rotation* rot = (Rotation*)(*it);
        glRotatef(rot->getAngle(), rot->getDirection()->getX(), rot->getDirection()->getY(), rot->getDirection()->getZ()); // Ball rotation when it moves
    }

    // To fix texture initial position
    glRotatef(110,0,1,0);
    glRotatef(-90,1,0,0);

    // Draw sphere
    for(int i = 0; i <= lats; i++)
    {
        double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);

        double lat1 = M_PI * (-0.5 + (double) i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        glColor3ub( color->getR(), color->getG(), color->getB());
        for(int j = 0; j <= longs; j++)
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

    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
 }
