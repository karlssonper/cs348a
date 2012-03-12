#ifndef CAMERA_H
#define CAMERA_H

#ifndef M_PI
#define M_PI 3.1415926535897932384f
#endif

class Camera {

public:
    Camera(float _x, float _y, float _z, float _yaw, float _pitch) 
       : posX(_x), posY(_y), posZ(_z), yaw(_yaw), pitch(_pitch) {}

    void yawInc(float _y) {  yaw += _y; }

    void pitchInc(float _p) { pitch += _p; }

    void walkForward(float _d) {
        posX -= _d * sin((M_PI/180.f)*yaw);
        posY += _d * sin((M_PI/180.f)*pitch);
        posZ += _d * cos((M_PI/180.f)*yaw);
    }

    void walkBackwards(float _d) { walkForward(-_d); }

    void strafeLeft(float _d) {
        posX -= _d * sin((M_PI/180.f)*(yaw-90.f));
        posZ += _d * cos((M_PI/180.f)*(yaw-90.f));
    }

    void strafeRight(float _d) { strafeLeft(-_d); }

    void lookThrough() {
        glLoadIdentity();
        glRotatef(pitch, 1.f, 0.f, 0.f);
        glRotatef(yaw, 0.f, 1.f, 0.f);
        glTranslatef(posX, posY, posZ);
    }

    float posX, posY, posZ;
    float yaw;
    float pitch;
};

#endif
