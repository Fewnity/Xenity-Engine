#include "camera.h"

#ifdef __PSP__
#include "../../psp/gu2gl.h"

void Camera::Apply()
{
    glMatrixMode(GL_VIEW);
    glLoadIdentity();

    ScePspFVector3 v = {x, y, z};
    gluRotateX(pitch / 180.0f * 3.14159f);
    gluRotateY(yaw / 180.0f * 3.14159f);
    gluTranslate(&v);

    glMatrixMode(GL_MODEL);
    glLoadIdentity();
}
#endif

#ifdef __psvita__

void Camera::Apply()
{
}

#endif