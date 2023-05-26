#include "camera.h"

#ifdef __PSP__
#include "../../psp/gu2gl.h"

void Camera::Apply()
{
    glMatrixMode(GL_VIEW);
    glLoadIdentity();
    gluRotateX(pitch / 180.0f * 3.14159f);

    gluRotateY((yaw + 180) / 180.0f * 3.14159f);
    // glTranslatef(x, y, z);
    glTranslatef(x, y, -z);

    glMatrixMode(GL_MODEL);
    glLoadIdentity();
}
#endif

#ifdef __vita__
#include <vitaGL.h>
void Camera::Apply()
{
    // glLoadIdentity();
    // gluRotateX(pitch / 180.0f * 3.14159f);

    // gluRotateY((yaw + 180) / 180.0f * 3.14159f);
    // glTranslatef(x, y, -z);
    gluLookAt(x, y, z, x, y, z + 1, 0, 1, 0);
}

#endif